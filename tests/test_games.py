import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent / 'tools'))
import build_game  # noqa: E402
import games  # noqa: E402


class GameProfiles(unittest.TestCase):
    def test_default_is_portal2(self):
        game = games.profile({})
        self.assertEqual(game['name'], 'portal2')
        self.assertEqual(game['program'], 'portal2_linux')
        self.assertEqual(game['roots'], 'config/game-roots.json')

    def test_hl2_profile(self):
        game = games.profile({'game': 'hl2'})
        self.assertEqual(game['program'], 'hl2_linux')
        self.assertEqual(game['lib_dirs'], ('bin', 'hl2_complete/bin'))
        # The host FreeType/Fontconfig replace the bundled ones (runtime/fonts.c).
        self.assertIn('libfreetype.so', game['excluded'])
        self.assertIn('libdxvk_d3d9.so', game['excluded'])

    def test_unknown_game_is_rejected(self):
        with self.assertRaises(SystemExit):
            games.profile({'game': 'quake'})

    def test_build_paths(self):
        # Portal 2 keeps the paths of single-game checkouts; Half-Life 2 lives under build/hl2.
        portal2, hl2 = games.GAMES['portal2'], games.GAMES['hl2']
        self.assertEqual((portal2['build_dir'], portal2['binary'], portal2['ios_dir'], portal2['executable']),
                         ('build/game', 'build/portal2-native', 'build/ios', 'portal2-native'))
        self.assertEqual((hl2['build_dir'], hl2['binary'], hl2['ios_dir'], hl2['executable']),
                         ('build/hl2', 'build/hl2/hl2-native', 'build/hl2/ios', 'hl2-native'))
        self.assertEqual(games.EXECUTABLES, ('portal2-native', 'hl2-native'))
        # No two games share a build tree, binary or data folder.
        for key in ('build_dir', 'binary', 'ios_dir', 'executable', 'app_name', 'log_name', 'bundle_id'):
            self.assertEqual(len({g[key] for g in games.GAMES.values()}), len(games.GAMES), key)

    def test_selection_precedence(self):
        both = {'game_root': '/p2', 'games': {'hl2': {'game_root': '/hl2'}}}
        self.assertEqual(games.selected(None, {}, environ={}), 'portal2')
        self.assertEqual(games.selected(None, both, environ={}), 'portal2')
        self.assertEqual(games.selected(None, dict(both, game='hl2'), environ={}), 'hl2')
        self.assertEqual(games.selected(None, dict(both, game='hl2'), environ={'P2_GAME': 'portal2'}), 'portal2')
        self.assertEqual(games.selected('hl2', {'game': 'portal2'}, environ={'P2_GAME': 'portal2'}), 'hl2')
        self.assertEqual(games.selected(None, {}, environ={'P2_GAME': ''}), 'portal2')
        # Only Half-Life 2 set up: no --game needed.
        self.assertEqual(games.selected(None, {'games': {'hl2': {'game_root': '/hl2'}}}, environ={}), 'hl2')
        self.assertEqual(games.selected(None, {'games': {'hl2': {}}}, environ={}), 'portal2')
        for name, environ in (('quake', {}), (None, {'P2_GAME': 'quake'})):
            with self.assertRaises(SystemExit):
                games.selected(name, {}, environ=environ)
        with self.assertRaises(SystemExit):
            games.selected(None, {'game': 'quake'}, environ={})

    def test_roots_follow_the_program(self):
        layout = {'program': 'hl2_linux', 'modules': [
            {'name': 'hl2_linux', 'init': 0x100, 'init_array': [1, 2]},
            {'name': 'engine.so', 'init': 0x200, 'init_array': [3]},
        ]}
        roots = build_game.roots(layout, roots_file='/nonexistent/roots.json')
        self.assertEqual(roots, ['hl2_linux:symbol:main', 'hl2_linux:init:dt', 'hl2_linux:init:0', 'hl2_linux:init:1'])

    def test_roots_need_the_program_key(self):
        # tools/elfprep.py always writes it (--program); no silent Portal 2 default.
        with self.assertRaises(KeyError):
            build_game.roots({'modules': []}, roots_file='/nonexistent/roots.json')

    def test_roots_file_is_relative_to_the_checkout(self):
        import json
        import tempfile
        layout = {'program': 'hl2_linux', 'modules': []}
        (build_game.ROOT / 'build').mkdir(exist_ok=True)   # absent on a fresh clone (CI python-3-9 job)
        with tempfile.TemporaryDirectory(dir=build_game.ROOT / 'build') as tmp:
            path = Path(tmp) / 'roots.json'
            path.write_text(json.dumps(['engine.so:code:0x10']))
            relative = path.relative_to(build_game.ROOT)
            self.assertEqual(build_game.roots(layout, roots_file=str(relative)),
                             ['hl2_linux:symbol:main', 'engine.so:code:0x10'])
            self.assertEqual(build_game.roots(layout, roots_file=str(path))[-1], 'engine.so:code:0x10')


