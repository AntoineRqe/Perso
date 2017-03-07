import sys
import os
import unittest

sys.path.insert(0, os.path.join(os.getcwd(), ".."))
import maze

class MazeTest(unittest.TestCase):
    """Unit test for toolbox"""

    def test_init(self):
        """
        Test init function
        """
