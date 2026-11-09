#!/usr/bin/env python3
"""Pick the game language and print the `-language <name>` argument to pass it.

The Source engine reads `-language` (localize.so, filesystem_stdio.so): it loads
resource/*_<language>.txt, closecaption_<language> and adds the portal2_<language>
folder to the search paths. The Mac Steam client does not know the language
chosen in the properties of a game installed by another client (Whisky,
CrossOver): without the argument the game starts in English.

Order of choice (the first that gives a language present in the data):
  1. `-language` already passed to the launcher: nothing is added;
  2. P2_LANGUAGE environment variable;
  3. "language" key of config/local.json;
  4. game language in steamapps/appmanifest_<steam_appid>.acf (UserConfig);
  5. system language (macOS);
  6. english.
A language whose files (gameui_<language>.txt, in portal2/resource or, for
Half-Life 2 ("game": "hl2" in the config the tools and the app pass), in
hl2/resource) are missing falls back to english.
Steam only downloads the voices of the language chosen there: without them the
text is translated and the voices stay in English.

    python3 tools/language.py [--config config/local.json] [--explain] [-- launcher arguments...]
"""
import argparse
import json
import os
import re
import subprocess
import sys
from pathlib import Path

# Noms de langue Steam / Source, tels que dans resource/gameui_<nom>.txt.
LANGUAGES = (
    'brazilian', 'bulgarian', 'czech', 'danish', 'dutch', 'english', 'finnish', 'french', 'german', 'greek',
    'hungarian', 'italian', 'japanese', 'korean', 'koreana', 'latam', 'norwegian', 'polish', 'portuguese',
    'romanian', 'russian', 'schinese', 'spanish', 'swedish', 'tchinese', 'thai', 'turkish', 'ukrainian',
    'vietnamese')

# Code ISO / BCP 47 (préfixe) -> nom Steam. Les cas régionaux sont traités avant.
ISO = {
    'bg': 'bulgarian', 'cs': 'czech', 'da': 'danish', 'nl': 'dutch', 'en': 'english', 'fi': 'finnish',
    'fr': 'french', 'de': 'german', 'el': 'greek', 'hu': 'hungarian', 'it': 'italian', 'ja': 'japanese',
    'ko': 'koreana', 'nb': 'norwegian', 'nn': 'norwegian', 'no': 'norwegian', 'pl': 'polish',
    'pt': 'portuguese', 'ro': 'romanian', 'ru': 'russian', 'es': 'spanish', 'sv': 'swedish', 'th': 'thai',
    'tr': 'turkish', 'uk': 'ukrainian', 'vi': 'vietnamese', 'zh': 'schinese'}


def normalize(value):
    """Steam name for `value` (name, ISO code or `fr_CA` locale), None if unknown."""
    if not value: return None
    text = value.strip().lower().replace('_', '-')
    if text in LANGUAGES: return text
    if text.startswith('pt-br'): return 'brazilian'
    if re.match(r'es-(419|mx|ar|cl|co|us)', text): return 'latam'
    if text.startswith('zh') and re.search(r'hant|-tw|-hk|-mo', text): return 'tchinese'
    return ISO.get(text.split('-')[0])


def acf_language(text):
    """Language of the UserConfig block (else MountedConfig) of an appmanifest_*.acf."""
    found = {}
    for block in ('UserConfig', 'MountedConfig'):
        match = re.search(r'"%s"\s*\{([^}]*)\}' % block, text)
        if match:
            language = re.search(r'"language"\s+"([^"]*)"', match.group(1))
            if language: found[block] = language.group(1)
    return normalize(found.get('UserConfig') or found.get('MountedConfig'))


