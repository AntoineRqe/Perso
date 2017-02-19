# coding: utf8

from donnees import *
from random import *
import pickle
import os


def add_word_to_dictionary(word):
    """
    Function to add a word to the dictionary
    :param word: string of the new word
    """
    if word & str(word).isalpha():
        dictionnaires_pendu.append(str(word))


def save_data(player_stats):
    """
        Save data in a save file
        :param player_stats : dictionary of player stats
    """
    with open(saves_file_path, 'ba') as scores:
        my_pickler = pickle.Pickler(scores)
        my_pickler.dump(player_stats)


def load_data(player):
    """
    Load score of the player
    :param player: name of the player data to be restored
    :return: dictionary of the player score

    """
    if not os.path.isfile(saves_file_path):
        open(saves_file_path, 'a').close()
        print("File %s created" % saves_file_path)
        return 0

    if os.stat(saves_file_path).st_size == 0:
        print("Nothing to load in %s" % saves_file_path)
        return 0

    with open(saves_file_path, 'br') as scores:
        my_unpickler = pickle.Unpickler(scores)
        my_scores = dict(my_unpickler.load())
        for player_key in my_scores.keys():
            if player_key == player:
                return my_scores[player]
    return


# Functions to play the pendu


def choose_word_to_guess():
    """
    Function to randomly int choose the word to guess
    :return str : word to be guessed
    """
    word_index = randint(0, len(dictionnaires_pendu) - 1)
    return dictionnaires_pendu[word_index]


def create_stared_word(size):
    """
    Function to create a stared string of given size
    :param: integer, size of the string to create
    :return: the created string
    """
    star = str('*' * size)
    return star


def pick_letter():
    """
    Ask user a letter
    :return  letter from the player
    """
    letter = input("Which letter do you want to pick?\n")
    while len(letter) != 1:
        letter = input("Please, just ONE letter?")
    return letter


def is_letter_already_picked(letter, letters):
    """
    Check if letter have already been picked
    :param letter: letter from the player
    :param letters: list of already choosed letter
    :return: True if already chosen, False otherwise
    """
    if letter in letters:
        print("letter %c already picked!" % letter)
        return True
    else:
        letters.append(letter)
        return False


def is_letter_in_string(letter, string):
    """
    Check if the letter is in the word to guess.
    :param letter : char to scan
    :param string:  string to be scanned
    :return: tuple : position of the occurences of letter
    """
    occurrences = list()
    for i, occurrence in enumerate(string):
        if occurrence == letter:
            occurrences.append(i)
    return tuple(occurrences)


def update_stared_word(letter, occurrences, stared_string):
    """
    Update stared word with founded letter
    :param letter: letter to be revealed
    :param occurrences : position of the letter in the stared string
    :param stared_string: string to update with founded letter
    :return: updated stared string
    """
    star_list = list(stared_string)
    for index in occurrences:
        star_list[index] = letter
    return "".join(star_list)


def is_all_letter_found(started_string):
    """
    Check if player has guessed all letter
    :param started_string: the word to be founded
    :return: True is player found the word, False otherwise.
    """
    if '*' in started_string:
        return False
    return True

