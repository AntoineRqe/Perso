#define MAX_WORD_SIZE 50

unsigned int count_words_in_string(char* sentence, const char delimiter, List *list);
unsigned int parse_dict(char* dict_name, List* word_list);
void test_count_words_in_string(void);
void test_parse_dict(void);
