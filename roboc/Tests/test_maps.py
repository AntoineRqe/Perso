import sys
import os
sys.path.insert(0, os.path.join(os.getcwd(), ".."))
import unittest
from maps import Maps


path_to_map = os.path.join(os.getcwd(),"Maps_tests")

facile_dict = {'facile': ['OOOOOOOOOO',
                          'O O    O O',
                          'O . OO   O',
                          'O O O   XO',
                          'O OOOO O.O',
                          'O O O    U',
                          'O OOOOOO.O',
                          'O O      O',
                          'O O OOOOOO',
                          'O . O    O',
                          'OOOOOOOOOO']}


class MapsTest(unittest.TestCase):
    """
    Unit test for Maps class
    """

    def test_init(self):
        """
        Test init from map
        """

        test = Maps()
        self.assertEqual(test.drawings, {})
        self.assertEqual(test.names, [])
        self.assertEqual(len(test.__dict__), 2)

    def test_has_map_good_format(self):
        """
        Test decode_map from Maps
        """
        test = Maps()
        result = test.has_map_good_format(path_to_map,"fake")
        self.assertFalse(result)

        test = Maps()
        result = test.has_map_good_format(path_to_map, "empty.txt")
        self.assertFalse(result)

        test = Maps()
        result = test.has_map_good_format(path_to_map, "facile.txt")
        self.assertTrue(result)

    def test_load_map(self):
        """
        Test load_map from Maps
        """
        test = Maps()
        test.load_map(path_to_map)
        self.assertIn("facile", test.names)
        self.assertNotIn("empty", test.names)
        self.assertEqual(len(test.drawings), 1)


if __name__ == "__main__":
    unittest.main()
