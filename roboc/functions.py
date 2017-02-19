# encoding utf-8


def find_entrance(map):
    """
    Find 'E' in a list of string
    :param map: the list of string to parse
    :return: a tuple with the coordinate of the entrance
    """
    for y, map_line in enumerate(map):
        if 'E' in map_line:
            return str(map_line).index('E'), y


def find_exit(map):
    """
    Find 'U' in a list of string
    :param map: the list of string to parse
    :return: a tuple with the coordinate of the entrance
    """
    for y, map_line in enumerate(map):
        if 'U' in map_line:
            return str(map_line).index('U'), y


def ask_cmd():
    """
    Function to ask command to move the robot
    :return: string of given cmd
    """
    cmd = input("How do you want to move the robot?\r\n")
    return str(cmd).upper()