class IosProfiles(unittest.TestCase):
    """tools/build_ios.py and runtime/ios/game_ios.c agree on each game."""
    ROOT = Path(__file__).resolve().parent.parent

    def c_table(self):
        import re
        text = (self.ROOT / 'runtime/ios/game_ios.c').read_text()
        rows = re.findall(r'\{"(\w+)", "(\w+)", "([\w-]+)", "(\d+)", (\d), (\w+), "(\w+)", (\d+)\}', text)
        return {r[0]: r for r in rows}

    def test_every_game_has_an_ios_profile(self):
        table = self.c_table()
        self.assertEqual(set(table), set(games.GAMES))
        for name, game in games.GAMES.items():
            row = table[name]
            self.assertEqual(row[1], game['mod'])
            self.assertEqual(int(row[3]), game['steam_appid'])
            self.assertTrue(game['ios_name'] and game['ios_executable'].isidentifier())
            self.assertIn(game['mod'], game['ios_data'])
        # Portal 2 keeps the folder of existing installs.
        self.assertEqual(table['portal2'][2], 'portal2-userdata')
        self.assertEqual(table['hl2'][5], 'RT_IOS_LAYOUT_HL2')

    def test_hl2_settings_page_has_no_portals(self):
        import plistlib
        import shutil
        import tempfile
        import build_ios
        with tempfile.TemporaryDirectory() as tmp:
            for game in ('portal2', 'hl2'):
                path = Path(tmp) / f'{game}.plist'
                shutil.copy(self.ROOT / 'ios/Settings.bundle/Root.plist', path)
                build_ios.settings_for_game(path, game)
                items = plistlib.loads(path.read_bytes())['PreferenceSpecifiers']
                keys = [i.get('Key') for i in items]
                # Gyro aiming: off by default in Portal 2 (back taps shake the
                # phone), on in Half-Life 2, as rt_gyro_enabled's default.
                gyro = next(i for i in items if i.get('Key') == 'P2Gyro')
                self.assertEqual(gyro['DefaultValue'], game == 'hl2')
                self.assertIn('P2GyroSpeed', keys)
                if game == 'portal2':
                    self.assertIn('P2BackTap', keys)
                    self.assertIn('P2PortalButtons', keys)
                else:
                    for key in ('P2BackTap', 'P2Haptics', 'P2BackTapRecord', 'P2PortalButtons'):
                        self.assertNotIn(key, keys)
                    for key in ('P2Renderer', 'P2TouchControls', 'P2TouchTrackpad', 'P2Log'):
                        self.assertIn(key, keys)

    def test_info_plist_names_the_game(self):
        plist = (self.ROOT / 'ios/Info.plist.in').read_text()
        self.assertIn('<key>P2Game</key><string>@GAME@</string>', plist)
        self.assertNotIn('Portal 2', plist)   # the name comes from the profile (@NAME@)


if __name__ == '__main__':
    unittest.main()
