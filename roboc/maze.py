# encoding utf-8

from custom_errors import EmptyOptions, InvalidCommands, EncounterObstacle, CoordinateOutOfRange
from pickle import Pickler
from os import path as op
from random import randrange
from collections import OrderedDict


def static_vars(**kwargs):
    """
    Decoration to implement static variable to a fu
    """
    def decorate(func):
        for k in kwargs:
            setattr(func, k, kwargs[k])
        return func
    return decorate


@static_vars(count=0)
def increment_number():
    """
    Define a increment number
    :return incremental number
    """
    increment_number.count += 1
    return str(increment_number.count)


def find_exit(maze):
    """
    Find 'U' in a list of string
    :param maze: the list of string to parse
    :return: a tuple with the coordinate of the entrance
    """
    for y, map_line in enumerate(maze):
        if 'U' in map_line:
            return str(map_line).index('U'), y


def find_entrance(maze):
    """
    Find 'X' in a list of string
    :param maze: the list of string to parse
    :return: a tuple with the coordinate of the entrance
    """
    for y, map_line in enumerate(maze):
        if 'X' in map_line:
            return str(map_line).index('X'), y


class Maze:

    def __init__(self, map_drawing=[], players=[]):

        if type(map_drawing) != list or len(map_drawing) <= 0:
            raise TypeError("Wrong type give")

        self.players = OrderedDict()
        self.current_player = ""
        self.map = list(map_drawing)
        self.clean_map = list(map_drawing)
        self.size = self.len()
        self.exit_position = find_exit(self.clean_map)
        self.robot_position = {}
        self.robot_commands = {
            "N": {
                "type": "move",
                "cmd": self.move,
                "desc": "Go to north"
            },
            "E": {
                "type": "move",
                "cmd": self.move,
                "desc": "Go to east"
            },
            "S": {
                "type": "move",
                "cmd": self.move,
                "desc": "Go to south"
            },
            "W": {
                "type": "move",
                "cmd": self.move,
                "desc": "Go to west"
            },
            "Q": {
                "type": "option",
                "cmd": self.save,
                "desc": "Save and quit the game"
            },
            "M": {
                "type": "transform",
                "cmd": self.put_wall,
                "desc": "Transform door into wall",
            },
            "P": {
                "type": "transform",
                "cmd": self.put_door,
                "desc": "Transform wall into door"
            }
        }

        # update player list with robot symbol
        for player in players:
            self.players[player] = increment_number()

        self.current_player = list(self.players.keys())[0]

        # Random position for robot
        for player in self.players.keys():
            self.update_robot_position(player, self.init_robot_position())
            print(" Test {} : {}".format(player, self.robot_position[player]))

    def __repr__(self):
        map_str = str()
        for map_line in self.map:
            map_str += map_line + "\r\n"
        return str(map_str)

    def len(self):
        """
        Function to get the dimension of the maze
        :return: the dimension of the maze
        """
        return len(self.clean_map[0]), len(self.clean_map)

    def init_robot_position(self):
        """
        Get a random position for robot
        :return: coordinate of initial position.
        """
        x, y = (-1, -1)
        while (x, y) == (-1, -1):
            x, y = randrange(0, self.size[0]), randrange(0, self.size[1])
            if self.map[y][x] != " " and self.map[y][x] != ".":
                x, y = (-1, -1)

        return x, y

    def print_cmd_usage(self):
        """
        Print all robot commands available
        """
        print("List of robot command : ")
        for key, body in self.robot_commands.items():
            print("%s : %s" % (key, body["desc"]))

    def update_robot_position(self, player, coordinate):
        """
        Update position of the robot into the maze
        :param player: name of the robot player
        :param coordinate: couple of coordinates
        """

        new_x = coordinate[0]
        new_y = coordinate[1]

        print("update_robot_position : {}".format(player))
        if player not in self.robot_position or self.robot_position[player] == ():
            self.robot_position[player] = (new_x, new_y)
        else:
            # -------------------------------
            # Delete previous robot position
            # -------------------------------
            map_list = list(self.map[self.robot_position[player][1]])
            map_list[self.robot_position[player][0]] = self.clean_map[self.robot_position[player][0]][1]
            self.map[self.robot_position[player][1]] = "".join(map_list)

        # -------------------------------
        # Update robot position
        # -------------------------------
        map_list = list(self.map[new_y])
        map_list[new_x] = self.players[player]
        self.map[new_y] = "".join(map_list)

        self.robot_position[player] = (new_x, new_y)
        self.get_next_player()

    def is_command_valid(self, cmd):
        """
        Check if the command has a valid format
        :param cmd : the given command to verify
        :return: True if command available, False otherwise
        """

        if type(cmd) != str or len(cmd) <= 0:
            return False
        elif cmd[0] not in self.robot_commands.keys():
            return False

        if self.robot_commands[cmd[0]]["type"] == "move":
            if len(cmd) == 1:
                return True
            elif cmd[1:].isdigit():
                return True
            else:
                return False

        elif self.robot_commands[cmd[0]]["type"] == "transform":
            if len(cmd) != 2:
                return False
            elif cmd[1] in ("N", "E", "S", "W"):
                return True
            else:
                return False

        elif self.robot_commands[cmd[0]]["type"] == "option":
            if len(cmd) == 1:
                return True
            else:
                return False

    def move(self, cmd):
        if len(cmd) == 1:
            step = 1
        else:
            step = int(cmd[1:])

        if not self.is_itinerary_clear(cmd[0], step):
            return

        x, y = self.calculate_coordinate(cmd[0], step)
        self.update_robot_position(self.current_player, (x, y))

    def put_wall(self, cmd):
        """
        Function to transform a door into a wall
        :param cmd: cmd given
        """

        x, y = self.calculate_coordinate(cmd[1], 1)

        if self.map[y][x] == ".":
            # update clean map
            map_list = list(self.clean_map[y])
            map_list[x] = 'O'
            self.clean_map[y] = "".join(map_list)

            # update current map
            map_list = list(self.map[y])
            map_list[x] = 'O'
            self.map[y] = "".join(map_list)

    def put_door(self, cmd):
        """
        Function to transform a wall into a door
        :param cmd: cmd given
        """

        x, y = self.calculate_coordinate(cmd[1], 1)

        if self.map[y][x] == "O":
            # update clean map
            map_list = list(self.clean_map[y])
            map_list[x] = '.'
            self.clean_map[y] = "".join(map_list)

            # update current map
            map_list = list(self.map[y])
            map_list[x] = '.'
            self.map[y] = "".join(map_list)

    def calculate_coordinate(self, direction, step):
        """
        Calculate new coordinate to move the robot
        :param direction: direction to move to
        :param step: number of step to move
        :return: tuple of the new coordinate, (-1, -1) if invalids coordinate
        """

        (x, y) = (-1, -1)
        if direction == "N":
            x, y = self.robot_position[self.current_player][0], self.robot_position[self.current_player][1] - step
        elif direction == "E":
            x, y = self.robot_position[self.current_player][0] + step, self.robot_position[self.current_player][1]
        elif direction == "S":
            x, y = self.robot_position[self.current_player][0], self.robot_position[self.current_player][1] + step
        elif direction == "W":
            x, y = self.robot_position[self.current_player][0] - step, self.robot_position[self.current_player][1]

        if x < 0 or y < 0 or x > self.len()[0] or y > self.len()[1]:
            raise CoordinateOutOfRange((x,y))

        return x, y

    def is_itinerary_clear(self, cmd_direction, cmd_steps):
        """
        Dry run the itinerary of robot and check if any obstacle
        :param cmd_direction: direction to follow
        :param cmd_steps: number of step in that direction
        :return: True if path is clean, False if and obstacles found
        """

        if type(cmd_direction) != str or type(cmd_steps) != int or cmd_steps < 0 or len(cmd_direction) != 1:
            return False

        try:
            self.calculate_coordinate(cmd_direction, cmd_steps)
        except CoordinateOutOfRange as e:
            print(e)
            return False

        itinerary = list()

        try:
            if cmd_direction == 'N':
                for i in range(0, cmd_steps + 1):
                    itinerary.append(self.map[(self.robot_position[self.current_player][1]) - i][self.robot_position[self.current_player][0]])
                if 'O' in itinerary:
                    raise EncounterObstacle(itinerary)

            elif cmd_direction == 'S':
                for i in range(0, cmd_steps + 1):
                    itinerary.append(self.map[(self.robot_position[self.current_player][1]) + i][self.robot_position[self.current_player][0]])
                if 'O' in itinerary:
                    raise EncounterObstacle(itinerary)

            elif cmd_direction == 'E':
                for i in range(0, cmd_steps + 1):
                    itinerary.append(self.map[self.robot_position[self.current_player][1]][self.robot_position[self.current_player][0] + i])
                if 'O' in itinerary:
                    raise EncounterObstacle(itinerary)

            elif cmd_direction == 'W':
                for i in range(0, cmd_steps + 1):
                    itinerary.append(self.map[self.robot_position[self.current_player][1]][self.robot_position[self.current_player][0] - i])
                if 'O' in itinerary:
                    raise EncounterObstacle(itinerary)

        except EncounterObstacle as e:
            print(e)
            return False

        except IndexError as e:
            print("You have encounter an obstacle with move {}{}".format(cmd_direction, cmd_steps))
            return False

        return True

    def is_maze_resolved(self):
        """
        Check if Robot found the exit
        :return: True if exit found, False otherwise
        """
        if self.robot_position == self.exit_position:
            return True
        else:
            return False

    def save(self, *args):
        """
        Save the game in binary file name.sav and quit the game
        """
        confirm = True
        save_name = str(input("Name of the save?\n\r"))
        print("File to save {}".format(save_name))

        if op.isfile(save_name):
            ret = str(input("Are you sure you want to erase save {}? (Y/N)\r\n".format(save_name)))
            while ret.upper() not in ("Y", "N"):
                ret = str(input("Are you sure you want to erase save {}? (Y/N)\r\n".format(save_name)))
            if ret.upper() == "N":
                confirm = False
            else:
                confirm = True

        if confirm:
            with open(save_name, 'wb') as save:
                my_pickler = Pickler(save)
                my_pickler.dump(self)
            print("File {} saved".format(save_name))
        exit()

    def get_next_player(self):
        """
        Iterative loop the the player
        :return: index to next player
        """
        player_list = list(self.players.keys())
        current_index = player_list.index(self.current_player)
        if current_index < len(player_list)-1:
            self.current_player = player_list[current_index+1]
        else:
            self.current_player = player_list[0]
