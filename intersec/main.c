#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void all_tests(void);

/* Return the number of words in a given sentence */
unsigned int count_words_in_string(char* sentence, const char delimiter){
    unsigned int words_counter = 0;
    char* current_ptr = sentence;
    char* old_ptr = sentence;

    if(current_ptr == NULL){
        return 0;
    }

    char* end_ptr = sentence + strlen(sentence);
    if ((end_ptr - current_ptr) == 1 && *current_ptr != delimiter) {
        return 1;
    }

    while((current_ptr = strchr(current_ptr, delimiter)) != NULL){
        if((current_ptr-old_ptr) <= 1){ //Two consecutive delimiter
            old_ptr = current_ptr;
            current_ptr++;
            continue;
        }
        words_counter++;
        old_ptr = current_ptr;
        current_ptr++;
    }

    if((end_ptr-old_ptr) > 1){
        words_counter++;
    }
    //printf("There are %d words in sentence '%s'\n", words_counter, sentence);
    return words_counter;
}



int main(void){
	all_tests();
	return 0;
}

/* All code related to testing */

char* sentences[100] = {
	"je m appelle antoine",		// 4 words
	"Je m appelle Antoine.",	// 4 words
	" ",						// 0 word
	" Antoine ",				// 1 word
	"",							// 0 word
	"jemappelleantoine",		// 1 word
    "a",                        // 1 word
    NULL                        // 0 word
};

void test_count_words_in_string(void){
	unsigned int test_counter = 0;
	test_counter = count_words_in_string(sentences[0], ' ');
	if(test_counter != 4){
		printf("[%s][%d] KO\n", __FUNCTION__, __LINE__);
	} else {
		printf("[%s][%d] OK\n", __FUNCTION__, __LINE__);
	}

	test_counter = count_words_in_string(sentences[1], ' ');
	if(test_counter != 4){
		printf("[%s][%d] KO\n", __FUNCTION__, __LINE__);
	} else {
		printf("[%s][%d] OK\n", __FUNCTION__, __LINE__);
	}

	test_counter = count_words_in_string(sentences[2], ' ');
	if(test_counter != 0){
		printf("[%s][%d] KO\n", __FUNCTION__, __LINE__);
	} else {
		printf("[%s][%d] OK\n", __FUNCTION__, __LINE__);
	}

    test_counter = count_words_in_string(sentences[3], ' ');
	if(test_counter != 1){
		printf("[%s][%d] KO\n", __FUNCTION__, __LINE__);
	} else {
		printf("[%s][%d] OK\n", __FUNCTION__, __LINE__);
	}

    test_counter = count_words_in_string(sentences[4], ' ');
	if(test_counter != 0){
		printf("[%s][%d] KO\n", __FUNCTION__, __LINE__);
	} else {
		printf("[%s][%d] OK\n", __FUNCTION__, __LINE__);
	}

    test_counter = count_words_in_string(sentences[5], ' ');
	if(test_counter != 1){
		printf("[%s][%d] KO\n", __FUNCTION__, __LINE__);
	} else {
		printf("[%s][%d] OK\n", __FUNCTION__, __LINE__);
	}

    test_counter = count_words_in_string(sentences[6], ' ');
	if(test_counter != 1){
		printf("[%s][%d] KO\n", __FUNCTION__, __LINE__);
	} else {
		printf("[%s][%d] OK\n", __FUNCTION__, __LINE__);
	}

    test_counter = count_words_in_string(sentences[7], ' ');
    if(test_counter != 0){
        printf("[%s][%d] KO\n", __FUNCTION__, __LINE__);
    } else {
        printf("[%s][%d] OK\n", __FUNCTION__, __LINE__);
    }
}

void all_tests(void){
	test_count_words_in_string();
}
