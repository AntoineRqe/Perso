import sys
import os
import unittest

sys.path.insert(0, os.path.join(os.getcwd(), ".."))
from toolbox import load, find_file_extension, add_file_extension, remove_file_extension


class ToolboxTest(unittest.TestCase):
    """Unit test for toolbox"""

    def test_load(self):
        """
        Test load function.
        """

        empty_save = os.path.join("","")
        fake_sav = os.path.join(os.getcwd(),"Saves_tests\\fake.sav")
        real_sav= os.path.join(os.getcwd(),"Saves_tests\\labybrain.sav")

        result = ""

        result = load(empty_save)
        self.assertEqual(result,None)

        result = load(fake_sav)
        self.assertEqual(result, None)

        result = load(real_sav)
        self.assertNotEqual(result, None)

    def test_find_file_extension(self):
        """
        Test find_file_extension function
        """

        result = []

        result = find_file_extension(".", "py")
        self.assertNotEqual(0, len(result))
        self.assertIn(os.path.basename(__file__), result)

        result = find_file_extension(".", "fake")
        self.assertEqual(result, [])

        result = find_file_extension(".\\Saves_tests", "sav")
        self.assertEqual(1, len(result))
        self.assertIn("labybrain.sav", result)

    def test_add_file_extension(self):
        """
        Test add_file_extension
        """

        number = 987
        name = "test"
        empty_ext = ""
        correct_ext = "txt"
        result = str()

        result = add_file_extension(name, empty_ext)
        self.assertEqual(result, None)

        result = add_file_extension(name, correct_ext)
        self.assertEqual(result, "{}.{}".format(name, correct_ext))

        result = add_file_extension(name, number)
        self.assertEqual(result, None)

    def test_remove_file_extension(self):
        """
        Test add_file_extension
        """
        correct_name = "test.txt"
        empty_name = ""
        number = 987
        incorrect_name = "abcdsds"
        result = str()

        result = remove_file_extension(correct_name)
        self.assertEqual(result, "test")

        result = remove_file_extension(empty_name)
        self.assertEqual(result, None)

        result = remove_file_extension(number)
        self.assertEqual(result, None)

        result = remove_file_extension(incorrect_name)
        self.assertEqual(result, None)

if __name__ == "__main__":
    unittest.main()