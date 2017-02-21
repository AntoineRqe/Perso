# encoding utf-8

from toolbox import *
from errors import *
from pickle import *
from maps import *
import os


def find_entrance(maze):
    """
    Find 'E' in a list of string
    :param maze: the list of string to parse
    :return: a tuple with the coordinate of the entrance
    """
    for y, map_line in enumerate(maze):
        if 'E' in map_line:
            return str(map_line).index('E'), y


def find_exit(maze):
    """
    Find 'U' in a list of string
    :param maze: the list of string to parse
    :return: a tuple with the coordinate of the entrance
    """
    for y, map_line in enumerate(maze):
        if 'U' in map_line:
            return str(map_line).index('U'), y


class Maze:

    def __init__(self, name):
        if type(name) != str:
            raise TypeError("name, difficulty are not a string")

        if type(map_catalog[name]) != list:
            raise TypeError("map is not a list")

        self.name = name
        self.map = map_catalog[name]
        self.size = self.len()
        self.entrance_position = find_entrance(self.map)
        self.exit_position = find_exit(self.map)
        self.robot_position = self.entrance_position
        self.init_robot_position()

    def __repr__(self):
        map_str = str()
        for map_line in self.map:
            map_str += map_line + "\r\n"
        return str(map_str)

    def len(self):
        """
        Function to get the dimension of the maze
        :return:
        """
        return len(self.map[0]), len(self.map)

    def init_robot_position(self):
        """
        Move the robot to the entrance of the maze
        """
        map_line = list(self.map[self.entrance_position[1]])
        map_line[self.entrance_position[0]] = "R"
        map_str = "".join(map_line)
        self.map[self.entrance_position[1]] = map_str

    def update_robot_position(self, direction, step):
        """
        Update position of the robot into the maze
        :param direction: direction for the robot to move
        :param step: number of step to do
        """

        new_x, new_y = self.calculate_coordinate(direction, step)

        # -------------------------------
        # Delete previous robot position
        # -------------------------------
        map_str = str(self.map[self.robot_position[1]])
        if self.robot_position == self.entrance_position:
            index = map_str.index("R")
            map_list = list(map_str)
            map_list[index] = 'E'
        else:
            map_list = list(map_str)
            map_list[self.robot_position[0]] = ' '

        self.map[self.robot_position[1]] = "".join(map_list)
        # -------------------------------
        # Update robot position
        # -------------------------------
        map_str = str(self.map[new_y])
        map_list = list(map_str)
        map_list[new_x] = 'R'
        self.map[new_y] = "".join(map_list)

        self.robot_position = (new_x, new_y)

    def parse_command(self, cmd_direction, cmd_steps):
        """
        Parse the command given to mode
        :param cmd_direction: command to be parsed
        :param cmd_steps: number of step in that direction
        :return: True if command valid, False otherwise
        """

        itinerary = list()
        ret = True

        try:
            if cmd_direction == 'N':
                for i in range(0, cmd_steps + 1):
                    itinerary.append(self.map[(self.robot_position[1]) - i][self.robot_position[0]])
                if 'X' in itinerary:
                    ret = False

            elif cmd_direction == 'S':
                for i in range(0, cmd_steps + 1):
                    itinerary.append(self.map[(self.robot_position[1]) + i][self.robot_position[0]])
                if 'X' in itinerary:
                    ret = False

            elif cmd_direction == 'E':
                for i in range(0, cmd_steps + 1):
                    itinerary.append(self.map[self.robot_position[1]][self.robot_position[0] + i])
                if 'X' in itinerary:
                    ret = False

            elif cmd_direction == 'W':
                for i in range(0, cmd_steps + 1):
                    itinerary.append(self.map[self.robot_position[1]][self.robot_position[0] - i])
                if 'X' in itinerary:
                    ret = False

            elif cmd_direction == 'Q':
                self.save()
                ret = True

            elif self.calculate_coordinate(cmd_direction, cmd_steps) == (-1, -1):
                ret = False

            print "itinerary : {}".format(itinerary)

        except IndexError as e:
            print "You have encounter an obstacle with move {}{}".format(cmd_direction, cmd_steps)
            ret = False

        if not ret:
            print "You have encounter an obstacle with move {}{}".format(cmd_direction, cmd_steps)
            print "Please, Enter new command"

        return ret

    def calculate_coordinate(self, direction, step):
        """
        Calculate new coordinate to move the robot
        :param direction: direction to move to
        :param step: number of step to move
        :return: tuple of the new coordinate, (-1, -1) if invalids coordinate
        """

        if direction == "N":
            x, y = self.robot_position[0], self.robot_position[1] - step
        elif direction == "E":
            x, y = self.robot_position[0] + step, self.robot_position[1]
        elif direction == "S":
            x, y = self.robot_position[0], self.robot_position[1] + step
        elif direction == "W":
            x, y = self.robot_position[0] - step , self.robot_position[1]

        if x < 0 or y < 0:
            raise CoordinateOutOfRange((x,y))
            return -1, -1

        return x, y

    def is_maze_resolved(self):
        """
        Check if Robot found the exit
        :return: True if exit found, False otherwise
        """
        if self.robot_position == self.exit_position:
            return True
        else:
            return False

    def save(self):
        """
        Save the game in binary file name.sav and quit the game
        """
        confirm = True
        file_name = "{}.sav".format(self.name)
        print "File to save {}".format(file_name)

        if os.path.isfile(file_name):
            ret = str(raw_input("Are you sure you want to erase save {}? (Y/N)\r\n".format(file_name)))
            while ret.upper() not in ("Y", "N"):
                ret = str(raw_input("Are you sure you want to erase save {}? (Y/N)\r\n".format(file_name)))
            if ret.upper() == "N":
                confirm = False
            else:
                confirm = True

        if confirm:
            with open(file_name, 'wb') as save:
                my_pickler = Pickler(save)
                my_pickler.dump(self)
            print "File {} saved".format(file_name)
        exit()
