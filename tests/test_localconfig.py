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


if __name__ == '__main__':
    unittest.main()
