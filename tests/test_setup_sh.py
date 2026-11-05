"""setup.sh without running it: syntax, --help and option errors (all before any check)."""
import subprocess
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent


class SetupScriptTests(unittest.TestCase):
    def test_scripts_parse(self):
        for script in ('setup.sh', 'uninstall.sh', 'tools/ios_device.sh'):
            self.assertEqual(subprocess.run(['bash', '-n', str(ROOT / script)]).returncode, 0, script)

    def test_help_prints_usage_only(self):
        for flag in ('-h', '--help'):
            result = subprocess.run(['bash', str(ROOT / 'setup.sh'), flag], capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertIn('usage: ./setup.sh', result.stdout)
            self.assertNotIn('set -e', result.stdout)
            self.assertNotIn('#', result.stdout.splitlines()[0])

    def test_unknown_option_exits_2(self):
        result = subprocess.run(['bash', str(ROOT / 'setup.sh'), '--bogus'], capture_output=True, text=True)
        self.assertEqual(result.returncode, 2)
        self.assertIn('unknown option --bogus', result.stderr)


if __name__ == '__main__':
    unittest.main()
