import importlib.util
from pathlib import Path
import tempfile
import unittest

spec = importlib.util.spec_from_file_location('language', Path(__file__).resolve().parents[1] / 'tools/language.py')
language = importlib.util.module_from_spec(spec)
spec.loader.exec_module(language)

ACF = '''"AppState"
{
	"appid"		"620"
	"UserConfig"
	{
		"language"		"%s"
	}
	"MountedConfig"
	{
		"language"		"%s"
	}
}
'''


class LanguageTests(unittest.TestCase):
    def game(self, *names):
        directory = tempfile.TemporaryDirectory()
        self.addCleanup(directory.cleanup)
        resource = Path(directory.name) / 'portal2/resource'
        resource.mkdir(parents=True)
        for name in names: (resource / f'gameui_{name}.txt').write_text('x')
        return directory.name

    def test_acf_reads_user_config_before_mounted_config(self):
        self.assertEqual(language.acf_language(ACF % ('french', 'english')), 'french')
        self.assertEqual(language.acf_language('"UserConfig"\n{\n}\n"MountedConfig"\n{\n"language" "german"\n}'), 'german')
        self.assertIsNone(language.acf_language('"AppState" { }'))

    def test_normalize_names_codes_and_regions(self):
        self.assertEqual(language.normalize('French'), 'french')
        self.assertEqual(language.normalize('fr-FR'), 'french')
        self.assertEqual(language.normalize('pt-BR'), 'brazilian')
        self.assertEqual(language.normalize('pt-PT'), 'portuguese')
        self.assertEqual(language.normalize('es-419'), 'latam')
        self.assertEqual(language.normalize('zh-Hant-TW'), 'tchinese')
        self.assertEqual(language.normalize('zh-Hans-CN'), 'schinese')
        self.assertEqual(language.normalize('ko-KR'), 'koreana')
        self.assertIsNone(language.normalize('klingon'))

    def test_priority_env_then_config_then_acf_then_system(self):
        root = self.game('french', 'german', 'spanish', 'italian')
        common = dict(game_root=root, acf_texts=[ACF % ('german', 'german')], system='italian')
        self.assertEqual(language.choose(env={'P2_LANGUAGE': 'spanish'}, config={'language': 'french'}, **common)[0], 'spanish')
        self.assertEqual(language.choose(env={}, config={'language': 'french'}, **common)[0], 'french')
        self.assertEqual(language.choose(env={}, **common)[0], 'german')
        self.assertEqual(language.choose(env={}, game_root=root, system='italian')[0], 'italian')
        self.assertEqual(language.choose(env={}, game_root=root, system=None)[0], 'english')

    def test_missing_game_files_fall_back_to_english(self):
        root = self.game()
        self.assertEqual(language.choose(env={}, config={'language': 'french'}, game_root=root)[0], 'english')
        self.assertEqual(language.choose(env={}, game_root=root, acf_texts=[ACF % ('french', 'french')])[0], 'english')
        self.assertEqual(language.choose(env={}, game_root=root, system='french')[0], 'english')

    def test_unknown_explicit_language_is_not_replaced_by_system(self):
        root = self.game('french')
        self.assertEqual(language.choose(env={'P2_LANGUAGE': 'klingon'}, game_root=root, system='french')[0], 'english')

    def test_explicit_language_flag_detected(self):
        self.assertEqual(language.explicit_language(['-game', 'portal2', '-language', 'german']), 'german')
        self.assertIsNone(language.explicit_language(['-game', 'portal2']))

    def test_acf_candidates_start_next_to_the_game(self):
        self.assertEqual(language.acf_candidates('/s/steamapps/common/Portal 2')[0], Path('/s/steamapps/appmanifest_620.acf'))


if __name__ == '__main__':
    unittest.main()
