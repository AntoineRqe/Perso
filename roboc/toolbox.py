# encoding utf-8

from maze import *
from errors import *
from maps import *


init_arguments = ("L", "S", "E")


def print_init_usage():
    """
    Print all init option available
    :return:
    """
    print "List of available init option"
    print "     {} : load a save game".format(init_arguments[0])
    print "     {} : Start a new game".format(init_arguments[1])
    print "     {} : Edit a new maze".format(init_arguments[2])


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
        if options.upper() not in init_arguments:
            raise WrongOptions(options)

    except NumericalOptions as e:
        print e
        print_init_usage()
    except EmptyOptions as e:
        print e
        print_init_usage()
    except OverSizedOptions as e:
        print e
        print_init_usage()
    except WrongOptions as e:
        print e
        print_init_usage()

    if options.upper() == init_arguments[0]:
        print "I want to load a game"
    elif options.upper() == init_arguments[1]:
        print "I want to start a random game"
        return Maze("test", "easy", easy_map)
    elif options.upper() == init_arguments[2]:
        print "I want to edit a maze"


