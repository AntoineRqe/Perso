# encoding utf-8

from maze import *
from errors import *
from maps import *
import glob


init_arguments = ("L", "S", "E")
command_arguments = ("N", "E", "S", "W", "Q")


def print_usage():
    """
    Usage for user
    """
    print "---------------------------------------------------------------------"
    print "Welcome in the maze game."
    print "With your robot, you have to exit the maze"
    print "On the map display"
    print "\tX : Wall"
    print "\tR : Robot"
    print "\tE : Entrance"
    print "\tU : Exit"
    print "You can move the robot using a direction(N,W,S,E) and number of steps"
    print "Example of use:"
    print "\tE5 : 5 steps to east direction"
    print "\tN3 : 3 steps to north direction"
    print "---------------------------------------------------------------------"


def print_init_usage():
    """
    Print all init option available
    """
    print "List of available init option"
    print "     {} : load a save game".format(init_arguments[0])
    print "     {} : Start a new game".format(init_arguments[1])
    print "     {} : Edit a new maze".format(init_arguments[2])


def print_cmd_usage():
    """
    Print all robot commands available
    """
    print "List of robot command"
    print "     {} : Go north".format(command_arguments[0])
    print "     {} : Go east".format(command_arguments[1])
    print "     {} : Go south".format(command_arguments[2])
    print "     {} : Go west".format(command_arguments[3])
    print "     {} : Save and quit".format(command_arguments[4])


def find_file_extension(file_extension):
    """
    Find all file with given extension
    :param file_extension: extension to look for
    :return: a list of all files found
    """
    return glob.glob('*.{}'.format(file_extension))


def ask_cmd():
    """
    Function to ask command to move the robot
    :return: string of given cmd
    """
    try:
        print "------------------------------------------"
        cmd = raw_input("So, where does the robot go?\r\n")
        cmd_direction = str(cmd[0]).upper()
        cmd_steps = str(cmd[1:])

        if cmd_direction == "Q":
            return cmd_direction, 0
        elif not cmd:
            raise EmptyOptions(cmd)
        elif cmd_direction not in command_arguments:
            raise InvalidCommands(cmd_direction)
        elif not cmd_steps.isdigit():
            raise InvalidCommands(cmd_direction)

    except InvalidCommands as e:
        print e
        print_cmd_usage()
        return "-1", -1

    except EmptyOptions as e:
        print e
        print_cmd_usage()
        return "-1", -1

    except IndexError as e:
        print_cmd_usage()
        return "-1", -1

    return cmd_direction, int(cmd_steps)


def load(save_file):
    """
    :param save_file: name of the file to load
    :return maze object loaded from saved file
    Load a previous saved game
    """

    if type(save_file) != str:
        print "Need to give a string as argument"
        return None

    if not os.path.isfile(save_file):
        print "{} doesn't exist, nothing to load".format(save_file)
        return None

    if os.stat(save_file).st_size == 0:
        print "No date to load in {}".format(save_file)
        return None

    with open(save_file, 'rb') as save:
        my_unpickler = Unpickler(save)
        my_maze = my_unpickler.load()

    return my_maze


def init():
    """
    Initialization to generate or load a given maze
    :return: return a maze
    """
    print "Time to start the game"
    print "You can : Start a predefined game(S)"
    print "          Load a game previously saved(L)"
    print "          Edit a new map(E)"
    try:
        options = raw_input("So how do you want to start?\r\n")
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
        print "Saves available : {}".format(find_file_extension("sav"))
        my_maze = None
        save = str(raw_input("Which file you want to load?\r\n"))
        my_maze = load(save)
        while my_maze is None:
            save = str(raw_input("Which file you want to load?\r\n"))
            my_maze = load(save)
        return my_maze

    elif options.upper() == init_arguments[1]:
        print "I want to start a game"
        print "Maps available are {}".format(map_catalog_str)
        name = str(raw_input("What map do you want?\r\n"))
        return Maze(name)

    elif options.upper() == init_arguments[2]:
        print "I want to edit a maze"


