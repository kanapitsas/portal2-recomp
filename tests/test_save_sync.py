"""tools/save_sync.py: which saves go between the Mac and the iPhone, with fake devicectl listings."""
import contextlib
import io
import json
import os
import sys
import tempfile
import unittest
from pathlib import Path
from unittest import mock

sys.path.insert(0, str(Path(__file__).resolve().parent.parent / 'tools'))
import save_sync as ss  # noqa: E402

MAC_ID = '76561198000000001'
IOS = ss.IOS_STEAM_ID
T0 = 1_790_000_000   # Unix time, late 2026


def iso(t):
    from datetime import datetime, timezone
    return datetime.fromtimestamp(t, timezone.utc).strftime('%Y-%m-%dT%H:%M:%SZ')


def listing(files, prefix='', folder='SAVE', steam_id=IOS):
    """devicectl's JSON report for {name: (mtime, size)} in <folder>/<steam_id>."""
    items = [{'relativePath': f'{prefix}{folder}/{steam_id}/{n}', 'name': n,
              'metadata': {'size': s, 'lastModDate': iso(t), 'type': 'file'}} for n, (t, s) in files.items()]
    items.append({'relativePath': f'{prefix}{folder}', 'metadata': {'type': 'directory', 'lastModDate': iso(T0)}})
    return {'info': {'outcome': 'success'}, 'result': {'files': items}}


def write(path, size, mtime):
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_bytes(b'x' * size)
    os.utime(path, (mtime, mtime))
    return path


class ParseTests(unittest.TestCase):
    def test_dates(self):
        self.assertEqual(ss.parse_date(iso(T0)), T0)
        self.assertEqual(ss.parse_date('2026-09-21T12:00:00.250+02:00'), ss.parse_date('2026-09-21T10:00:00Z'))
        self.assertEqual(ss.parse_date(T0 - ss.APPLE_EPOCH), T0)   # NSDate reference number
        self.assertEqual(ss.parse_date(T0), T0)
        self.assertIsNone(ss.parse_date('yesterday'))

    def test_listing_keeps_only_saves_and_screenshots(self):
        report = listing({'autosave.sav': (T0, 10), 'a.tga': (T0, 3), 'sp_a1.hl2': (T0, 5),
                          'steam_autocloud.vdf': (T0, 1)})
        report['result']['files'].append({'relativePath': 'cfg/config.cfg', 'metadata': {'size': 1}})
        self.assertEqual(ss.parse_listing(report), {('SAVE', IOS, 'autosave.sav'): (T0, 10),
                                                    ('SAVE', IOS, 'a.tga'): (T0, 3)})

    def test_paths_relative_to_the_container_and_other_case(self):
        report = listing({'quick.sav': (T0, 7)}, prefix='Documents/portal2-userdata/portal2/', folder='save')
        self.assertEqual(ss.parse_listing(report), {('save', IOS, 'quick.sav'): (T0, 7)})

    def test_errors_are_not_empty_listings(self):
        with self.assertRaises(ss.SyncError):
            ss.parse_listing({'error': {'code': 1}, 'result': {'files': []}})
        with self.assertRaises(ss.SyncError):
            ss.parse_listing({'result': {}})
        undated = {'result': {'files': [{'relativePath': f'SAVE/{IOS}/a.sav', 'metadata': {'size': 1}}]}}
        with self.assertRaises(ss.SyncError):
            ss.parse_listing(undated)

    def test_device_save_folder(self):
        both = {('SAVE', IOS, 'a.sav'): (T0, 1), ('SAVE', MAC_ID, 'b.sav'): (T0, 1)}
        self.assertEqual(ss.device_saves(both), ('SAVE', IOS, {'a.sav': (T0, 1)}))
        self.assertEqual(ss.device_saves({('save', MAC_ID, 'b.sav'): (T0, 1)}), ('save', MAC_ID, {'b.sav': (T0, 1)}))
        self.assertEqual(ss.device_saves({}), ('SAVE', IOS, {}))


