#!/usr/bin/env python3
"""Portal 2 saves between the Mac and the iPhone: the comparison and the plan.

tools/ios_device.sh does the device side (xcrun devicectl over the cable) and
calls this script; everything that decides what to copy lives here, so it can
be tested without a device (tests/test_save_sync.py).

    save_sync.py status --listing L.json --device UDID
    save_sync.py plan pull|push --listing L.json --device UDID [--force] [--dry-run]
    save_sync.py record --listing L.json --device UDID

Synced: <save dir>/<SteamID64>/*.sav and *.tga under the user data's portal2/
folder, nothing else (see docs/usage.md#saves-between-mac-and-iphone-portal-2). The Mac
saves sit under the Steam account's ID, the iPhone ones under the ID of the
offline Steam stub (runtime/ios/steam_stub.c): each file keeps its name and
moves to the other side's folder.

On the Mac the game sees the user data over the Steam game folder
(runtime/paths.c): a save only in <game_root>/portal2/SAVE/<id> is visible
unless the guest deleted it (whiteout marker). Those count as Mac saves for
`push`; `pull` only ever writes into the user data, never into the game folder
(Steam Cloud keeps that one).

Three-way comparison: after each copy the state file records both sides'
(mtime, size); a side whose values changed since is "changed". Without a
record, the newer mtime wins and equal size + mtime within 2 s counts as the
same file. A file changed on both sides is a conflict: never copied without
--force. Whatever a copy replaces is first backed up on the Mac.

`plan` backs up the Mac files it will replace, writes the pending copies next
to the state and prints the device operations, one per line:
    from<TAB><device path><TAB><Mac path>    (devicectl device copy from)
    to<TAB><Mac path><TAB><device path>      (devicectl device copy to)
A pulled file arrives as <save>.p2part. `record` (after the copies, with a
fresh listing) moves the complete ones into place with the iPhone's mtime (the
game orders its saves by date) and records the copies that landed.
"""
import argparse
import json
import os
import re
import shutil
import sys
import time
from datetime import datetime, timezone
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import localconfig  # noqa: E402

DEVICE_ROOT = 'Documents/portal2-userdata/portal2'   # what ios_device.sh lists (ios/main_ios.c: --user-dir)
IOS_STEAM_ID = '76561197960265729'                   # runtime/ios/steam_stub.c: ISteamUser::GetSteamID
WHITEOUTS = '.portal2-whiteouts'                     # runtime/posix.h: RT_WHITEOUTS
SYNCED = re.compile(r'.+\.(sav|tga)$', re.IGNORECASE)
SAVE_PATH = re.compile(r'(?:^|/)(save)/(\d{17})/([^/]+)$', re.IGNORECASE)
PART = '.p2part'         # pulled file before it replaces the save (save_sync.py record)
SAME_WINDOW = 2          # seconds: equal sizes and mtimes this close are the same file
APPLE_EPOCH = 978307200  # 2001-01-01 in Unix time (NSDate numbers)

# Verdicts of compare()
SAME, MAC_NEWER, IPHONE_NEWER, MAC_ONLY, IPHONE_ONLY, CONFLICT = (
    'same', 'mac-newer', 'iphone-newer', 'mac-only', 'iphone-only', 'conflict')
LABELS = {SAME: 'same', MAC_NEWER: 'Mac newer', IPHONE_NEWER: 'iPhone newer',
          MAC_ONLY: 'only on Mac', IPHONE_ONLY: 'only on iPhone', CONFLICT: 'changed on both sides'}


class SyncError(Exception):
    pass


# ---- device listing (devicectl device info files --json-output) ----

def parse_date(value):
    """Unix time (int) from devicectl's date: ISO 8601 text or an NSDate number."""
    if isinstance(value, bool):
        return None
    if isinstance(value, (int, float)):
        return int(value + APPLE_EPOCH if value < 1.2e9 else value)
    if isinstance(value, str):
        text = value.strip().replace('Z', '+00:00')
        try:
            moment = datetime.fromisoformat(text)
        except ValueError:
            try:
                moment = datetime.strptime(value.strip(), '%Y-%m-%d %H:%M:%S %z')
            except ValueError:
                return None
        if moment.tzinfo is None:
            moment = moment.replace(tzinfo=timezone.utc)
        return int(moment.timestamp())
    return None


def _field(item, exact, test):
    """Value of the first key found in the item or its metadata: exact names first, then test(key)."""
    places = [item] + [item[k] for k in ('metadata', 'attributes') if isinstance(item.get(k), dict)]
    for name in exact:
        for place in places:
            if name in place:
                return place[name]
    for place in places:
        for key, value in place.items():
            if test(key.lower()):
                return value
    return None


