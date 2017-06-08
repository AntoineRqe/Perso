#define MAX_WORD_SIZE 50
#define MAX_SENTENCE_SIZE 500

unsigned int count_words_in_string(char* sentence, const char delimiter, List *list);
unsigned int parse_dict(char* dict_name, List* word_list);
unsigned int parse_text(char* text_name, List* word_list);

/* --------------------------------------------------------------------------------
 *                                      Tests
 * ------------------------------------------------------------------------------ */
void test_count_words_in_string(void);
void test_parse_dict(void);
