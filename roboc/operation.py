# encoding utf-8

from toolbox import find_file_extension
from pickle import Pickler


def load_game(*args, **kwargs):
    print("Saves available : {}".format(find_file_extension(os.path.join(os.getcwd()), "sav")))
    my_maze = None
    save = str(input("Which file you want to load?\r\n"))
    my_maze = load(save)
    while my_maze is None:
        save = str(input("Which file you want to load?\r\n"))
        my_maze = load(save)
    return my_maze


def start_game(labyrinths, players=[],  *args, **kwargs):
    print("I want to start a game")
    print("Maps available are :".format(labyrinths.names))

    for index, name in enumerate(labyrinths.names):
        print("\t {} - {}".format(index, name))

    # try:
    #     your_choice = int(input("What map do you want?\r\n"))
    # except ValueError:
    #     your_choice = ""
    #     pass
    #
    # while type(your_choice) != int or your_choice < 0 or your_choice >= len(labyrinths.names):
    #     your_choice = int(input("What map do you want?\r\n"))
    your_choice = 0
    return labyrinths.drawings[labyrinths.names[your_choice]]


def edit_map(*args, **kwargs):
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

    file_name = "{}.text".format(new_name)
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
            my_pickler.dump({new_name: new_maze})
        print("Map {} saved".format(file_name))

    exit()