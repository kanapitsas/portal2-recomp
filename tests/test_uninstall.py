import json
import os
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest

SCRIPT = Path(__file__).resolve().parents[1] / 'uninstall.sh'
sys.path.insert(0, str(SCRIPT.parent / 'tools'))
import games  # noqa: E402


class UninstallTests(unittest.TestCase):
    """uninstall.sh contre un faux dossier personnel (P2_UNINSTALL_HOME)."""

    def setUp(self):
        # A game running for real does not stop runs against a fake home.
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

    def test_installer_build_folder_only_on_its_own_yes(self):
        build = self.home / 'Library/Application Support/Portal 2 Natif Build'
        (build / 'build').mkdir(parents=True)
        out = self.run_script('y\n\n\n')   # app yes, saves no, build folder no answer
        self.assertIn('source and build', out)
        self.assertFalse(self.app.exists())
        self.assertTrue(build.exists())
        self.run_script('n\ny\n')   # saves no, build folder yes
        self.assertFalse(build.exists())
        self.assertTrue(self.support.exists())

    def test_running_check_covers_every_game(self):
        text = SCRIPT.read_text()
        self.assertIn("pgrep -qx '" + '|'.join(games.EXECUTABLES) + "'", text)

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

    def add_hl2(self):
        self.hl2_app = self.home / 'Applications/Half-Life 2 Natif.app'
        (self.hl2_app / 'Contents/MacOS').mkdir(parents=True)
        self.hl2_log = self.home / 'Library/Logs/HalfLife2Natif.log'
        self.hl2_log.write_text('log')
        self.hl2_support = self.home / 'Library/Application Support/Half-Life 2 Natif'
        (self.hl2_support / 'userdata/hl2_complete/save').mkdir(parents=True)
        self.hl2_game = self.home / 'Steam/steamapps/common/Half-Life 2'
        self.hl2_game.mkdir(parents=True)
        config = json.loads(self.config.read_text())
        config['games'] = {'hl2': {'game_root': str(self.hl2_game), 'linux_root': str(self.home / 'depot_224')}}
        self.config.write_text(json.dumps(config))

    def test_both_games_in_one_run(self):
        self.add_hl2()
        out = self.run_script('y\ny\ny\ny\n')
        for path in (self.app, self.log, self.support, self.explicit, self.hl2_app, self.hl2_log, self.hl2_support):
            self.assertFalse(path.exists(), path)
        self.assertTrue((self.home / '.Trash/Half-Life 2 Natif/userdata/hl2_complete/save').is_dir())
        self.assertTrue((self.home / '.Trash/Portal 2 Natif/userdata/portal2/SAVE').is_dir())
        for path in (self.game, self.hl2_game):
            self.assertTrue(path.exists(), path)
        self.assertIn('Half-Life 2 game files', out)
        self.assertIn('depot 224', out)
        self.assertIn('depot 661', out)

    def test_each_game_answered_separately(self):
        # Keep Portal 2 (two no), remove Half-Life 2 (two yes).
        self.add_hl2()
        self.run_script('n\nn\ny\ny\n')
        for path in (self.app, self.log, self.support, self.explicit, self.game, self.hl2_game):
            self.assertTrue(path.exists(), path)
        for path in (self.hl2_app, self.hl2_log, self.hl2_support):
            self.assertFalse(path.exists(), path)

    def test_hl2_alone_and_older_hl2_config(self):
        # An older single-game HL2 config ("game": "hl2", keys at the top level).
        self.add_hl2()
        for path in (self.app, self.log, self.support):
            os.rename(path, self.tmp_away(path))
        self.config.write_text(json.dumps({'game': 'hl2', 'game_root': str(self.hl2_game),
                                           'linux_root': str(self.home / 'depot_224')}))
        out = self.run_script('y\ny\n')
        for path in (self.hl2_app, self.hl2_log, self.hl2_support):
            self.assertFalse(path.exists(), path)
        self.assertTrue(self.explicit.exists())   # Portal 2's user_dir is not in this config any more
        self.assertIn('Half-Life 2 game files', out)
        self.assertNotIn('Portal 2 game files', out)
        self.assertNotIn('Portal 2 Natif.app', out)

    def tmp_away(self, path):
        away = self.home / 'away'
        away.mkdir(exist_ok=True)
        return away / path.name

    def test_hl2_never_offers_portal2_data_as_its_user_dir(self):
        # An HL2 user_dir that is Portal 2's folder (the old shared default) or inside it.
        self.add_hl2()
        for user_dir in (self.support, self.support / 'userdata'):
            config = json.loads(self.config.read_text())
            config['games']['hl2']['user_dir'] = str(user_dir)
            self.config.write_text(json.dumps(config))
            self.run_script('n\nn\ny\ny\n')   # Portal 2: no, no; HL2: yes, yes
            self.assertTrue((self.support / 'userdata/portal2/SAVE').is_dir(), user_dir)

    def test_one_game_never_takes_the_other_games_custom_saves(self):
        # Both user_dirs custom, equal or nested: a yes for Portal 2's saves must not move HL2's.
        self.add_hl2()
        for hl2_dir in (self.explicit, self.explicit / 'hl2'):
            hl2_dir.mkdir(parents=True, exist_ok=True)
            config = json.loads(self.config.read_text())
            config['games']['hl2']['user_dir'] = str(hl2_dir)
            self.config.write_text(json.dumps(config))
            self.run_script('n\ny\nn\nn\n')   # Portal 2: app no, saves yes; HL2: no, no
            self.assertTrue(hl2_dir.is_dir(), hl2_dir)

    def test_folders_holding_a_game_or_depot_are_never_offered(self):
        # A game copied into the data folder, a depot left in the installer's build folder.
        build = self.home / 'Library/Application Support/Portal 2 Natif Build'
        depot = build / 'depot_661'
        depot.mkdir(parents=True)
        game = self.support / 'Portal 2'
        game.mkdir()
        self.config.write_text(json.dumps({'game_root': str(game), 'linux_root': str(depot)}))
        self.run_script('y\ny\ny\n')
        self.assertTrue(game.is_dir())
        self.assertTrue(depot.is_dir())
        self.assertFalse(self.app.exists())

    def test_nothing_installed(self):
        self.run_script('y\ny\n')
        out = self.run_script('y\ny\n')
        self.assertIn('nothing installed', out)


if __name__ == '__main__':
    unittest.main()
