import sys
import os
sys.path.insert(0, os.path.join(os.getcwd(), ".."))
import unittest
try:
    from unittest import mock
except ImportError:
    import mock

import time
from maze import Maze, increment_number
from custom_errors import CoordinateOutOfRange

test_maps = ["OOOOO",
             "O  UO",
             "O1  O",
             "OOOOO"]

test_maps_door_W = ["OOOOO",
                    "O  UO",
                    ".   O",
                    "OOOOO"]

clean_test_maps = ["OOOOO",
                   "O  UO",
                   "O   O",
                   "OOOOO"]

clean_test_maps_door_W = ["OOOOO",
                          "O  UO",
                          ".   O",
                          "OOOOO"]

test_maps_1N = ["OOOOO",
                "O1 UO",
                "O   O",
                "OOOOO"]

test_maps_1S = ["OOOOO",
                "O  UO",
                "O   O",
                "O1OOO"]

test_maps_1E = ["OOOOO",
                "O  UO",
                "O 1 O",
                "OOOOO"]

test_maps_1W = ["OOOOO",
                "O  UO",
                "1   O",
                "OOOOO"]

player_name = "Antoine"


class MazeTest(unittest.TestCase):
    """Unit test for toolbox"""

    @mock.patch("maze.increment_number", return_value="1")
    def setUp(self, return_value):
        self.maze = Maze(clean_test_maps, [player_name])
        self.maze.update_robot_position(player_name, (1, 2))

    def tearDown(self):
        self.maze = None

    def test_init(self):
        """
        Test init function from Maze
        """

        for line in test_maps:
            print(line)
        print()
        print(self.maze)
        time.sleep(1)

        self.assertEqual(test_maps, self.maze.map)
        self.assertEqual(clean_test_maps, self.maze.clean_map)
        self.assertEqual((5, 4), self.maze.size)
        self.assertEqual((1, 2), self.maze.robot_position[player_name])
        self.assertEqual((3, 1), self.maze.exit_position)

        with self.assertRaises(TypeError):
            self.maze = Maze()

        with self.assertRaises(TypeError):
            self.maze = Maze([])

    def test_len(self):
        """
        Test len function from Maze.
        """
        self.assertEqual(self.maze.len(),(5,4))

    def test_calculate_coordinate(self):
        """
        Test calculate_coordinate function from Maze.
        """

        self.assertEqual((1, 2), self.maze.robot_position[player_name])
        self.assertEqual(self.maze.calculate_coordinate("N", 1), (1, 1))
        self.assertEqual(self.maze.calculate_coordinate("E", 2), (3, 2))
        self.assertEqual(self.maze.calculate_coordinate("S", 1), (1, 3))

        with self.assertRaises(CoordinateOutOfRange):
            self.maze.calculate_coordinate("W", 10), (-1, -1)
        with self.assertRaises(CoordinateOutOfRange):
            self.maze.calculate_coordinate("Q", 1), (-1, -1)
        with self.assertRaises(CoordinateOutOfRange):
            self.maze.calculate_coordinate(900, 10), (-1, -1)
        with self.assertRaises(CoordinateOutOfRange):
            self.maze.calculate_coordinate(0, 0), (-1, -1)
        with self.assertRaises(CoordinateOutOfRange):
            self.maze.calculate_coordinate("", ""), (-1, -1)

    def test_update_robot_position(self):
        """
        Test update_robot_position function
        """

        # Test North - South
        self.maze.update_robot_position(player_name, (1, 1))
        self.assertEqual(self.maze.map, test_maps_1N)

        self.maze.update_robot_position(player_name, (1, 2))
        self.assertEqual(self.maze.map, test_maps)

        # Test West - East
        self.maze.update_robot_position(player_name, (2, 2))
        self.assertEqual(self.maze.map, test_maps_1E)

        self.maze.update_robot_position(player_name, (1, 2))
        self.assertEqual(self.maze.map, test_maps)

        with self.assertRaises(TypeError):
            self.maze.update_robot_position(player_name, ("1", "2"))

        with self.assertRaises(IndexError):
            self.maze.update_robot_position(player_name, (10, 10))

    def test_is_maze_resolved(self):
        """
        Test is_maze_resolved function from Maps.
        """
        self.assertFalse(self.maze.is_maze_resolved())
        self.maze.update_robot_position(player_name, (1, 1))
        self.assertFalse(self.maze.is_maze_resolved())
        self.maze.update_robot_position(player_name, (3, 1))
        self.assertTrue(self.maze.is_maze_resolved())

    def test_is_itinerary_clear(self):
        """
        Test is_itinerary_clear function
        """
        self.assertTrue(self.maze.is_itinerary_clear("N1"))
        self.assertFalse(self.maze.is_itinerary_clear("S1"))
        self.assertTrue(self.maze.is_itinerary_clear("E1"))
        self.assertFalse(self.maze.is_itinerary_clear("W1"))
        self.assertFalse(self.maze.is_itinerary_clear("E10"))
        self.assertFalse(self.maze.is_itinerary_clear("S2"))
        self.assertFalse(self.maze.is_itinerary_clear("2"))
        self.assertFalse(self.maze.is_itinerary_clear("N2"))
        self.assertFalse(self.maze.is_itinerary_clear("E3"))

    def test_is_command_valid(self):

        self.assertFalse(self.maze.is_command_valid(""))
        self.assertFalse(self.maze.is_command_valid("A"))
        self.assertFalse(self.maze.is_command_valid("9"))

        self.assertTrue(self.maze.is_command_valid("N"))
        self.assertFalse(self.maze.is_command_valid("NN"))
        self.assertFalse(self.maze.is_command_valid("N/"))
        self.assertFalse(self.maze.is_command_valid("N-10"))
        self.assertFalse(self.maze.is_command_valid("S1"))
        self.assertFalse(self.maze.is_command_valid("W2"))
        self.assertFalse(self.maze.is_command_valid("E8"))

        self.assertTrue(self.maze.is_command_valid("Q"))
        self.assertFalse(self.maze.is_command_valid("Q1"))

        self.assertFalse(self.maze.is_command_valid("M"))
        self.assertFalse(self.maze.is_command_valid("MM"))
        self.assertFalse(self.maze.is_command_valid("M9"))
        self.assertFalse(self.maze.is_command_valid("M."))
        self.assertTrue(self.maze.is_command_valid("MN"))

    def test_move(self):
        self.maze.move("N1")
        self.assertEqual(self.maze.map, test_maps_1N)
        self.maze.move("S1")
        self.assertEqual(self.maze.map, test_maps)

        self.maze.move("E1")
        self.assertEqual(self.maze.map, test_maps_1E)
        self.maze.move("W1")
        self.assertEqual(self.maze.map, test_maps)

    def test_put_wall(self):
        self.maze.put_door("PW")
        self.assertEqual(self.maze.clean_map, clean_test_maps_door_W)

        self.maze.put_wall("MW")
        self.assertEqual(self.maze.clean_map, clean_test_maps)

    def test_init_robot_position(self):
        for i in range(0, 100):
            self.assertGreater(self.maze.size[0], self.maze.init_robot_position()[0])
            self.assertLessEqual(0, self.maze.init_robot_position()[0])
            self.assertGreater(self.maze.size[1], self.maze.init_robot_position()[1])
            self.assertLessEqual(0, self.maze.init_robot_position()[1])

    @staticmethod
    def test_save():
        """
        Test  save function
        """
        pass

if __name__ == "__main__":
    unittest.main()
