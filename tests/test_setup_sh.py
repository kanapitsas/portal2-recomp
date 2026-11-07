"""setup.sh without running it: syntax, --help and option errors (all before any check)."""
import json
import re
import shutil
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / 'tools'))
import games  # noqa: E402


def run_setup(*args, setup=ROOT / 'setup.sh'):
    return subprocess.run(['bash', str(setup), *args], capture_output=True, text=True)


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

    def test_game_option_errors(self):
        for args, message in ((['--game'], '--game needs a value'),
                              (['--game', 'quake'], 'unknown game quake'),
                              (['--game=quake'], 'unknown game quake')):
            result = run_setup(*args)
            self.assertEqual(result.returncode, 2, args)
            self.assertIn(message, result.stderr, args)

    def test_configured_checkout_never_switches_game(self):
        # build/ and the app belong to the configured game: switching needs another clone.
        with tempfile.TemporaryDirectory() as tmp:
            repo = Path(tmp)
            shutil.copy(ROOT / 'setup.sh', repo / 'setup.sh')
            (repo / 'config').mkdir()
            for config, args, configured in (({}, ['--game', 'hl2'], 'portal2'),   # no "game": Portal 2
                                             ({'game': 'portal2'}, ['--game=hl2'], 'portal2'),
                                             ({'game': 'hl2'}, ['--game', 'portal2'], 'hl2')):
                (repo / 'config/local.json').write_text(json.dumps(config))
                result = run_setup(*args, setup=repo / 'setup.sh')
                self.assertEqual(result.returncode, 2, config)
                self.assertIn(f'set up for {configured}', result.stderr)
                self.assertIn('clone the repository again', result.stderr)

    def test_game_table_matches_profiles(self):
        # setup.sh names the app, log and Steam ids itself; tools/games.py must agree.
        text = (ROOT / 'setup.sh').read_text()
        for name, profile in games.GAMES.items():
            block = re.search(rf'^    {name}\)\n(.*?);;', text, re.M | re.S).group(1)
            title = re.search(r'TITLE="([^"]+)"', block).group(1)
            self.assertEqual(f'{title} Natif', profile['app_name'])
            self.assertEqual(title.replace(' ', '').replace('-', '') + 'Natif.log', profile['log_name'])
            self.assertEqual(int(re.search(r'APPID=(\d+)', block).group(1)), profile['steam_appid'])
            self.assertEqual(int(re.search(r'LINUX_DEPOT=(\d+)', block).group(1)), profile['depot_id'])


if __name__ == '__main__':
    unittest.main()