def parse_listing(report):
    """{(save dir name, id, file name): (mtime, size)} of the synced files in a devicectl JSON report.

    Paths are matched from their end (<save>/<id>/<name>), so they may be relative
    to the listed subdirectory or to the container. A listing without dates is an
    error: comparing would then be guesswork."""
    if not isinstance(report, dict) or report.get('error'):
        raise SyncError('devicectl reported an error instead of a file listing')
    files = (report.get('result') or {}).get('files')
    if not isinstance(files, list):
        raise SyncError('not a devicectl file listing (no result.files)')
    found = {}
    for item in files:
        if not isinstance(item, dict):
            continue
        path = item.get('relativePath') or item.get('path') or item.get('name') or ''
        match = SAVE_PATH.search(str(path).rstrip('/'))
        if not match or not SYNCED.match(match.group(3)):
            continue
        kind = str(_field(item, ('type', 'fileType'), lambda k: False) or '').lower()
        if 'dir' in kind:
            continue
        mtime = parse_date(_field(item, ('lastModDate', 'modificationDate', 'lastModificationDate'),
                                  lambda k: 'mod' in k and 'date' in k))
        size = _field(item, ('size', 'fileSize'), lambda k: 'size' in k)
        if mtime is None:
            raise SyncError(f'no modification date for {path} in the devicectl listing')
        found[match.groups()] = (mtime, int(size or 0))
    return found


def device_saves(listing):
    """(save dir name, id, {name: (mtime, size)}) of the iPhone: the stub's ID when it has saves,
    else the only other ID found, else the stub's ID in a new SAVE folder."""
    by_id = {}
    for (folder, steam_id, name), entry in listing.items():
        by_id.setdefault(steam_id, (folder, {}))[1][name] = entry
    if IOS_STEAM_ID in by_id:
        steam_id = IOS_STEAM_ID
    elif len(by_id) == 1:
        steam_id = next(iter(by_id))
    elif by_id:
        raise SyncError('several save folders on the iPhone: ' + ', '.join(sorted(by_id)))
    else:
        return 'SAVE', IOS_STEAM_ID, {}
    folder, files = by_id[steam_id]
    return folder, steam_id, files


# ---- Mac side ----

def _save_dir(portal2):
    """The portal2/<save> folder whatever its case (SAVE on the Mac), or None."""
    if portal2.is_dir():
        for child in sorted(portal2.iterdir()):
            if child.is_dir() and child.name.lower() == 'save':
                return child
    return None


def mac_save_id(user_dir, game_root=None, forced=None):
    """SteamID64 folder used on the Mac: P2_SAVE_ID, else the most recently changed
    numeric folder of the user data, else of the Steam game folder."""
    if forced:
        if not re.fullmatch(r'\d{17}', forced):
            raise SyncError(f'P2_SAVE_ID must be a SteamID64 (17 digits), not {forced!r}')
        return forced
    for root in (user_dir, game_root):
        if root is None:
            continue
        save = _save_dir(Path(root) / 'portal2')
        ids = [d for d in save.iterdir() if d.is_dir() and re.fullmatch(r'\d{17}', d.name)
               and d.name != IOS_STEAM_ID] if save else []
        if ids:
            return max(ids, key=lambda d: (d.stat().st_mtime, d.name)).name
    raise SyncError('no Mac save folder yet: save once in the game on the Mac, '
                    'or set P2_SAVE_ID to your SteamID64')


def _whiteout(user_dir, save_name, steam_id, name):
    for folder in {save_name, 'SAVE', 'save'}:
        if (Path(user_dir) / WHITEOUTS / 'portal2' / folder / steam_id / name / '.wh').exists():
            return True
    return False


def mac_saves(user_dir, game_root, steam_id):
    """{name: (mtime, size, path, from_game_folder)}: what the Mac game sees in its save folder."""
    seen = {}
    user_save = _save_dir(Path(user_dir) / 'portal2')
    layers = [(user_save, False)]
    if game_root:
        layers.append((_save_dir(Path(game_root) / 'portal2'), True))
    for save, lower in layers:
        folder = save / steam_id if save else None
        if not folder or not folder.is_dir():
            continue
        for path in sorted(folder.iterdir()):
            if path.name in seen or not path.is_file() or not SYNCED.match(path.name):
                continue
            if lower and _whiteout(user_dir, user_save.name if user_save else 'SAVE', steam_id, path.name):
                continue
            info = path.stat()
            seen[path.name] = (int(info.st_mtime), info.st_size, path, lower)
    return seen


# ---- comparison and plan (pure) ----

