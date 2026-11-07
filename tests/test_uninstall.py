import json
import os
from pathlib import Path
import subprocess
import tempfile
import unittest

SCRIPT = Path(__file__).resolve().parents[1] / 'uninstall.sh'


class UninstallTests(unittest.TestCase):
    """uninstall.sh contre un faux dossier personnel (P2_UNINSTALL_HOME)."""

    def setUp(self):
        if subprocess.run(['pgrep', '-qx', 'portal2-native']).returncode == 0:
            self.skipTest('the game is running: uninstall.sh refuses to run')
        directory = tempfile.TemporaryDirectory()
        self.addCleanup(directory.cleanup)
        self.home = Path(directory.name).resolve()
        self.app = self.home / 'Applications/Portal 2 Natif.app'
        (self.app / 'Contents/MacOS').mkdir(parents=True)
        self.log = self.home / 'Library/Logs/Portal2Natif.log'
        self.log.parent.mkdir(parents=True)
        self.log.write_text('log')
        self.support = self.home / 'Library/Application Support/Portal 2 Natif'
        (self.support / 'userdata/portal2/SAVE').mkdir(parents=True)
        self.explicit = self.home / 'Library/Application Support/Portal2Recomp'
        self.explicit.mkdir()
        self.game = self.home / 'Steam/steamapps/common/Portal 2'
        self.game.mkdir(parents=True)
        self.config = self.home / 'local.json'
        self.config.write_text(json.dumps({'game_root': str(self.game), 'linux_root': str(self.home / 'depot_661'),
                                           'user_dir': str(self.explicit)}))

    def run_script(self, answers):
        env = dict(os.environ, P2_UNINSTALL_HOME=str(self.home), P2_CONFIG=str(self.config))
        result = subprocess.run(['bash', str(SCRIPT)], input=answers, env=env, capture_output=True, text=True)
        self.assertEqual(result.returncode, 0, result.stderr)
        return result.stdout

    def test_no_answer_keeps_everything(self):
        out = self.run_script('')
        for path in (self.app, self.log, self.support, self.explicit, self.game):
            self.assertTrue(path.exists(), path)
        self.assertIn(str(self.game), out)
        self.assertIn('saves kept', out)

    def test_app_only_keeps_saves_and_game(self):
        self.run_script('y\n\n')
        self.assertFalse(self.app.exists())
        self.assertFalse(self.log.exists())
        self.assertTrue((self.home / '.Trash/Portal 2 Natif.app').exists())
        for path in (self.support, self.explicit, self.game):
            self.assertTrue(path.exists(), path)

    def test_installer_build_folder_goes_with_the_app(self):
        build = self.home / 'Library/Application Support/Portal 2 Natif Build'
        (build / 'build').mkdir(parents=True)
        out = self.run_script('\n\n')
        self.assertIn('source and build', out)
        self.assertTrue(build.exists())
        self.run_script('y\n\n')
        self.assertFalse(build.exists())
        self.assertTrue(self.support.exists())

    def test_saves_to_trash_only_on_yes(self):
        self.run_script('y\ny\n')
        self.assertFalse(self.support.exists())
        self.assertFalse(self.explicit.exists())
        self.assertTrue((self.home / '.Trash/Portal 2 Natif/userdata/portal2/SAVE').is_dir())
        self.assertTrue((self.home / '.Trash/Portal2Recomp').is_dir())
        self.assertTrue(self.game.exists())

    def set_user_dir(self, path):
        config = json.loads(self.config.read_text())
        config['user_dir'] = str(path)
        self.config.write_text(json.dumps(config))

    def test_user_dir_overlapping_shared_folders_is_never_trashed(self):
        # A user_dir equal to, inside or around the game folder, or the home itself, must survive two yeses.
        link = self.home / 'game-link'
        link.symlink_to(self.game)
        for path in (self.game, self.game / 'portal2', self.game.parent, self.home, link):
            path.mkdir(parents=True, exist_ok=True)
            self.set_user_dir(path)
            self.run_script('n\ny\n')
            self.assertTrue(self.game.is_dir(), path)
            self.assertTrue(path.exists(), path)

    def test_hl2_checkout_removes_only_hl2(self):
        # An HL2 checkout must never offer Portal 2's app or saves.
        hl2_app = self.home / 'Applications/Half-Life 2 Natif.app'
        (hl2_app / 'Contents/MacOS').mkdir(parents=True)
        hl2_log = self.home / 'Library/Logs/HalfLife2Natif.log'
        hl2_log.write_text('log')
        hl2_support = self.home / 'Library/Application Support/Half-Life 2 Natif'
        (hl2_support / 'userdata/hl2_complete/save').mkdir(parents=True)
        hl2_game = self.home / 'Steam/steamapps/common/Half-Life 2'
        hl2_game.mkdir(parents=True)
        self.config.write_text(json.dumps({'game': 'hl2', 'game_root': str(hl2_game),
                                           'linux_root': str(self.home / 'depot_224')}))
        out = self.run_script('y\ny\n')
        for path in (hl2_app, hl2_log, hl2_support):
            self.assertFalse(path.exists(), path)
        self.assertTrue((self.home / '.Trash/Half-Life 2 Natif/userdata/hl2_complete/save').is_dir())
        for path in (self.app, self.log, self.support, self.explicit, self.game, hl2_game):
            self.assertTrue(path.exists(), path)
        self.assertIn('Half-Life 2 game files', out)
        self.assertIn('depot 224', out)
        self.assertNotIn('Portal 2 Natif', out)

    def test_hl2_never_offers_portal2_data_as_its_user_dir(self):
        # An HL2 config whose user_dir is Portal 2's folder (the old shared default) or inside it.
        for user_dir in (self.support, self.support / 'userdata'):
            self.config.write_text(json.dumps({'game': 'hl2', 'user_dir': str(user_dir)}))
            self.run_script('y\ny\n')
            self.assertTrue((self.support / 'userdata/portal2/SAVE').is_dir(), user_dir)

    def test_nothing_installed(self):
        self.run_script('y\ny\n')
        out = self.run_script('y\ny\n')
        self.assertIn('nothing installed', out)


if __name__ == '__main__':
    unittest.main()