class CompareTests(unittest.TestCase):
    def test_without_a_record_the_newer_mtime_wins(self):
        self.assertEqual(ss.compare((T0, 5), None), ss.MAC_ONLY)
        self.assertEqual(ss.compare(None, (T0, 5)), ss.IPHONE_ONLY)
        self.assertEqual(ss.compare((T0 + 60, 5), (T0, 6)), ss.MAC_NEWER)
        self.assertEqual(ss.compare((T0, 5), (T0 + 60, 5)), ss.IPHONE_NEWER)
        self.assertEqual(ss.compare((T0, 5), (T0 + 1, 5)), ss.SAME)
        self.assertEqual(ss.compare((T0, 5), (T0 + 1, 6)), ss.CONFLICT)

    def test_with_a_record_only_changes_count(self):
        last = {'mac': [T0, 5], 'iphone': [T0 + 500, 5]}   # devicectl gave the pushed copy a later date
        self.assertEqual(ss.compare((T0, 5), (T0 + 500, 5), last), ss.SAME)
        self.assertEqual(ss.compare((T0 + 100, 6), (T0 + 500, 5), last), ss.MAC_NEWER)
        self.assertEqual(ss.compare((T0, 5), (T0 + 900, 7), last), ss.IPHONE_NEWER)
        self.assertEqual(ss.compare((T0 + 100, 6), (T0 + 900, 7), last), ss.CONFLICT)

    def test_plan_never_overwrites_a_newer_file_without_force(self):
        mac = {'m.sav': (T0 + 60, 1), 'mo.sav': (T0, 1), 'same.sav': (T0, 2), 'c.sav': (T0, 3)}
        phone = {'m.sav': (T0, 1), 'io.sav': (T0, 1), 'same.sav': (T0, 2), 'c.sav': (T0 + 1, 4)}
        self.assertEqual(ss.plan('pull', mac, phone, {}),
                         (['io.sav'], [('c.sav', ss.CONFLICT), ('m.sav', ss.MAC_NEWER)], ['same.sav']))
        self.assertEqual(ss.plan('push', mac, phone, {}),
                         (['m.sav', 'mo.sav'], [('c.sav', ss.CONFLICT)], ['same.sav']))
        self.assertEqual(ss.plan('pull', mac, phone, {}, force=True)[0], ['c.sav', 'io.sav', 'm.sav'])


