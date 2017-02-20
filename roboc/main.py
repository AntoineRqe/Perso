# encoding utf-8

from toolbox import *
from maze import *


def main():

    my_maze = init()
    while not my_maze:
        my_maze = init()

    print my_maze

    while True:
        return


    # my_maze = Maze("test", "easy", easy_map)
    # print "Maze of dimension {}\r\n{}".format(my_maze.len(), my_maze)
    # my_maze.update_robot_position(3, 3)
    # print "Maze of dimension {}\r\n{}".format(my_maze.len(), my_maze)
    # my_maze.update_robot_position(4, 4)
    # print "Maze of dimension {}\r\n{}".format(my_maze.len(), my_maze)
    # my_maze.update_robot_position(5, 6)
    # print "Maze of dimension {}\r\n{}".format(my_maze.len(), my_maze)


if __name__ == "__main__":
    main()