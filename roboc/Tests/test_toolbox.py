import sys
import os
import unittest

sys.path.insert(0, os.path.join(os.getcwd(), ".."))
from toolbox import load, find_file_extension


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
        self.assertEqual(0, len(result))

        result = find_file_extension(".\\Saves_tests", "sav")
        self.assertEqual(1, len(result))
        self.assertIn("labybrain.sav", result)