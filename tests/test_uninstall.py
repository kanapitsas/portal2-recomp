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

    def test_saves_to_trash_only_on_yes(self):
        self.run_script('y\ny\n')
        self.assertFalse(self.support.exists())
        self.assertFalse(self.explicit.exists())
        self.assertTrue((self.home / '.Trash/Portal 2 Natif/userdata/portal2/SAVE').is_dir())
        self.assertTrue((self.home / '.Trash/Portal2Recomp').is_dir())
        self.assertTrue(self.game.exists())

    def test_nothing_installed(self):
        self.run_script('y\ny\n')
        out = self.run_script('y\ny\n')
        self.assertIn('nothing installed', out)


if __name__ == '__main__':
    unittest.main()
