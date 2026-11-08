#!/usr/bin/env python3
"""Signe un paquet .app iOS pour un appareil réel.

Prend le profil de provisionnement le plus récent obtenu par Xcode pour le
CFBundleIdentifier du paquet (voir ios/signing/project.yml), l'embarque et
signe avec l'identité « Apple Development » de son équipe et les droits du
profil.

    python3 ios/signing/sign.py build/ios/device/Portal2.app
"""
import argparse
import hashlib
import plistlib
import subprocess
import sys
import tempfile
from pathlib import Path

PROFILES = [Path.home() / 'Library/Developer/Xcode/UserData/Provisioning Profiles',
            Path.home() / 'Library/MobileDevice/Provisioning Profiles']


def decode(profile):
    raw = subprocess.run(['security', 'cms', '-D', '-i', str(profile)], capture_output=True, check=True).stdout
    return plistlib.loads(raw)


def find_profile(bundle_id):
    best = None
    for folder in PROFILES:
        for path in folder.glob('*.mobileprovision') if folder.is_dir() else []:
            data = decode(path)
            app_id = data['Entitlements'].get('application-identifier', '')
            if app_id.split('.', 1)[-1] != bundle_id:
                continue
            if best is None or data['CreationDate'] > best[1]['CreationDate']:
                best = (path, data)
    if not best:
        sys.exit(f'aucun profil pour {bundle_id} : lancer xcodebuild sur ios/signing (voir project.yml)')
    return best


def pick_identity(find_identity_output, profile_certificates):
    """Empreinte de la première identité de signature dont le certificat est
    dans le profil. Avec plusieurs comptes Apple dans Xcode (équipe gratuite
    et équipe payante), la première identité « Apple Development » venue
    pouvait être celle d'une autre équipe : l'iPhone refusait alors l'app."""
    allowed = {hashlib.sha1(der).hexdigest().upper() for der in profile_certificates}
    for line in find_identity_output.splitlines():
        fields = line.split()
        if len(fields) > 1 and fields[1].upper() in allowed:
            return fields[1]   # empreinte : sans ambiguïté s'il y a plusieurs certificats
    sys.exit('aucune identité du trousseau ne correspond aux certificats du profil '
             '(Xcode > Settings > Accounts > Manage Certificates)')


def identity(profile):
    found = subprocess.run(['security', 'find-identity', '-v', '-p', 'codesigning'],
                           capture_output=True, text=True, check=True).stdout
    return pick_identity(found, profile.get('DeveloperCertificates', []))


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('app', type=Path, help='paquet .app iOS à signer')
    app = ap.parse_args().app
    if not (app / 'Info.plist').is_file():
        sys.exit(f'{app} : pas de paquet .app (Info.plist absent)')
    bundle_id = plistlib.loads((app / 'Info.plist').read_bytes())['CFBundleIdentifier']
    path, data = find_profile(bundle_id)
    team = data['TeamIdentifier'][0]
    (app / 'embedded.mobileprovision').write_bytes(path.read_bytes())
    with tempfile.TemporaryDirectory() as tmp:
        entitlements = Path(tmp) / 'entitlements.plist'
        entitlements.write_bytes(plistlib.dumps(data['Entitlements']))
        subprocess.run(['codesign', '-f', '-s', identity(data), '--entitlements', str(entitlements),
                        '--generate-entitlement-der', str(app)], check=True)
    print(f'{app} signé ({bundle_id}, équipe {team}, profil expirant le {data["ExpirationDate"]:%Y-%m-%d})')


if __name__ == '__main__':
    main()
