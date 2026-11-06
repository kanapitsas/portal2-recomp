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

    def test_roots_follow_the_program(self):
        layout = {'program': 'hl2_linux', 'modules': [
            {'name': 'hl2_linux', 'init': 0x100, 'init_array': [1, 2]},
            {'name': 'engine.so', 'init': 0x200, 'init_array': [3]},
        ]}
        roots = build_game.roots(layout, roots_file='/nonexistent/roots.json')
        self.assertEqual(roots, ['hl2_linux:symbol:main', 'hl2_linux:init:dt', 'hl2_linux:init:0', 'hl2_linux:init:1'])

    def test_roots_without_program_key_stay_portal2(self):
        roots = build_game.roots({'modules': []}, roots_file='/nonexistent/roots.json')
        self.assertEqual(roots, ['portal2_linux:symbol:main'])


if __name__ == '__main__':
    unittest.main()
