# encoding utf-8

from toolbox import *
from maze import *


def main():
    print_usage()
    my_maze = init()
    while not my_maze:
        my_maze = init()

    print my_maze

    while True:
        (direction, step) = ask_cmd()
        while direction == "-1" or direction == -1 or not my_maze.parse_command(direction, step):
            (direction, step) = ask_cmd()

        my_maze.update_robot_position(direction, step)
        print my_maze
        if my_maze.is_maze_resolved():
            print "Bravo, You exited the maze!"
            break

if __name__ == "__main__":
    main()
