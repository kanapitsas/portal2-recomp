"""ios/signing/sign.py: the signing identity must be one the profile allows."""
import hashlib
import importlib.util
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
spec = importlib.util.spec_from_file_location('ios_sign', ROOT / 'ios/signing/sign.py')
sign = importlib.util.module_from_spec(spec)
spec.loader.exec_module(sign)

FREE, PAID = b'free-team certificate', b'paid-team certificate'


def sha1(der):
    return hashlib.sha1(der).hexdigest().upper()


# Two Apple accounts in Xcode: a free team listed first, the paid team second.
FIND_IDENTITY = f'''  1) {sha1(FREE)} "Apple Development: someone@icloud.com (4289AHMJB2)"
  2) {sha1(PAID)} "Apple Development: Someone (CAU4T2RD85)"
  3) {'0' * 40} "Developer ID Application: SOMEONE (3RG2R8TYFA)"
     3 valid identities found
'''


class PickIdentity(unittest.TestCase):
    def test_identity_comes_from_the_profile_certificates(self):
        # A paid-team profile: signing with the free team's certificate made
        # the iPhone refuse the app ("A valid provisioning profile for this
        # executable was not found").
        self.assertEqual(sign.pick_identity(FIND_IDENTITY, [PAID]), sha1(PAID))
        self.assertEqual(sign.pick_identity(FIND_IDENTITY, [FREE]), sha1(FREE))

    def test_no_certificate_of_the_profile_in_the_keychain(self):
        with self.assertRaises(SystemExit):
            sign.pick_identity(FIND_IDENTITY, [b'another certificate'])


if __name__ == '__main__':
    unittest.main()