def compare(mac, iphone, last=None):
    """Verdict for one file. mac, iphone: (mtime, size) or None; last: the recorded
    {'mac': [mtime, size], 'iphone': [mtime, size]} of the previous sync, or None."""
    if iphone is None:
        return MAC_ONLY if mac is not None else None
    if mac is None:
        return IPHONE_ONLY
    mac, iphone = tuple(mac[:2]), tuple(iphone[:2])
    if last:
        mac_changed = mac != tuple(last['mac'])
        iphone_changed = iphone != tuple(last['iphone'])
        if not mac_changed and not iphone_changed:
            return SAME
        if mac_changed != iphone_changed:
            return MAC_NEWER if mac_changed else IPHONE_NEWER
    if abs(mac[0] - iphone[0]) <= SAME_WINDOW:
        return SAME if mac[1] == iphone[1] else CONFLICT
    if last:   # both changed since the last sync, to different content
        return CONFLICT
    return MAC_NEWER if mac[0] > iphone[0] else IPHONE_NEWER


def plan(mode, mac, iphone, state, force=False):
    """(copies, skipped, unchanged) for mode 'pull' (iPhone -> Mac) or 'push' (Mac -> iPhone).

    mac, iphone: {name: (mtime, size, ...)}; state: {name: record}. copies: names to
    copy; skipped: [(name, verdict)] that a copy would wrongly overwrite (without
    --force); unchanged: names already the same on both sides."""
    newer, only, other = ((IPHONE_NEWER, IPHONE_ONLY, MAC_NEWER) if mode == 'pull'
                          else (MAC_NEWER, MAC_ONLY, IPHONE_NEWER))
    copies, skipped, unchanged = [], [], []
    for name in sorted(set(mac) | set(iphone)):
        verdict = compare(mac.get(name), iphone.get(name), state.get(name))
        if verdict == SAME:
            unchanged.append(name)
        elif verdict in (newer, only):
            copies.append(name)
        elif verdict in (other, CONFLICT):
            (copies.append(name) if force else skipped.append((name, verdict)))
    return copies, skipped, unchanged


# ---- state, backups ----

def sync_dir():
    return Path(os.environ.get('P2_SAVE_SYNC_DIR') or localconfig.SUPPORT / 'save-sync')


def _state_path(device):
    return sync_dir() / f'{re.sub(r"[^A-Za-z0-9._-]", "_", device)}.json'


def load_state(device):
    path = _state_path(device)
    return json.loads(path.read_text()).get('files', {}) if path.exists() else {}


def save_state(device, files):
    path = _state_path(device)
    path.parent.mkdir(parents=True, exist_ok=True)
    tmp = path.with_suffix('.tmp')
    tmp.write_text(json.dumps({'files': files}, indent=1, sort_keys=True) + '\n')
    tmp.replace(path)


def _when(mtime):
    return time.strftime('%Y-%m-%d %H:%M', time.localtime(mtime)) if mtime is not None else '-'


# ---- commands ----

class Context:
    def __init__(self, args):
        config = localconfig.load()
        self.user_dir = localconfig.user_dir(config)
        self.game_root = config.get('game_root')
        self.device = args.device
        report = json.loads(Path(args.listing).read_text())
        self.listing = parse_listing(report)
        self.ios_folder, self.ios_id, self.iphone = device_saves(self.listing)
        self.mac_id = mac_save_id(self.user_dir, self.game_root, os.environ.get('P2_SAVE_ID'))
        self.mac = mac_saves(self.user_dir, self.game_root, self.mac_id)
        self.state = load_state(self.device)

    def device_path(self, name):
        return f'{DEVICE_ROOT}/{self.ios_folder}/{self.ios_id}/{name}'

    def mac_target(self, name):
        save = _save_dir(Path(self.user_dir) / 'portal2')
        return (save or Path(self.user_dir) / 'portal2/SAVE') / self.mac_id / name


def cmd_status(ctx, args):
    print(f'Mac:    {ctx.user_dir}/portal2/SAVE/{ctx.mac_id}')
    print(f'iPhone: {ctx.device_path("")}')
    names = sorted(set(ctx.mac) | set(ctx.iphone))
    if not names:
        print('no saves on either side')
        return 0
    width = max(len(n) for n in names)
    print(f'{"file":<{width}}  {"Mac":<16}  {"iPhone":<16}  state')
    for name in names:
        m, i = ctx.mac.get(name), ctx.iphone.get(name)
        verdict = compare(m, i, ctx.state.get(name))
        steam = ' (Steam game folder)' if m and m[3] else ''
        print(f'{name:<{width}}  {_when(m and m[0]):<16}  {_when(i and i[0]):<16}  {LABELS[verdict]}{steam}')
    pulls = plan('pull', ctx.mac, ctx.iphone, ctx.state)
    pushes = plan('push', ctx.mac, ctx.iphone, ctx.state)
    conflicts = sum(1 for _, v in pulls[1] if v == CONFLICT)
    print(f'pull would copy {len(pulls[0])}, push would copy {len(pushes[0])}, '
          f'{conflicts} changed on both sides (only --force copies those)')
    return 0


