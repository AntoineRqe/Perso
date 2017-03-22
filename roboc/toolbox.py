# encoding utf-8

from pickle import Unpickler
import os

save_path = os.path.join(os.getcwd(), "Saves")


def print_usage():
    """
    Usage for user
    """
    print("---------------------------------------------------------------------")
    print("Welcome in the maze game.")
    print("With your robot, you have to exit the maze")
    print("On the map display")
    print("\tO : Wall")
    print("\tX : Robot")
    print("\t. : Door")
    print("\tU : Exit")
    print("You can move the robot using a direction(N,W,S,E) and number of steps")
    print("Example of use:")
    print("\tE5 : 5 steps to east direction")
    print("\tN3 : 3 steps to north direction")
    print("---------------------------------------------------------------------")


def print_init_usage(game_options):
    """
    Print all init option available
    :param : game_options : dictionary of game option
    """
    print("List of available init option: ")
    for name, interface in game_options.items():
        print("    {} : {}".format(name, interface["desc"]))


def find_file_extension(file_path, file_extension):
    """
    Find all file with given extension
    :param file_path: path to folder to look for
    :param file_extension: extension to look for
    :return: a list of all files found
    """

    if type(file_path) != str or type(file_extension) != str:
        return None

    files = [f for f in os.listdir(file_path) if os.path.isfile(os.path.join(file_path, f))]
    final_file = list(files)

    for f in files:
        extension = f.split(".")[-1]
        if extension != file_extension:
            final_file.remove(f)

    return final_file


def add_file_extension(name, extension):
    """
    Add an extension to a name file
    :param name: basic name of the file
    :param extension: to be added at end of name
    :return: full name with extension
    """

    if type(name) != str or type(extension) != str:
        return None
    elif len(name) <= 0 or len(extension) <= 0:
        return None

    full_name = "{}.{}".format(name, extension)

    return full_name


def remove_file_extension(full_name):
    """
    Remove extension to full name
    :param full_name: full name of the file to be castrated
    :return: name with no extension
    """
    if type(full_name) != str:
        return None
    elif len(full_name) <= 0:
        return None
    elif "." not in full_name:
        return None

    basename = str(full_name).split(".")[0]

    return basename


def load(full_path_save_file):
    """
    :param full_path_save_file: full path of the name of the file to load
    :return maze object loaded from saved file
    Load a previous saved game
    """

    if type(full_path_save_file) != str:
        print("Need to give a string as argument")
        return None

    if not os.path.isfile(full_path_save_file):
        print("{} doesn't exist, nothing to load".format(full_path_save_file))
        return None

    if os.stat(full_path_save_file).st_size == 0:
        print("No date to load in {}".format(full_path_save_file))
        return None

    with open(full_path_save_file, 'rb') as save:
        my_unpickler = Unpickler(save)
        my_maze = my_unpickler.load()

    print("{} has been loaded".format(full_path_save_file))

    return my_maze



