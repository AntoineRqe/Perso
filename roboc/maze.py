# encoding utf-8

from  import find_entrance, find_exit
from errors import *


class Maze:

    def __init__(self, name, difficulty, map):
        if (type(name) != str) | (type(difficulty) != str):
            raise TypeError("name, difficulty are not a string")

        if type(map) != list:
            raise TypeError("map is not a list")

        self.name = name
        self.difficulty = difficulty
        self.map = map
        self.size = self.len()
        self.entrance_position = find_entrance(self.map)
        self.exit_position = find_exit(self.map)
        self.robot_position = self.entrance_position

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

    def update_robot_position(self, cmd):
        """
        Update position of the robot into the maze
        :param cmd: command to move the robot
        :return:
        """

        cmd_direction = str(cmd[0]).upper()
        cmd_steps = str(cmd[1:]).upper()

        self.parse_command(cmd)

        map_list = list()
        map_str = str()
        x = 0
        y = 0

        if (0 > x) or (x > self.size[0]):
            raise Exception()

        if (0 > y) or (y > self.size[1]):
            raise Exception()

        for letter in self.map[self.robot_position[1]]:
            map_list.append(letter)

        if (self.entrance_position[0], self.entrance_position[1]) == (self.robot_position[0], self.robot_position[1]):
            map_list[self.robot_position[0]] = 'E'
        else:
            map_list[self.robot_position[0]] = ' '

        map_str = "".join(map_list)
        self.map[self.robot_position[1]] = map_str

        map_list = list()
        map_str = str()

        for letter in self.map[y]:
            map_list.append(letter)

        map_list[x] = 'R'
        map_str = "".join(map_list)

        self.map[y] = map_str
        self.robot_position = (x, y)

    def parse_command(self, cmd):
        """
        Parse the command given to mode
        :param cmd: command to be parsed
        :return: True if command valid, False otherwise
        """
        cmd_direction = str(cmd[0])
        cmd_steps = str(cmd[1:])

        if cmd_direction.upper() not in ('N', 'S', 'E', 'O', 'Q'):
            raise InvalidCommands(cmd_direction.upper())
        if not cmd_steps.isdigit():
            raise InvalidCommands(cmd_direction.upper())

        # ------------------------
        # Calculate next position
        # ------------------------
        itinary = list()

        if cmd_direction == 'N':
            for i in range(cmd_steps):
                itinary.append(self.map[(self.robot_position[1]) - i][self.robot_position[0]])
            if 'X' in itinary:
                return ()

        elif cmd_direction == 'S':
            for i in range(cmd_steps):
                itinary.append(self.map[(self.robot_position[1]) + i][self.robot_position[0]])
            if 'X' in itinary:
                return ()

        elif cmd_direction == 'E':
            for i in range(cmd_steps):
                itinary.append(self.map[self.robot_position[1]][self.robot_position[0]] + i)
            if 'X' in itinary:
                return ()

        elif cmd_direction == 'O':
            for i in range(cmd_steps):
                itinary.append(self.map[self.robot_position[1]][self.robot_position[0] - i])
            if 'X' in itinary:
                return ()

    def calculate_coordinate(self, direction, step):
        """
        Calculate new coordinate to move the robot
        :param direction: direction to move to
        :param step: number of step to move
        :return: tuple of the new coordinate
        """

        if direction == 'N':
            return self.robot_position[0], self.robot_position[1] - step
        elif direction == 'S':
            return self.robot_position[0], self.robot_position[1] + step
        elif direction == 'E':
            return self.robot_position[0] + step, self.robot_position[1]
        elif direction == 'O':
            return self.robot_position[0] - step , self.robot_position[1]


