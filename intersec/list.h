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

List *initialisation(void);
void insert(List* list, char* word_to_add, int size);
unsigned int count(Element* start);
Element* search_word(Element *node, char* word);
int word_occurences(Element* node, char* word);
void print_list(Element* start);
void liberate(List * list);
void test_chained_list(void);

