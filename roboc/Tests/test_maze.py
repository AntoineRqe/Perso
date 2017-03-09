import sys
import os
sys.path.insert(0, os.path.join(os.getcwd(), ".."))
import unittest
from maze import *



test_maps = ["OOOOO",
             "O  UO",
             "OX  O",
             "OOOOO"]

clean_test_maps = ["OOOOO",
                   "O  UO",
                   "O   O",
                   "OOOOO"]

test_maps_1N = ["OOOOO",
                "OX UO",
                "O   O",
                "OOOOO"]

test_maps_1S = ["OOOOO",
                "O  UO",
                "O   O",
                "OXOOO"]

test_maps_1E = ["OOOOO",
                "O  UO",
                "O X O",
                "OOOOO"]

test_maps_1W = ["OOOOO",
                "O  UO",
                "X   O",
                "OOOOO"]


class MazeTest(unittest.TestCase):
    """Unit test for toolbox"""

    def test_init(self):
        """
        Test init function from Maze
        """
        test = Maze(test_maps)
        self.assertEqual(test_maps, test.map)
        self.assertEqual(clean_test_maps, test.clean_map)
        self.assertEqual((5, 4), test.size)
        self.assertEqual((1, 2), test.robot_position)
        self.assertEqual((3, 1), test.exit_position)

        with self.assertRaises(TypeError):
            test = Maze()

        with self.assertRaises(TypeError):
            test = Maze([])

    def test_len(self):
        """
        Test len function from Maze.
        """
        test = Maze(test_maps)
        self.assertEqual(test.len(),(5,4))

    def test_calculate_coordinate(self):
        """
        Test calculate_coordinate function from Maze.
        """
        test = Maze(test_maps)
        self.assertEqual((1, 2), test.robot_position)
        self.assertEqual(test.calculate_coordinate("N", 1), (1, 1))
        self.assertEqual(test.calculate_coordinate("E", 2), (3, 2))
        self.assertEqual(test.calculate_coordinate("S", 1), (1, 3))

        with self.assertRaises(CoordinateOutOfRange):
            test.calculate_coordinate("W", 10), (-1, -1)
        with self.assertRaises(CoordinateOutOfRange):
            test.calculate_coordinate("Q", 1), (-1, -1)
        with self.assertRaises(CoordinateOutOfRange):
            test.calculate_coordinate(900, 10), (-1, -1)
        with self.assertRaises(CoordinateOutOfRange):
            test.calculate_coordinate(0, 0), (-1, -1)
        with self.assertRaises(CoordinateOutOfRange):
            test.calculate_coordinate("", ""), (-1, -1)

    def test_update_robot_position(self):
        """
        Test update_robot_position function
        """
        test = Maze(test_maps)

        # Test North - South
        test.update_robot_position((1, 1))
        self.assertEqual(test.map, test_maps_1N)

        test.update_robot_position((1, 2))
        self.assertEqual(test.map, test_maps)

        test.update_robot_position((1, 3))
        self.assertEqual(test.map, test_maps_1S)

        test.update_robot_position((1, 2))
        self.assertEqual(test.map, test_maps)

        # Test West - East
        test.update_robot_position((2, 2))
        self.assertEqual(test.map, test_maps_1E)

        test.update_robot_position((1, 2))
        self.assertEqual(test.map, test_maps)

        test.update_robot_position((0, 2))
        self.assertEqual(test.map, test_maps_1W)

        test.update_robot_position((1, 2))
        self.assertEqual(test.map, test_maps)

        with self.assertRaises(TypeError):
            test.update_robot_position(("1", "2"))

        with self.assertRaises(IndexError):
            test.update_robot_position((10, 10))

    def test_is_maze_resolved(self):
        """
        Test is_maze_resolved function from Maps.
        """
        test = Maze(test_maps)
        self.assertFalse(test.is_maze_resolved())
        test.update_robot_position((1, 1))
        self.assertFalse(test.is_maze_resolved())
        test.update_robot_position((3, 1))
        self.assertTrue(test.is_maze_resolved())

    def test_is_itinerary_clear(self):
        """
        Test is_itinerary_clear function
        """
        test = Maze(test_maps)
        self.assertTrue(test.is_itinerary_clear("N",1))
        self.assertFalse(test.is_itinerary_clear("S", 1))
        self.assertTrue(test.is_itinerary_clear("E", 1))
        self.assertFalse(test.is_itinerary_clear("W", 1))
        self.assertFalse(test.is_itinerary_clear("E", 10))
        self.assertFalse(test.is_itinerary_clear("S", 2))
        self.assertFalse(test.is_itinerary_clear("", 2))
        self.assertFalse(test.is_itinerary_clear(10, "2"))
        self.assertFalse(test.is_itinerary_clear("EAS", "1"))
        self.assertFalse(test.is_itinerary_clear("NAZ", -1))
        self.assertFalse(test.is_itinerary_clear("N", 2))
        self.assertFalse(test.is_itinerary_clear("E", 3))

    def test_parse_command(self):
        """
        Test parse command function
        """
        test = Maze(test_maps)
        with self.assertRaises(InvalidCommands):
            test.parse_command("abc")
        with self.assertRaises(TypeError):
            test.parse_command(133)
        with self.assertRaises(EmptyOptions):
            test.parse_command("")

        (cmd, direction) = test.parse_command("N")
        self.assertEqual((cmd, direction), ("N",1))

        (cmd, direction) = test.parse_command("E2")
        self.assertEqual((cmd, direction), ("E", 2))

        (cmd, direction) = test.parse_command("S1")
        self.assertEqual((cmd, direction), (-1, -1))

        (cmd, direction) = test.parse_command("W10")
        self.assertEqual((cmd, direction), (-1, -1))

    def test_repr(self):
        test = Maze(test_maps)
        repr = test.__repr__().rsplit("\r\n")
        self.assertEqual(repr[:-1], test_maps)

    def test_save(self):
        """
        Test  save function
        """
if __name__ == "__main__":
    unittest.main()