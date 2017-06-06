#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char* sentences[100] = {
	"je m appelle antoine",		// 4 words
	"Je m appelle Antoine.",	// 4 words
	" ",						// 0 word 
	" Antoine ",				// 1 word
	"",							// 0 word
	"!"							// 0 word
};

/* Return the number of words in a given sentence */
unsigned int count_words_in_string(const char* sentence, const char delimiter){
	unsigned int words_counter = 0;
	char* current_ptr = NULL;
	char* old_ptr = NULL;
	
	if(sentence == NULL){
		return 0;
	}
	
	while((current_ptr = strchr(sentence, delimiter)) != NULL){
		if(current_ptr == sentence){
			old_ptr = current_ptr;
			current_ptr++;
			continue;
		}
		words_counter++;
		current_ptr++;
		old_ptr = current_ptr;
	}

	if(old_ptr != current_ptr){
		words_counter++;
	}

	return words_counter;
}

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
}

void all_tests(void){
	test_count_words_in_string();
}

int main(void){
	all_tests();
	return 0;
}