class MacSideTests(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        root = Path(self.tmp.name)
        self.user, self.game = root / 'userdata', root / 'game'

    def tearDown(self):
        self.tmp.cleanup()

    def test_save_id_skips_the_ios_stub_and_falls_back_to_the_game_folder(self):
        with self.assertRaises(ss.SyncError):
            ss.mac_save_id(self.user, self.game)
        write(self.game / f'portal2/SAVE/{MAC_ID}/a.sav', 1, T0)
        (self.user / f'portal2/SAVE/{IOS}').mkdir(parents=True)
        self.assertEqual(ss.mac_save_id(self.user, self.game), MAC_ID)
        self.assertEqual(ss.mac_save_id(self.user, self.game, forced='76561198000000002'), '76561198000000002')
        with self.assertRaises(ss.SyncError):
            ss.mac_save_id(self.user, self.game, forced='42')

    def test_game_view_user_copy_first_and_whiteouts(self):
        write(self.user / f'portal2/SAVE/{MAC_ID}/autosave.sav', 3, T0 + 10)
        write(self.user / f'portal2/SAVE/{MAC_ID}/sp.hl1', 3, T0)
        write(self.game / f'portal2/SAVE/{MAC_ID}/autosave.sav', 9, T0)
        write(self.game / f'portal2/SAVE/{MAC_ID}/old.sav', 4, T0 - 10)
        write(self.game / f'portal2/SAVE/{MAC_ID}/gone.sav', 4, T0 - 10)
        write(self.user / f'{ss.WHITEOUTS}/portal2/SAVE/{MAC_ID}/gone.sav/.wh', 0, T0)
        view = ss.mac_saves(self.user, self.game, MAC_ID)
        self.assertEqual({n: (v[0], v[1], v[3]) for n, v in view.items()},
                         {'autosave.sav': (T0 + 10, 3, False), 'old.sav': (T0 - 10, 4, True)})


class CommandTests(unittest.TestCase):
    """plan, the copies devicectl would make (done by hand here), then record."""
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        root = Path(self.tmp.name)
        self.user, self.game, self.sync = root / 'userdata', root / 'game', root / 'sync'
        self.phone = root / 'phone'        # stands for Documents/portal2-userdata/portal2 on the device
        self.listing = root / 'listing.json'
        patches = [mock.patch.object(ss.localconfig, 'load', return_value={'game_root': str(self.game)}),
                   mock.patch.object(ss.localconfig, 'user_dir', return_value=self.user),
                   mock.patch.dict(os.environ, {'P2_SAVE_SYNC_DIR': str(self.sync)})]
        for p in patches:
            p.start()
            self.addCleanup(p.stop)
        os.environ.pop('P2_SAVE_ID', None)

    def tearDown(self):
        self.tmp.cleanup()

    def relist(self):
        files = {p.name: (int(p.stat().st_mtime), p.stat().st_size) for p in (self.phone / f'SAVE/{IOS}').glob('*')}
        self.listing.write_text(json.dumps(listing(files)))

    def run_cmd(self, *args):
        out, err = io.StringIO(), io.StringIO()
        with contextlib.redirect_stdout(out), contextlib.redirect_stderr(err):
            code = ss.main([*args, '--listing', str(self.listing), '--device', 'UDID'])
        return code, out.getvalue(), err.getvalue()

    def device(self, path):
        return self.phone / path.removeprefix(ss.DEVICE_ROOT + '/')

    def apply(self, ops, now):
        for line in ops.splitlines():
            op, src, dst = line.split('\t')
            source, target = (self.device(src), Path(dst)) if op == 'from' else (Path(src), self.device(dst))
            target.parent.mkdir(parents=True, exist_ok=True)
            target.write_bytes(source.read_bytes())
            os.utime(target, (now, now))   # devicectl's copy: a fresh date

    def test_pull_backs_up_keeps_dates_and_records(self):
        mac = write(self.user / f'portal2/SAVE/{MAC_ID}/autosave.sav', 3, T0)
        write(self.user / f'portal2/SAVE/{MAC_ID}/mine.sav', 2, T0 + 900)
        write(self.phone / f'SAVE/{IOS}/autosave.sav', 5, T0 + 100)
        write(self.phone / f'SAVE/{IOS}/mine.sav', 6, T0 + 50)     # older than the Mac's: kept
        write(self.phone / f'SAVE/{IOS}/new.sav', 4, T0 + 200)
        self.relist()
        code, ops, err = self.run_cmd('plan', 'pull')
        self.assertEqual(code, 0, err)
        self.assertIn('kept mine.sav: Mac newer', err)
        self.assertEqual([line.split('\t')[0] for line in ops.splitlines()], ['from', 'from'])
        backups = list((self.sync / 'backups').glob('*-pull/autosave.sav'))
        self.assertEqual([b.stat().st_size for b in backups], [3])
        self.apply(ops, T0 + 5000)
        self.relist()
        self.assertEqual(self.run_cmd('record')[0], 0)
        self.assertEqual((int(mac.stat().st_mtime), mac.stat().st_size), (T0 + 100, 5))   # the iPhone's date
        self.assertEqual((self.user / f'portal2/SAVE/{MAC_ID}/new.sav').stat().st_size, 4)
        code, ops, err = self.run_cmd('plan', 'pull')
        self.assertEqual(ops, '')
        self.assertIn('kept mine.sav', err)

    def test_pull_never_writes_into_the_game_folder(self):
        steam = write(self.game / f'portal2/SAVE/{MAC_ID}/old.sav', 3, T0)
        write(self.phone / f'SAVE/{IOS}/old.sav', 5, T0 + 100)
        self.relist()
        code, ops, _ = self.run_cmd('plan', 'pull')
        self.assertEqual(ops.split('\t')[2].strip(), str(self.user / f'portal2/SAVE/{MAC_ID}/old.sav') + ss.PART)
        self.assertEqual(steam.stat().st_size, 3)

    def test_push_backs_up_the_iphone_copy_first_then_is_stable(self):
        write(self.user / f'portal2/SAVE/{MAC_ID}/autosave.sav', 8, T0 + 300)
        write(self.phone / f'SAVE/{IOS}/autosave.sav', 5, T0 + 100)
        self.relist()
        code, ops, err = self.run_cmd('plan', 'push')
        self.assertEqual(code, 0, err)
        lines = [line.split('\t') for line in ops.splitlines()]
        self.assertEqual([op for op, *_ in lines], ['from', 'to'])
        self.assertIn('-push', lines[0][2])
        self.apply(ops, T0 + 9000)   # the pushed copy looks newer than the Mac's afterwards
        self.assertEqual(Path(lines[0][2]).stat().st_size, 5)
        self.relist()
        self.assertEqual(self.run_cmd('record')[0], 0)
        code, out, _ = self.run_cmd('status')
        self.assertIn('same', out.splitlines()[3])
        self.assertIn('pull would copy 0, push would copy 0', out)

    def test_a_copy_that_did_not_arrive_is_not_recorded(self):
        write(self.user / f'portal2/SAVE/{MAC_ID}/a.sav', 8, T0 + 300)
        self.relist()
        self.assertEqual(self.run_cmd('plan', 'push')[0], 0)
        self.assertEqual(self.run_cmd('record')[0], 1)   # nothing copied: the iPhone has no a.sav
        self.assertEqual(self.run_cmd('plan', 'push', '--dry-run')[2].count('would copy a.sav'), 1)

    def test_a_pull_cut_short_leaves_the_mac_save_alone(self):
        mac = write(self.user / f'portal2/SAVE/{MAC_ID}/a.sav', 3, T0)
        write(self.phone / f'SAVE/{IOS}/a.sav', 5, T0 + 100)
        self.relist()
        code, ops, _ = self.run_cmd('plan', 'pull')
        Path(ops.split('\t')[2].strip()).write_bytes(b'xx')   # 2 of 5 bytes arrived
        self.assertEqual(self.run_cmd('record')[0], 1)
        self.assertEqual((mac.stat().st_size, int(mac.stat().st_mtime)), (3, T0))
        self.assertFalse(Path(str(mac) + ss.PART).exists())

    def test_dry_run_changes_nothing(self):
        write(self.user / f'portal2/SAVE/{MAC_ID}/a.sav', 3, T0)
        write(self.phone / f'SAVE/{IOS}/a.sav', 5, T0 + 100)
        self.relist()
        code, ops, err = self.run_cmd('plan', 'pull', '--dry-run')
        self.assertEqual((code, ops), (0, ''))
        self.assertIn('would copy a.sav', err)
        self.assertFalse(self.sync.exists())


if __name__ == '__main__':
    unittest.main()
