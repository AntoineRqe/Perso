#ifndef LIST_H
#define LIST_H
/* Functions and structures related to chained list */

typedef struct Element Element;
typedef struct List List;

struct Element{
    char* word;
    struct Element* next;
};

struct List{
    struct Element* head;
};

/**
 * \fn List* init(void);
 * \brief Initialize a list of words.
 * \param list pointer to the top of the list.
 *
 * \return pointer to the top of the list, NULL otherwise.
 */
List* init(void);

/**
 * \fn void fini(List * list);
 * \brief Clear a previously initialized list.
 * \param list pointer to the list to clear.
 *
 */
void fini(List * list);

/**
 * \fn int insert(List * list, const char * new_word);
 * \brief Insert a new word to the top of the list.
 * \param list pointer to the top of the list.
 * \param new_word string to store in the list.
 *
 * \return 1 if sucess, -1 otherwise.
 */
int insert(List * list, const char * new_word);

/**
 * \fn unsigned int count(Element* head);
 * \brief Insert a new word to the top of the list.
 * \param head top of the list.
 *
 * \return total number of word found in the list.
 */
unsigned int count_items(Element* head);

/**
 * \fn int count_occurences(Element* node, const char* word)
 * \brief Count occurences of a word in the list
 * \param head top of the list.
 * \param word string to look for.
 *
 * \return Number of occurences for word in the list.
 */
int count_occurences(Element* head, const char* word);

void compare_list(Element* dict_list, Element* text_list);

#endif //LIST_H
