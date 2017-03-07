# encoding utf-8

from toolbox import *
from maps import *
from maze import *


def init():
    """
    Initialization to generate or load a given maze
    :return: return a maze
    """

    # Load all maps available before starting.
    labyrinths = Maps()
    labyrinths.load_map(os.path.join(os.getcwd(), "Maps"))

    print("Time to start the game")
    print("You can : Start a predefined game(S)")
    print("          Load a game previously saved(L)")
    print("          Edit a new map(E)")
    try:
        options = input("So how do you want to start?\r\n")
        if options.isdigit():
            raise NumericalOptions(options)
        if not options:
            raise EmptyOptions(options)
        if len(options) > 1:
            raise OverSizedOptions(options)
        if options.upper() not in init_arguments:
            raise WrongOptions(options)

    except NumericalOptions as e:
        print(e)
        print_init_usage()

    except EmptyOptions as e:
        print(e)
        print_init_usage()

    except OverSizedOptions as e:
        print (e)
        print_init_usage()

    except WrongOptions as e:
        print (e)
        print_init_usage()

    # Load a previous saved game
    if options.upper() == init_arguments[0]:
        print("Saves available : {}".format(find_file_extension(os.path.join(os.getcwd()), "sav")))
        my_maze = None
        save = str(input("Which file you want to load?\r\n"))
        my_maze = load(save)
        while my_maze is None:
            save = str(input("Which file you want to load?\r\n"))
            my_maze = load(save)
        return my_maze

    # Start a new game
    elif options.upper() == init_arguments[1]:
        print("I want to start a game")
        print("Maps available are :".format(labyrinths.names))

        for index, name in enumerate(labyrinths.names):
            print("\t {} - {}".format(index, name))

        try:
            your_choice = int(input("What map do you want?\r\n"))
        except ValueError:
            your_choice = ""
            pass

        while type(your_choice) != int or your_choice < 0 or your_choice >= len(labyrinths.names):
            your_choice = int(input("What map do you want?\r\n"))

        return Maze(labyrinths.drawings[labyrinths.names[your_choice]])

    # Edit a new map
    elif options.upper() == init_arguments[2]:
        new_maze = list()
        new_line = str()
        i = 0
        confirm = True
        has_entrance = 0
        has_exit = 0

        new_name = str(input("Name your new maze?\r\n"))
        print("To edit {}, please enter line by line your maze, the press Q when over".format(new_name))
        print("Make sure you have one entrance (E) and one Exit (U)")
        while new_line != "Q":
            new_line = str(input("Enter line {}\r\n".format(i)))
            if "R" in new_line:
                has_entrance += 1
            if "U" in new_line:
                has_exit += 1
            if new_line.upper() == "Q":
                break
            new_maze.append(new_line)
            i += 1

        if has_entrance != 1 or has_exit != 1:
            print("You can only have one entrance {} and one exit {}, quit program".format(has_entrance, has_exit))
            exit()

        file_name = "{}.map".format(new_name)
        print("Map to save {}".format(file_name))

        if os.path.isfile(file_name):
            ret = str(input("Are you sure you want to erase map {}? (Y/N)\r\n".format(file_name)))
            while ret.upper() not in ("Y", "N"):
                ret = str(input("Are you sure you want to erase save {}? (Y/N)\r\n".format(file_name)))
            if ret.upper() == "N":
                confirm = False
            else:
                confirm = True

        if confirm:
            with open(file_name, 'wb') as save:
                my_pickler = Pickler(save)
                my_pickler.dump({new_name:new_maze})
            print("Map {} saved".format(file_name))

        exit()


def main():
    print_usage()
    my_maze = init()
    while not my_maze:
        my_maze = init()

    print(my_maze)

    while True:
        (direction, step) = my_maze.parse_command()
        while (direction, step) == (-1, -1):
            (direction, step) = my_maze.parse_command()

        my_maze.update_robot_position(direction, step)
        print(my_maze)
        if my_maze.is_maze_resolved():
            print("Bravo, You exited the maze!")
            break

if __name__ == "__main__":
    main()
