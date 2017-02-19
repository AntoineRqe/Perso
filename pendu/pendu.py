# coding: utf8

from function import *


def main():

    player_name = str(input("Please Enter your name?\n")).lower()

    while True:

        player_score = load_data(player_name)
        tries_number = max_tries
        word_to_guess = choose_word_to_guess().lower()
        print("Word to guess %s" % word_to_guess)
        star_word = create_stared_word(len(word_to_guess))
        letters_list = list()

        while tries_number > 0:
            print("-----------------------------------------------------------")
            print("you still have %d tries" % tries_number)
            print("you have already pick letters (%s)" % ",".join(map(str, letters_list)))
            current_letter = str(pick_letter()).lower()
            while is_letter_already_picked(current_letter, letters_list):
                current_letter = str(pick_letter()).lower()

            occurences = is_letter_in_string(current_letter, word_to_guess)
            if occurences:
                star_word = update_stared_word(current_letter, occurences, star_word)
                print("Word to guess %s" % star_word)
                if is_all_letter_found(star_word):
                    player_score += tries_number
                    print({player_name: player_score})
                    save_data({player_name: player_score})
                    break
            else:
                tries_number -= 1
                if tries_number <= 0 :
                    print("You don't have life anymore, let's start a new word")
                    break

if __name__ == "__main__" :
    main()

