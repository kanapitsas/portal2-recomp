import importlib.util
import json
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest
from unittest import mock

REPO = Path(__file__).resolve().parents[1]
spec = importlib.util.spec_from_file_location('make_app', REPO / 'tools/make_app.py')
make_app = importlib.util.module_from_spec(spec)
spec.loader.exec_module(make_app)


class MakeAppTests(unittest.TestCase):
    """Une fausse arborescence : <tmp>/repo (ROOT), <tmp>/work (ancien défaut), <tmp>/support."""

    def setUp(self):
        directory = tempfile.TemporaryDirectory()
        self.addCleanup(directory.cleanup)
        self.tmp = Path(directory.name).resolve()
        self.root = self.tmp / 'repo'
        (self.root / 'build/game/prep').mkdir(parents=True)
        (self.root / 'build/game/prep/layout.json').write_text('{}')
        (self.root / 'build/game/prep/engine.so').write_bytes(b'\x7fELF')
        (self.root / 'build/portal2-native').write_text('')
        (self.root / 'config').mkdir()
        (self.root / 'tools').mkdir(exist_ok=True)
        (self.root / 'tools/language.py').write_text('')
        steam = self.tmp / 'steamworks/libsteam_api.dylib'
        steam.parent.mkdir()
        steam.write_bytes(b'steam')
        self.config = {'game_root': str(self.tmp / 'Portal 2'), 'linux_root': str(self.tmp / 'depot'),
                       'steam_api_dylib': str(steam)}
        self.support = self.tmp / 'support'
        self.old = self.tmp / 'work/portal2-userdata'
        # The fake binary is not Mach-O: skip dylib bundling and signing here.
        for patch in (mock.patch.object(make_app, 'ROOT', self.root), mock.patch.object(make_app, 'SUPPORT', self.support),
                      mock.patch.object(make_app, 'bundle_libraries', lambda app, binary: None),
                      mock.patch.object(make_app, 'sign_app', lambda app: None)):
            patch.start()
            self.addCleanup(patch.stop)

    def run_main(self):
        (self.root / 'config/local.json').write_text(json.dumps(self.config))
        dest = self.tmp / 'Applications'
        with mock.patch.object(sys, 'argv', ['make_app.py', '--dest', str(dest)]), mock.patch('builtins.print'):
            make_app.main()
        return dest / 'Portal 2 Natif.app'

    def test_launcher_reads_nothing_from_repo(self):
        app = self.run_main()
        self.assertTrue((app / 'Contents/Resources/language.py').exists())
        self.assertTrue((app / 'Contents/Resources/local.json').exists())
        self.assertNotIn(str(self.root), (app / 'Contents/MacOS/launcher').read_text())

    def test_steam_api_bundled_and_exported(self):
        # The copied executable cannot find the repo, so the launcher must name the bundled Steam library.
        app = self.run_main()
        self.assertEqual((app / 'Contents/Frameworks/libsteam_api.dylib').read_bytes(), b'steam')
        self.assertIn('P2_STEAM_API_DYLIB="$HERE/../Frameworks/libsteam_api.dylib"',
                      (app / 'Contents/MacOS/launcher').read_text())

    def test_failed_rebuild_keeps_existing_app(self):
        app = self.run_main()
        Path(self.config['steam_api_dylib']).unlink()   # fails late, after the bundle was started
        with self.assertRaises(SystemExit):
            self.run_main()
        self.assertTrue((app / 'Contents/Info.plist').is_file())   # the previous, complete app
        self.assertFalse(any(p.name.startswith('.') for p in app.parent.iterdir()))

    def test_prep_copied_into_bundle(self):
        app = self.run_main()
        self.assertEqual((app / 'Contents/Resources/prep/engine.so').read_bytes(), b'\x7fELF')
        launcher = (app / 'Contents/MacOS/launcher').read_text()
        self.assertIn('--images "$(dirname "$HERE")/Resources/prep"', launcher)
        self.assertNotIn(str(self.root / 'build/game/prep'), launcher)
        self.assertIn(str(self.support / 'userdata'), launcher)
        self.assertEqual(subprocess.run(['bash', '-n', str(app / 'Contents/MacOS/launcher')]).returncode, 0)

    def test_old_saves_copied_once_not_moved(self):
        (self.old / 'portal2/SAVE').mkdir(parents=True)
        (self.old / 'portal2/SAVE/quick.sav').write_text('old')
        self.run_main()
        new = self.support / 'userdata/portal2/SAVE/quick.sav'
        self.assertEqual(new.read_text(), 'old')
        self.assertTrue((self.old / 'portal2/SAVE/quick.sav').exists())
        new.write_text('played since')
        self.run_main()  # déjà migré : ne pas écraser
        self.assertEqual(new.read_text(), 'played since')

    def test_explicit_user_dir_respected(self):
        (self.old / 'portal2').mkdir(parents=True)
        self.config['user_dir'] = str(self.tmp / 'mine')
        launcher = (self.run_main() / 'Contents/MacOS/launcher').read_text()
        self.assertIn(str(self.tmp / 'mine'), launcher)
        self.assertFalse(self.support.exists())

    def test_d3d9metal_bundled_only_when_configured(self):
        frameworks = self.run_main() / 'Contents/Frameworks'
        self.assertFalse((frameworks / 'libd3d9metal.dylib').exists())
        library = self.tmp / 'mtld3d/libd3d9metal.dylib'
        library.parent.mkdir()
        library.write_bytes(b'metal')
        self.config['d3d9metal_dylib'] = str(library)
        frameworks = self.run_main() / 'Contents/Frameworks'
        self.assertEqual((frameworks / 'libd3d9metal.dylib').read_bytes(), b'metal')
        library.unlink()
        with self.assertRaises(SystemExit):
            self.run_main()

    def test_missing_prep_keeps_existing_app(self):
        app = self.run_main()
        (self.root / 'build/game/prep/layout.json').unlink()
        with self.assertRaises(SystemExit):
            self.run_main()
        self.assertTrue((app / 'Contents/MacOS/launcher').exists())


if __name__ == '__main__':
    unittest.main()
