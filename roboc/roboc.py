# encoding utf-8

from custom_errors import NumericalOptions, EmptyOptions, OverSizedOptions, WrongOptions, InvalidCommands
from toolbox import print_init_usage, print_usage
from maps import Maps
from maze import Maze
from interface import game_options
import os


def init(tries=5):
    """
    Initialization to generate or load a given maze
    :return: return a maze
    """

    players = list()
    players.append("Antoine")
    players.append("Remi")
    print(players)

    if tries == 0:
        print("No more tries, exit the program!")
        exit(0)

    # Load all maps available before starting.
    labyrinths = Maps()
    labyrinths.load_map(os.path.join(os.getcwd(), "Maps"))

    print_init_usage(game_options)

    # try:
    #     options = str(input("So how do you want to start?\r\n")).upper()
    #     if options.isdigit():
    #         raise NumericalOptions(options)
    #     if not options:
    #         raise EmptyOptions(options)
    #     if len(options) > 1:
    #         raise OverSizedOptions(options)
    #     if options not in game_options.keys():
    #         raise WrongOptions(options)
    #
    # except Exception as e:
    #     print(e)
    #     print_init_usage(game_options)
    #     return init(tries=tries - 1)

    options = "S"
    if options == "S":
        print("Test : {}".format(players))
        return Maze(game_options[options]["cmd"](labyrinths), players)
    else:
        return game_options[options]["cmd"](labyrinths)


def main():
    print_usage()
    my_maze = init()
    while not my_maze:
        my_maze = init()

    print(my_maze)

    while True:
        is_valid_command = False
        while not is_valid_command:
            print("------------------------------------------")
            my_maze.print_cmd_usage()
            cmd = str(input("{}, what does the robot do?\r\n"
                            .format(my_maze.players[my_maze.current_player]))).upper()
            is_valid_command = my_maze.is_command_valid(cmd)

        my_maze.robot_commands[cmd[0]]["cmd"](cmd)
        print(my_maze)
        if my_maze.is_maze_resolved():
            print("Bravo, You exited the maze!")
            break

if __name__ == "__main__":
    main()
