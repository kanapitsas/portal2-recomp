#!/usr/bin/env python3
"""Choisit la langue du jeu et imprime l'argument `-language <nom>` à lui passer.

Le moteur Source lit `-language` (localize.so, filesystem_stdio.so) : il charge
resource/*_<langue>.txt, closecaption_<langue> et ajoute le dossier
portal2_<langue> aux chemins de recherche. Le client Steam du Mac ne connaît
pas la langue choisie dans les propriétés d'un jeu installé par un autre
client (Whisky, CrossOver) : sans argument, le jeu démarre donc en anglais.

Ordre de choix (le premier qui donne une langue présente dans les données) :
  1. `-language` déjà passé au lanceur : rien n'est ajouté ;
  2. variable d'environnement P2_LANGUAGE ;
  3. clé "language" de config/local.json ;
  4. langue du jeu dans steamapps/appmanifest_620.acf (UserConfig) ;
  5. langue du système (macOS) ;
  6. english.
Une langue dont les fichiers (gameui_<langue>.txt) manquent retombe sur english.
Les voix ne sont téléchargées par Steam que pour la langue choisie là-bas : sans
elles, le texte est traduit et les voix restent en anglais.

    python3 tools/language.py [--config config/local.json] [--explain] [-- arguments du lanceur…]
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
    """Nom Steam pour `value` (nom, code ISO ou locale `fr_CA`), None si inconnu."""
    if not value: return None
    text = value.strip().lower().replace('_', '-')
    if text in LANGUAGES: return text
    if text.startswith('pt-br'): return 'brazilian'
    if re.match(r'es-(419|mx|ar|cl|co|us)', text): return 'latam'
    if text.startswith('zh') and re.search(r'hant|-tw|-hk|-mo', text): return 'tchinese'
    return ISO.get(text.split('-')[0])


def acf_language(text):
    """Langue du bloc UserConfig (sinon MountedConfig) d'un appmanifest_*.acf."""
    found = {}
    for block in ('UserConfig', 'MountedConfig'):
        match = re.search(r'"%s"\s*\{([^}]*)\}' % block, text)
        if match:
            language = re.search(r'"language"\s+"([^"]*)"', match.group(1))
            if language: found[block] = language.group(1)
    return normalize(found.get('UserConfig') or found.get('MountedConfig'))


def system_language():
    """Première langue reconnue des réglages macOS (AppleLanguages), None ailleurs."""
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
    """Où Steam range appmanifest_620.acf : à côté du jeu (steamapps/common/<jeu>), puis Steam natif."""
    candidates = []
    if game_root: candidates.append(Path(game_root).parent.parent / 'appmanifest_620.acf')
    candidates.append(Path.home() / 'Library/Application Support/Steam/steamapps/appmanifest_620.acf')
    return candidates


def installed(language, game_root):
    """Vrai si le jeu contient le texte de cette langue (l'anglais est toujours là)."""
    if language == 'english': return True
    return bool(game_root) and (Path(game_root) / 'portal2/resource' / f'gameui_{language}.txt').is_file()


def explicit_language(arguments):
    """Valeur de `-language` dans une liste d'arguments, None s'il n'y est pas."""
    for index, argument in enumerate(arguments):
        if argument == '-language': return arguments[index + 1] if index + 1 < len(arguments) else ''
    return None


def choose(env=None, config=None, game_root=None, acf_texts=(), system=None):
    """Retourne (langue, origine). `acf_texts` : contenus des appmanifest, dans l'ordre de préférence."""
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
    parser.add_argument('--explain', action='store_true', help="dire sur stderr d'où vient le choix")
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
