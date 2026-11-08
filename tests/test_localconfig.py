"""tools/localconfig.py: config/local.json access and the saves folder default."""
import json
import os
import sys
import tempfile
import unittest
from unittest import mock
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent / 'tools'))
import localconfig  # noqa: E402


class LocalConfigTests(unittest.TestCase):
    def test_missing_file_is_empty_config(self):
        with tempfile.TemporaryDirectory() as tmp:
            self.assertEqual(localconfig.load(Path(tmp) / 'local.json'), {})

    def test_load_reads_the_given_file(self):
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / 'local.json'
            path.write_text(json.dumps({'game_root': '/g'}))
            self.assertEqual(localconfig.load(path), {'game_root': '/g'})
            self.assertEqual(localconfig.load(str(path)), {'game_root': '/g'})

    def test_default_path_is_the_repository_config(self):
        self.assertEqual(localconfig.DEFAULT_PATH, Path(__file__).resolve().parent.parent / 'config/local.json')

    def test_user_dir_default_and_override(self):
        self.assertEqual(localconfig.user_dir({}),
                         Path.home() / 'Library/Application Support/Portal 2 Natif/userdata')
        self.assertEqual(localconfig.user_dir({}, support=Path('/s')), Path('/s/userdata'))
        with tempfile.TemporaryDirectory() as tmp:
            mine = Path(tmp).resolve() / 'mine'
            self.assertEqual(localconfig.user_dir({'user_dir': str(mine)}), mine)
            self.assertEqual(localconfig.user_dir({'user_dir': ''}), localconfig.SUPPORT / 'userdata')

    def test_host_prefix_env_then_config_then_homebrew(self):
        with mock.patch.dict('os.environ', {}, clear=False):
            os.environ.pop('P2_HOST_PREFIX', None)
            self.assertEqual(localconfig.host_prefix({}), Path('/opt/homebrew'))
            self.assertEqual(localconfig.host_prefix({'host_prefix': '/b'}), Path('/b'))
            os.environ['P2_HOST_PREFIX'] = '/e'
            self.assertEqual(localconfig.host_prefix({'host_prefix': '/b'}), Path('/e'))

    def test_host_flags_name_the_prefix_and_its_rpath(self):
        cflags, libs = localconfig.host_flags(Path('/p q'))
        self.assertIn('-I/p q/include/freetype2', cflags)
        self.assertEqual(libs[:2], ['-L/p q/lib', '-Wl,-rpath,/p q/lib'])
    def test_each_game_has_its_own_data_folder(self):
        # Portal 2 keeps its folder; HL2 must not share it (overlapping whiteouts, cfg, platform/).
        self.assertEqual(localconfig.support_dir({}), localconfig.SUPPORT)
        self.assertEqual(localconfig.support_dir({'game': 'portal2'}), localconfig.SUPPORT)
        hl2 = localconfig.support_dir({'game': 'hl2'})
        self.assertEqual(hl2, Path.home() / 'Library/Application Support/Half-Life 2 Natif')
        self.assertEqual(localconfig.user_dir({'game': 'hl2'}), hl2 / 'userdata')
        self.assertEqual(localconfig.user_dir({'game': 'hl2', 'user_dir': '/x'}), Path('/x').resolve())


    TWO_GAMES = {'game_root': '/p2', 'linux_root': '/d661', 'steam_appid': 620, 'user_dir': '/p2saves',
                 'ios_bundle_id': 'x.portal2', 'host_prefix': '/deps', 'd3d9metal_dylib': '/m.dylib', 'language': 'french',
                 'games': {'hl2': {'game_root': '/hl2', 'linux_root': '/d224', 'steam_appid': 220,
                                   'steam_api_dylib': '/sw2024/libsteam_api.dylib'}}}

    def test_merged_view_per_game(self):
        with mock.patch.dict('os.environ', {}, clear=False):
            os.environ.pop('P2_GAME', None)
            portal2 = localconfig.game_config(self.TWO_GAMES)
            hl2 = localconfig.game_config(self.TWO_GAMES, 'hl2')
        self.assertEqual(portal2, {k: v for k, v in self.TWO_GAMES.items() if k != 'games'} | {'game': 'portal2'})
        # Shared keys reach HL2; Portal 2's own keys (user_dir, ios_bundle_id...) do not.
        self.assertEqual(hl2, {'host_prefix': '/deps', 'd3d9metal_dylib': '/m.dylib', 'language': 'french',
                               'game': 'hl2', 'game_root': '/hl2', 'linux_root': '/d224', 'steam_appid': 220,
                               'steam_api_dylib': '/sw2024/libsteam_api.dylib'})
        self.assertEqual(localconfig.user_dir(hl2), Path.home() / 'Library/Application Support/Half-Life 2 Natif/userdata')
        self.assertEqual(localconfig.configured_games(self.TWO_GAMES), ['portal2', 'hl2'])
        self.assertEqual(localconfig.configured_games({'games': {'hl2': {'game_root': '/hl2'}}}), ['hl2'])
        self.assertEqual(localconfig.configured_games({}), [])

    def test_portal2_section_overrides_top_level(self):
        config = {'game_root': '/old', 'games': {'portal2': {'game_root': '/new'}}}
        self.assertEqual(localconfig.game_config(config, 'portal2')['game_root'], '/new')

    def test_older_hl2_checkout_is_read_as_its_section(self):
        # A single-game HL2 config (before both games shared a checkout) keeps working.
        legacy = {'game': 'hl2', 'game_root': '/hl2', 'linux_root': '/d224', 'steam_appid': 220,
                  'd3d9metal_dylib': '/m.dylib'}
        migrated = localconfig.migrate(legacy)
        self.assertEqual(migrated, {'game': 'hl2', 'd3d9metal_dylib': '/m.dylib', 'games': {
            'hl2': {'game_root': '/hl2', 'linux_root': '/d224', 'steam_appid': 220}}})
        self.assertEqual(legacy['game_root'], '/hl2')   # a copy: the caller's dict is untouched
        self.assertEqual(localconfig.game_config(legacy, 'hl2')['game_root'], '/hl2')
        self.assertNotIn('game_root', localconfig.game_config(legacy, 'portal2'))
        with mock.patch.dict('os.environ', {}, clear=False):
            os.environ.pop('P2_GAME', None)
            self.assertEqual(localconfig.game_config(legacy)['game'], 'hl2')   # still the default game
        # Already migrated, or Portal 2's own config: unchanged.
        self.assertEqual(localconfig.migrate(migrated), migrated)
        self.assertEqual(localconfig.migrate({'game': 'portal2', 'game_root': '/p2'}), {'game': 'portal2', 'game_root': '/p2'})


if __name__ == '__main__':
    unittest.main()
