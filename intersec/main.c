#include "list.h"
#include "dict.h"

void all_tests(void);

int main(void){
    all_tests();
    return 0;
}

/* All code related to testing */

void all_tests(void){
    test_count_words_in_string();
    test_chained_list();
    test_parse_dict();
}