def system_language():
    """First recognised language of the macOS settings (AppleLanguages), None elsewhere."""
    try:
        output = subprocess.run(['/usr/bin/defaults', 'read', '-g', 'AppleLanguages'], capture_output=True,
                                text=True, timeout=5).stdout
    except (OSError, subprocess.SubprocessError):
        return None
    for code in re.findall(r'[A-Za-z]{2,3}(?:-[A-Za-z0-9]+)*', output):
        language = normalize(code)
        if language: return language
    return None


# Folder of gameui_<language>.txt per game (tools/games.py names); this script
# is copied into the apps, so it stays standalone.
RESOURCES = {'portal2': 'portal2/resource', 'hl2': 'hl2/resource'}


def acf_candidates(game_root, appid=620):
    """Where Steam keeps appmanifest_<appid>.acf: next to the game (steamapps/common/<game>), then native Steam."""
    candidates = []
    if game_root: candidates.append(Path(game_root).parent.parent / f'appmanifest_{appid}.acf')
    candidates.append(Path.home() / f'Library/Application Support/Steam/steamapps/appmanifest_{appid}.acf')
    return candidates


def installed(language, game_root, game='portal2'):
    """True when the game holds this language's text (English is always there)."""
    if language == 'english': return True
    folder = RESOURCES.get(game, RESOURCES['portal2'])
    return bool(game_root) and (Path(game_root) / folder / f'gameui_{language}.txt').is_file()


def explicit_language(arguments):
    """Value of `-language` in an argument list, None when absent."""
    for index, argument in enumerate(arguments):
        if argument == '-language': return arguments[index + 1] if index + 1 < len(arguments) else ''
    return None


def choose(env=None, config=None, game_root=None, acf_texts=(), system=None):
    """Return (language, origin). `acf_texts`: appmanifest contents, in order of preference."""
    env = os.environ if env is None else env
    config = config or {}
    game_root = game_root or config.get('game_root')
    game = config.get('game') or 'portal2'
    acf = f'appmanifest_{config.get("steam_appid") or 620}.acf'
    for origin, value in (('P2_LANGUAGE', env.get('P2_LANGUAGE')), ('config/local.json', config.get('language'))):
        if not value: continue
        language = normalize(value)
        if language and installed(language, game_root, game): return language, origin
        return 'english', f'{origin}: "{value}" inconnue ou absente des données du jeu'
    for text in acf_texts:
        language = acf_language(text)
        if language and installed(language, game_root, game): return language, acf
        if language: return 'english', f'{acf}: langue absente des données du jeu'
    language = system() if callable(system) else system
    if language and installed(language, game_root, game): return language, 'système'
    return 'english', 'défaut'


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('--config', type=Path, default=Path(__file__).resolve().parent.parent / 'config/local.json')
    parser.add_argument('--explain', action='store_true', help='say on stderr where the choice comes from')
    parser.add_argument('arguments', nargs=argparse.REMAINDER)
    args = parser.parse_args()
    launcher_args = args.arguments[1:] if args.arguments[:1] == ['--'] else args.arguments
    try: config = json.loads(args.config.read_text())
    except FileNotFoundError: config = {}
    except (OSError, ValueError) as exc:   # unreadable or malformed: say so rather than silently defaulting
        print(f'language: {args.config}: {exc}; using the default language choice', file=sys.stderr)
        config = {}
    words = language_args(config, launcher_args, explain=args.explain)
    if words: print(*words)
    return 0


def language_args(config, launcher_args=(), explain=False):
    """['-language', <name>] for `config` (one game's: "game", "game_root",
    "steam_appid"), [] when the launcher arguments already name one."""
    if explicit_language(list(launcher_args)) is not None: return []
    texts = []
    for path in acf_candidates(config.get('game_root'), config.get('steam_appid') or 620):
        try: texts.append(path.read_text(errors='replace'))
        except OSError: pass
    language, origin = choose(config=config, acf_texts=texts, system=system_language)
    if explain: print(f'language: {language} ({origin})', file=sys.stderr)
    return ['-language', language]


if __name__ == '__main__':
    sys.exit(main())
