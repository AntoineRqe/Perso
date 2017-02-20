# encoding utf-8

from maze import *
from errors import *
from maps import *


init_arguments = ("L", "I", "E")


def find_entrance(maze):
    """
    Find 'E' in a list of string
    :param maze: the list of string to parse
    :return: a tuple with the coordinate of the entrance
    """
    for y, map_line in enumerate(maze):
        if 'E' in map_line:
            return str(map_line).index('E'), y
    return -1


def find_exit(maze):
    """
    Find 'U' in a list of string
    :param maze: the list of string to parse
    :return: a tuple with the coordinate of the entrance
    """
    for y, map_line in enumerate(maze):
        if 'U' in map_line:
            return str(map_line).index('U'), y
    return -1


def ask_cmd():
    """
    Function to ask command to move the robot
    :return: string of given cmd
    """
    cmd = input("How do you want to move the robot?\r\n")
    return str(cmd).upper()


def init():
    """
    Initialization to generate or load a given maze
    :return: return a maze
    """
    try:
        options = raw_input("What maze do you want to do?\r\n")
        if options.isdigit():
            raise NumericalOptions(options)
        if not options:
            raise EmptyOptions(options)
        if len(options) > 1:
            raise OverSizedOptions(options)

    except NumericalOptions as e:
        print e.message
        return
    except EmptyOptions as e:
        print e.message
        return
    except OverSizedOptions as e:
        print e.message
        return

    try:
        if options.upper() not in init_arguments:
            raise WrongOptions()
            return
        elif options.upper() == "L":
            print "I want to load a game"
        elif options.upper() == "I":
            print "I want to start a random game"
            return Maze("test", "easy", easy_map)
        elif options.upper() == "E":
            print "I want to edit a maze"
        else:
            raise WrongOptions()

    except WrongOptions as e:
        print e.message
        return
