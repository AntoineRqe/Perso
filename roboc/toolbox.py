# encoding utf-8

from custom_errors import *
from pickle import *
import os

init_arguments = ("L", "S", "E")
command_arguments = ("N", "E", "S", "W", "Q")


def print_usage():
    """
    Usage for user
    """
    print("---------------------------------------------------------------------")
    print("Welcome in the maze game.")
    print("With your robot, you have to exit the maze")
    print("On the map display")
    print("\tX : Wall")
    print("\tR : Robot")
    print("\tE : Entrance")
    print("\tU : Exit")
    print("You can move the robot using a direction(N,W,S,E) and number of steps")
    print("Example of use:")
    print("\tE5 : 5 steps to east direction")
    print("\tN3 : 3 steps to north direction")
    print("---------------------------------------------------------------------")


def print_init_usage():
    """
    Print all init option available
    """
    print("List of available init option")
    print("     {} : load a save game".format(init_arguments[0]))
    print("     {} : Start a new game".format(init_arguments[1]))
    print("     {} : Edit a new maze".format(init_arguments[2]))


def print_cmd_usage():
    """
    Print all robot commands available
    """
    print("List of robot command")
    print("     {} : Go north".format(command_arguments[0]))
    print("     {} : Go east".format(command_arguments[1]))
    print("     {} : Go south".format(command_arguments[2]))
    print("     {} : Go west".format(command_arguments[3]))
    print("     {} : Save and quit".format(command_arguments[4]))


def find_file_extension(file_path, file_extension):
    """
    Find all file with given extension
    :param file_path: path to folder to look for
    :param file_extension: extension to look for
    :return: a list of all files found
    """

    maps_files = [f for f in os.listdir(file_path) if os.path.isfile(os.path.join(file_path, f))]
    for f in maps_files:
        extension = f.split(".")[-1]
        if extension != file_extension:
            maps_files.remove(f)

    return maps_files


def ask_cmd():
    """
    Function to ask command to move the robot
    :return: string of given cmd
    """
    try:
        print("------------------------------------------")
        cmd = input("So, where does the robot go?\r\n")

        if len(cmd) == 1 and cmd.upper() in command_arguments:
            return cmd.upper(), 1

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
        print(e)
        print_cmd_usage()
        return "-1", -1

    except EmptyOptions as e:
        print(e)
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
        print("Need to give a string as argument")
        return None

    if not os.path.isfile(save_file):
        print("{} doesn't exist, nothing to load".format(save_file))
        return None

    if os.stat(save_file).st_size == 0:
        print("No date to load in {}".format(save_file))
        return None

    with open(save_file, 'rb') as save:
        my_unpickler = Unpickler(save)
        my_maze = my_unpickler.load()

    return my_maze
