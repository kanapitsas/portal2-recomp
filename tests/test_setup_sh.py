"""setup.sh without running it: syntax, --help and option errors (all before any check)."""
import json
import os
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
                              (['--game=quake'], 'unknown game quake'),
                              (['--game', 'all', '/a'], 'paths are for one game'),
                              (['/a', '/b', '/c'], 'too many arguments')):
            result = run_setup(*args)
            self.assertEqual(result.returncode, 2, args)
            self.assertIn(message, result.stderr, args)


class GameDetectionTests(unittest.TestCase):
    """setup.sh --detect against a fake home (Steam folders) and a copy of the repository."""

    def setUp(self):
        directory = tempfile.TemporaryDirectory()
        self.addCleanup(directory.cleanup)
        self.tmp = Path(directory.name).resolve()
        self.repo = self.tmp / 'repo'
        (self.repo / 'tools').mkdir(parents=True)
        shutil.copy(ROOT / 'setup.sh', self.repo / 'setup.sh')
        for name in ('games.py', 'localconfig.py'):
            shutil.copy(ROOT / 'tools' / name, self.repo / 'tools' / name)
        self.home = self.tmp / 'home'
        self.steam = self.home / 'Library/Application Support/Steam'

    def touch(self, *paths):
        for path in paths:
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_bytes(b'')

    def install_portal2(self, depot=True):
        game = self.steam / 'steamapps/common/Portal 2'
        self.touch(game / 'portal2/pak01_dir.vpk', game / 'platform/x')
        item = game / 'portal2_dlc2/scripts/editoritems.txt'
        self.touch(item)
        item.write_text('"ItemData" {}')
        if depot:
            linux = self.steam / 'steamapps/content/app_620/depot_661'
            self.touch(linux / 'bin/linux32/engine.so', linux / 'portal2/bin/linux32/server.so')
        return game

    def install_hl2(self, depot=True):
        game = self.steam / 'steamapps/common/Half-Life 2'
        self.touch(game / 'hl2/hl2_pak_dir.vpk', game / 'hl2_complete/gameinfo.txt', game / 'platform/x')
        if depot:
            linux = self.steam / 'steamapps/content/app_220/depot_224'
            self.touch(linux / 'hl2_linux', linux / 'bin/engine.so', linux / 'hl2_complete/bin/server.so')
        return game

    def detect(self, *args, config=None):
        if config is not None:
            (self.repo / 'config').mkdir(exist_ok=True)
            (self.repo / 'config/local.json').write_text(json.dumps(config))
        env = {k: v for k, v in os.environ.items() if not k.startswith('P2_')}
        env['HOME'] = str(self.home)
        return subprocess.run(['bash', str(self.repo / 'setup.sh'), '--detect', *args],
                              capture_output=True, text=True, env=env)

    def assert_builds(self, result, games_line):
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertEqual(result.stdout.strip().splitlines()[-1], f'would build: {games_line}')

    def test_portal2_alone_stays_the_default(self):
        self.install_portal2()
        self.assert_builds(self.detect(), 'portal2')

    def test_incomplete_portal2_data_is_reported_before_building(self):
        game = self.install_portal2()
        item = game / 'portal2_dlc2/scripts/editoritems.txt'
        for contents in (None, ''):
            if contents is None:
                item.unlink()
            else:
                item.write_text(contents)
            result = self.detect('--game', 'portal2')
            self.assertEqual(result.returncode, 1)
            self.assertIn(str(item), result.stderr)
            self.assertIn('download_depot 620 621', result.stderr)

    def test_complete_depot_wins_over_incomplete_config_and_steam_folder(self):
        game = self.install_portal2()
        depot = self.steam / 'steamapps/content/app_620/depot_621'
        shutil.copytree(game, depot)
        (game / 'portal2_dlc2/scripts/editoritems.txt').unlink()
        result = self.detect(config={'game_root': str(game)})
        self.assert_builds(result, 'portal2')
        self.assertIn(f'game data:      {depot}', result.stdout)
        # An explicit user selection is diagnosed, not silently replaced.
        result = self.detect('--game', 'portal2', str(game))
        self.assertEqual(result.returncode, 1)
        self.assertIn('incomplete', result.stderr)

    def test_incomplete_portal2_does_not_block_hl2(self):
        game = self.install_portal2()
        (game / 'portal2_dlc2/scripts/editoritems.txt').unlink()
        self.install_hl2()
        result = self.detect()
        self.assert_builds(result, 'hl2')
        self.assertIn('Portal 2 skipped: incomplete game data', result.stderr)

    def test_every_installed_game_is_built(self):
        self.install_portal2()
        self.install_hl2()
        self.assert_builds(self.detect(), 'portal2 hl2')
        self.assert_builds(self.detect('--game', 'all'), 'portal2 hl2')
        self.assert_builds(self.detect('--game', 'hl2'), 'hl2')

    def test_game_without_linux_binaries_is_skipped_with_a_hint(self):
        # Many Portal 2 players have HL2 in Steam: that must not stop Portal 2.
        self.install_portal2()
        self.install_hl2(depot=False)
        result = self.detect()
        self.assert_builds(result, 'portal2')
        self.assertIn('download_depot 220 224', result.stderr)
        result = self.detect('--game', 'all')
        self.assertEqual(result.returncode, 1)
        self.assertIn("Half-Life 2's Linux binaries not found", result.stderr)

    def test_only_game_without_linux_binaries_says_what_to_fetch(self):
        self.install_hl2(depot=False)
        result = self.detect()
        self.assertEqual(result.returncode, 1)
        self.assertIn('download_depot 220 224', result.stderr)

    def test_leftover_portal2_folder_does_not_stop_hl2(self):
        # Steam on a Mac can keep steamapps/common/Portal 2 with only an empty
        # portal2/ folder (no macOS version any more): it is not an install.
        self.touch(self.steam / 'steamapps/common/Portal 2/portal2/.keep')
        linux = self.steam / 'steamapps/content/app_620/depot_661'   # depot fetched earlier
        self.touch(linux / 'bin/linux32/engine.so', linux / 'portal2/bin/linux32/server.so')
        self.install_hl2()
        self.assert_builds(self.detect(), 'hl2')

    def test_partial_depot_does_not_stop_the_other_game(self):
        game = self.install_portal2(depot=False)
        self.touch(self.steam / 'steamapps/content/app_620/depot_661/bin/linux32/engine.so')   # interrupted download
        self.install_hl2()
        result = self.detect()
        self.assert_builds(result, 'hl2')
        self.assertIn('download_depot 620 661', result.stderr)
        self.assertTrue(game.is_dir())

    def test_nothing_found(self):
        result = self.detect()
        self.assertEqual(result.returncode, 1)
        self.assertIn('no game found', result.stderr)

    def test_hl2_data_depot_is_found_without_a_steam_install(self):
        installed = self.install_hl2()
        depot = self.steam / 'steamapps/content/app_220/depot_221'
        shutil.move(str(installed), str(depot))
        # A leftover Steam folder and stale config must not hide the data depot.
        self.touch(installed / 'hl2_complete/.keep')
        config = {'games': {'hl2': {'game_root': str(self.tmp / 'missing')}}}
        for args in ((), ('--game', 'hl2')):
            result = self.detect(*args, config=config)
            self.assert_builds(result, 'hl2')
            self.assertIn(f'game data:      {depot}', result.stdout)

    def test_hl2_steam_install_still_takes_priority_over_data_depot(self):
        installed = self.install_hl2()
        depot = self.steam / 'steamapps/content/app_220/depot_221'
        shutil.copytree(installed, depot)
        result = self.detect('--game', 'hl2')
        self.assert_builds(result, 'hl2')
        self.assertIn(f'game data:      {installed}', result.stdout)

    def test_missing_hl2_data_explains_the_data_download(self):
        result = self.detect('--game', 'hl2')
        self.assertEqual(result.returncode, 1)
        self.assertIn('download_depot 220 221', result.stderr)
        self.assertIn('Depot download complete', result.stderr)

    def test_adding_a_game_to_a_configured_checkout(self):
        # No refusal any more: a Portal 2 checkout takes Half-Life 2 beside it.
        portal2 = self.install_portal2(depot=False)
        hl2 = self.install_hl2()
        config = {'game_root': str(portal2), 'linux_root': str(self.tmp / 'depot_661')}
        self.assert_builds(self.detect('--game', 'hl2', config=config), 'hl2')

    def test_configured_games_are_found_outside_steam(self):
        elsewhere = self.tmp / 'elsewhere'
        game, linux = elsewhere / 'Half-Life 2', elsewhere / 'depot_224'
        self.touch(game / 'hl2/hl2_pak_dir.vpk', game / 'hl2_complete/gameinfo.txt', game / 'platform/x',
                   linux / 'hl2_linux', linux / 'bin/engine.so', linux / 'hl2_complete/bin/server.so')
        section = {'game_root': str(game), 'linux_root': str(linux)}
        self.assert_builds(self.detect(config={'games': {'hl2': section}}), 'hl2')
        # An older single-game HL2 config: its keys at the top level.
        self.assert_builds(self.detect(config=dict(section, game='hl2')), 'hl2')
        # Paths without --game: a folder with hl2_complete/ is Half-Life 2.
        self.assert_builds(self.detect(str(game), str(linux), config={}), 'hl2')

    def test_ios_builds_each_selected_game(self):
        self.install_hl2()
        result = self.detect('--ios')
        self.assert_builds(result, 'hl2')
        self.assertIn('iPhone apps: hl2', result.stdout)
        self.install_portal2()
        for args, line in ((('--ios',), 'portal2 hl2'), (('--ios', '--game', 'hl2'), 'hl2'),
                           (('--ios', '--game', 'portal2'), 'portal2')):
            result = self.detect(*args)
            self.assert_builds(result, line)
            self.assertIn(f'iPhone apps: {line}', result.stdout)
        self.assertNotIn('iPhone', self.detect().stdout)


