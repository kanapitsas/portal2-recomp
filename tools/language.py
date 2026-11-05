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
  4. game language in steamapps/appmanifest_620.acf (UserConfig);
  5. system language (macOS);
  6. english.
A language whose files (gameui_<language>.txt) are missing falls back to english.
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


def acf_candidates(game_root):
    """Where Steam keeps appmanifest_620.acf: next to the game (steamapps/common/<game>), then native Steam."""
    candidates = []
    if game_root: candidates.append(Path(game_root).parent.parent / 'appmanifest_620.acf')
    candidates.append(Path.home() / 'Library/Application Support/Steam/steamapps/appmanifest_620.acf')
    return candidates


def installed(language, game_root):
    """True when the game holds this language's text (English is always there)."""
    if language == 'english': return True
    return bool(game_root) and (Path(game_root) / 'portal2/resource' / f'gameui_{language}.txt').is_file()


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
    for origin, value in (('P2_LANGUAGE', env.get('P2_LANGUAGE')), ('config/local.json', config.get('language'))):
        if not value: continue
        language = normalize(value)
        if language and installed(language, game_root): return language, origin
        return 'english', f'{origin}: "{value}" inconnue ou absente des données du jeu'
    for text in acf_texts:
        language = acf_language(text)
        if language and installed(language, game_root): return language, 'appmanifest_620.acf'
        if language: return 'english', 'appmanifest_620.acf: langue absente des données du jeu'
    language = system() if callable(system) else system
    if language and installed(language, game_root): return language, 'système'
    return 'english', 'défaut'


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('--config', type=Path, default=Path(__file__).resolve().parent.parent / 'config/local.json')
    parser.add_argument('--explain', action='store_true', help='say on stderr where the choice comes from')
    parser.add_argument('arguments', nargs=argparse.REMAINDER)
    args = parser.parse_args()
    launcher_args = args.arguments[1:] if args.arguments[:1] == ['--'] else args.arguments
    if explicit_language(launcher_args) is not None: return 0
    try: config = json.loads(args.config.read_text())
    except (OSError, ValueError): config = {}
    texts = []
    for path in acf_candidates(config.get('game_root')):
        try: texts.append(path.read_text(errors='replace'))
        except OSError: pass
    language, origin = choose(config=config, acf_texts=texts, system=system_language)
    if args.explain: print(f'language: {language} ({origin})', file=sys.stderr)
    print('-language', language)
    return 0


if __name__ == '__main__':
    sys.exit(main())
