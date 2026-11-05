"""Generated runtime files are up to date with their generators (same style as
test_gen_d3d9): gl_generated.c (tools/gen_gl.py, needs the macOS SDK headers)
and ios/steam_stub_generated.c (tools/gen_steam_stub.py). Both are deterministic."""
import subprocess
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / 'tools'))
import gen_steam_stub  # noqa: E402


def sdk_available():
    try:
        return subprocess.run(['xcrun', '--show-sdk-path'], capture_output=True, text=True).returncode == 0
    except OSError:
        return False


class GeneratedFiles(unittest.TestCase):
    @unittest.skipUnless(sdk_available(), 'xcrun --show-sdk-path failed: no macOS SDK headers')
    def test_gl_generated_is_up_to_date(self):
        import gen_gl
        text, count, _ = gen_gl.generate()
        self.assertGreater(count, 100)
        self.assertEqual((ROOT / 'runtime/gl_generated.c').read_text(), text, 'run python3 tools/gen_gl.py')

    def test_steam_stub_generated_is_up_to_date(self):
        text, count, _ = gen_steam_stub.generate()
        self.assertGreater(count, 100)
        self.assertEqual((ROOT / 'runtime/ios/steam_stub_generated.c').read_text(), text,
                         'run python3 tools/gen_steam_stub.py')


if __name__ == '__main__':
    unittest.main()