class SetupConfigTests(unittest.TestCase):
    """The config/local.json writer of setup.sh (its Python heredoc), run alone."""

    def write_config(self, config, team, *game_args):
        text = (ROOT / 'setup.sh').read_text()
        script = re.search(r'^python3 - "\$REPO/tools" "\$CONFIG".*?<<\'EOF\'\n(.*?)^EOF$', text, re.M | re.S).group(1)
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / 'local.json'
            if config is not None:
                path.write_text(json.dumps(config))
            result = subprocess.run([sys.executable, '-', str(ROOT / 'tools'), str(path), team, '', '', '0', '',
                                     *game_args], input=script, capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, result.stderr)
            return json.loads(path.read_text())

    def test_each_game_gets_its_own_ios_bundle_id(self):
        both = ('portal2', '/p2', '/p2l', '/p2.dylib', 'hl2', '/hl2', '/hl2l', '/hl2.dylib')
        config = self.write_config(None, 'ABCDE12345', *both)
        self.assertEqual(config['ios_bundle_id'], 'local.portal2recomp.abcde12345')
        self.assertEqual(config['games']['hl2']['ios_bundle_id'], 'local.hl2recomp.abcde12345')
        # An identifier already set is kept, and HL2 alone does not touch Portal 2's.
        config = self.write_config({'ios_bundle_id': 'my.portal2', 'games': {'hl2': {'ios_bundle_id': 'my.hl2'}}},
                                   'ABCDE12345', *both)
        self.assertEqual((config['ios_bundle_id'], config['games']['hl2']['ios_bundle_id']), ('my.portal2', 'my.hl2'))
        config = self.write_config({}, 'ABCDE12345', *both[4:])
        self.assertNotIn('ios_bundle_id', config)
        self.assertEqual(config['games']['hl2']['ios_bundle_id'], 'local.hl2recomp.abcde12345')
        # Without --ios (no team), no identifier is written.
        config = self.write_config(None, '', *both)
        self.assertNotIn('ios_bundle_id', config)
        self.assertNotIn('ios_bundle_id', config['games']['hl2'])


