"""tools/localconfig.py: config/local.json access and the saves folder default."""
import json
import sys
import tempfile
import unittest
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


if __name__ == '__main__':
    unittest.main()
