#!/usr/bin/env python3
"""Run the native executable against the user's real game data and log output."""
import argparse
import shutil
import subprocess
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import games  # noqa: E402
import language  # noqa: E402
import launch  # noqa: E402
import localconfig  # noqa: E402

TIMEOUT_STATUS = 124   # as timeout(1): the game was still running when the diagnostic timeout fell


def run(config, game, game_args=(), binary=None, timeout=60, show=False, env=None):
    """Launch one game (its merged config and profile) and return
    (exit status, log path); the log is <build_dir>/launch.log, rewritten at
    every launch. `game_args` after an optional `--`; empty: a windowed
    launch of the game's mod. Without `show`, a diagnostic launch: hidden
    window, muted sound, on a copy of the user data (never two instances on
    the same saves and settings, never the user's settings changed)."""
    build_dir = games.ROOT / game['build_dir']
    binary = Path(binary) if binary else games.ROOT / game['binary']
    game_args = list(game_args)
    if game_args[:1] == ['--']: game_args = game_args[1:]
    if not game_args: game_args = ['-game', game['mod'], '-novid', '-windowed', '-w', '1280', '-h', '720']
    # Langue du jeu (P2_LANGUAGE, config/local.json, Steam, système) : voir tools/language.py.
    game_args = [*game_args, *language.language_args(config, game_args)]   # the game's config: its folder and appmanifest
    user_dir = localconfig.user_dir(config)
    if not show:
        diag_dir = user_dir.with_name(f'{game["name"]}-userdata-diag')   # portal2-userdata-diag as before
        if not diag_dir.exists():
            if user_dir.is_dir(): shutil.copytree(user_dir, diag_dir, symlinks=True)
            else: diag_dir.mkdir(parents=True)   # no saves yet: the game starts from scratch
        user_dir = diag_dir
    command = launch.command(binary, build_dir / 'prep', config['game_root'], config['linux_root'], user_dir, game_args)
    env = launch.environment(config, game, base=env, hidden=not show)
    log = build_dir / 'launch.log'
    log.parent.mkdir(parents=True, exist_ok=True)
    with log.open('w') as output:
        try:
            result = subprocess.run(command, stdout=output, stderr=subprocess.STDOUT, timeout=timeout, env=env)
            status = result.returncode
        except subprocess.TimeoutExpired:
            status = TIMEOUT_STATUS
            output.write('\nNative launch exceeded the diagnostic timeout. This is not proof of a running game.\n')
        except FileNotFoundError:
            sys.exit(f'{binary} missing: python3 tools/build_game.py --game {game["name"]}')
    return status, log


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    games.add_argument(parser)
    parser.add_argument('--timeout', type=int, default=60)
    parser.add_argument('--show', action='store_true', help='show the window (otherwise P2_HIDDEN_WINDOWS=1)')
    parser.add_argument('--binary', type=Path,
                        help='executable to run (default: the game\'s, e.g. build/portal2-native; '
                             'A/B comparisons without touching it)')
    parser.add_argument('arguments', nargs=argparse.REMAINDER)
    args = parser.parse_args(argv)
    config = localconfig.for_game(args.game)
    game = games.profile(config)
    for key in ('game_root', 'linux_root'):
        if not config.get(key):
            sys.exit(f'{game["name"]} is not set up in config/local.json ({key} missing): '
                     f'run ./setup.sh --game {game["name"]}')
    status, log = run(config, game, args.arguments, args.binary, args.timeout, args.show)
    print(log.read_text()[-12000:])
    print(f'native process exit: {status}')
    return status if status >= 0 else 128 - status


if __name__ == '__main__':
    sys.exit(main())
