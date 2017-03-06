import unittest
from maps import *
import toolbox


class ToolboxTest(unittest.TestCase):
    """Unit test for toolbox"""

    def test_load(self):
        """
        Test load function.
        """

        emptymap = ""
        fakemap = "fake"
        realmap= "easy"

        result = ""

        result = toolbox.load(emptymap)
        self.assertEqual(result,None)

        result = toolbox.load(fakemap)
        self.assertEqual(result, None)

        result = toolbox.load(realmap)
        self.assertEqual(result, easy)