class SetupDeviceTests(unittest.TestCase):
    """The iPhone choice of setup.sh --ios, run alone with a fake xcrun."""

    def pick(self, p2_device=None):
        text = (ROOT / 'setup.sh').read_text()
        block = re.search(r'^    DEVICE=.*?\}\'\)$', text, re.M | re.S).group(0)
        with tempfile.TemporaryDirectory() as tmp:
            xcrun = Path(tmp) / 'xcrun'
            xcrun.write_text('#!/bin/sh\n'
                             'echo "Larus  a.coredevice.local  00008130-000C25812613803A  available (paired)  iPhone 15 Pro Max (physical)"\n'
                             'echo "Other  b.coredevice.local  00008110-00112233445566AA  available (paired)  iPhone 13 (physical)"\n')
            xcrun.chmod(0o755)
            env = {k: v for k, v in os.environ.items() if not k.startswith('P2_')}
            env['PATH'] = f"{tmp}:{env['PATH']}"
            if p2_device:
                env['P2_DEVICE'] = p2_device
            result = subprocess.run(['bash', '-c', block + '\necho "$DEVICE"'], capture_output=True, text=True, env=env)
            self.assertEqual(result.returncode, 0, result.stderr)
            return result.stdout.strip()

    def test_first_iphone_by_default(self):
        self.assertEqual(self.pick(), '00008130-000C25812613803A')

    def test_p2_device_picks_the_iphone(self):
        self.assertEqual(self.pick('00008110-00112233445566AA'), '00008110-00112233445566AA')


class SetupTableTests(unittest.TestCase):
    def test_game_table_matches_profiles(self):
        # setup.sh names the app, log and Steam ids itself; tools/games.py must agree.
        text = (ROOT / 'setup.sh').read_text()
        for name, profile in games.GAMES.items():
            block = re.search(rf'^ *{name}\)\n(.*?);;', text, re.M | re.S).group(1)
            title = re.search(r'TITLE="([^"]+)"', block).group(1)
            self.assertEqual(f'{title} Natif', profile['app_name'])
            self.assertEqual(title.replace(' ', '').replace('-', '') + 'Natif.log', profile['log_name'])
            self.assertEqual(int(re.search(r'APPID=(\d+)', block).group(1)), profile['steam_appid'])
            self.assertEqual(int(re.search(r'LINUX_DEPOT=(\d+)', block).group(1)), profile['depot_id'])


if __name__ == '__main__':
    unittest.main()