def cmd_plan(ctx, args):
    copies, skipped, unchanged = plan(args.mode, ctx.mac, ctx.iphone, ctx.state, args.force)
    for name, verdict in skipped:
        hint = 'push it instead' if verdict == MAC_NEWER and args.mode == 'pull' else \
               'pull it instead' if verdict == IPHONE_NEWER else 'pick a side with --force'
        print(f'kept {name}: {LABELS[verdict]} ({hint})', file=sys.stderr)
    replaced = [n for n in copies if n in (ctx.mac if args.mode == 'pull' else ctx.iphone)]
    stamp = time.strftime('%Y%m%d-%H%M%S')
    backup = sync_dir() / 'backups' / f'{stamp}-{args.mode}'
    for name in copies:
        print(f'{"would copy" if args.dry_run else "copy"} {name} '
              f'{"iPhone -> Mac" if args.mode == "pull" else "Mac -> iPhone"}'
              f'{" (old copy backed up)" if name in replaced else ""}', file=sys.stderr)
    if not copies:
        print(f'nothing to {args.mode} ({len(unchanged)} already the same)', file=sys.stderr)
    if args.dry_run:
        return 0
    pending = {'mode': args.mode, 'copies': {}, 'unchanged': unchanged}
    lines = []
    if replaced:
        backup.mkdir(parents=True, exist_ok=True)
        print(f'backups: {backup}', file=sys.stderr)
    for name in copies:
        if args.mode == 'pull':
            target = ctx.mac_target(name)
            if name in ctx.mac:   # also the Steam game folder's copy, which the new file hides
                shutil.copy2(ctx.mac[name][2], backup / name)
            target.parent.mkdir(parents=True, exist_ok=True)
            # Into a side file first: a copy cut short never leaves a half-written save.
            lines.append(f'from\t{ctx.device_path(name)}\t{target}{PART}')
            pending['copies'][name] = {'mac': str(target)}
        else:
            if name in ctx.iphone:
                lines.append(f'from\t{ctx.device_path(name)}\t{backup / name}')
            lines.append(f'to\t{ctx.mac[name][2]}\t{ctx.device_path(name)}')
            old = ctx.iphone.get(name)
            pending['copies'][name] = {'mac': str(ctx.mac[name][2]), 'before': list(old[:2]) if old else None}
    path = _state_path(ctx.device).with_suffix('.pending.json')
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(pending, indent=1) + '\n')
    for line in lines:
        print(line)
    return 0


def cmd_record(ctx, args):
    """Records the copies that landed (sizes match on both sides) and the files already the same."""
    path = _state_path(ctx.device).with_suffix('.pending.json')
    if not path.exists():
        return 0
    pending = json.loads(path.read_text())
    state = dict(ctx.state)
    done = failed = 0
    for name, copy in pending['copies'].items():
        target, phone = Path(copy['mac']), ctx.iphone.get(name)
        # Arrived: same size on both sides, and the destination is no longer the file it was.
        if pending['mode'] == 'pull':
            part = Path(f'{target}{PART}')
            landed = phone is not None and part.is_file() and part.stat().st_size == phone[1]
            if landed:
                part.replace(target)
            elif part.exists():
                part.unlink()
        else:
            before = copy['before']
            landed = (target.is_file() and phone is not None and target.stat().st_size == phone[1]
                      and (before is None or list(phone[:2]) != before))
        if not landed:
            failed += 1
            continue
        if pending['mode'] == 'pull':   # the iPhone's date, so the game lists the saves in order
            os.utime(target, (phone[0], phone[0]))
        info = target.stat()
        state[name] = {'mac': [int(info.st_mtime), info.st_size], 'iphone': list(phone[:2])}
        done += 1
    for name in pending.get('unchanged', []):
        if name in ctx.mac and name in ctx.iphone:
            state[name] = {'mac': list(ctx.mac[name][:2]), 'iphone': list(ctx.iphone[name][:2])}
    save_state(ctx.device, state)
    path.unlink()
    print(f'{done} copied' + (f', {failed} did not arrive (run it again)' if failed else ''), file=sys.stderr)
    return 1 if failed else 0


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    sub = parser.add_subparsers(dest='command', required=True)
    for name in ('status', 'plan', 'record'):
        p = sub.add_parser(name)
        if name == 'plan':
            p.add_argument('mode', choices=('pull', 'push'))
            p.add_argument('--force', action='store_true')
            p.add_argument('--dry-run', action='store_true')
        p.add_argument('--listing', required=True)
        p.add_argument('--device', required=True)
    args = parser.parse_args(argv)
    try:
        ctx = Context(args)
        return {'status': cmd_status, 'plan': cmd_plan, 'record': cmd_record}[args.command](ctx, args)
    except (SyncError, OSError, ValueError) as error:
        print(f'save sync: {error}', file=sys.stderr)
        return 1


if __name__ == '__main__':
    sys.exit(main())
