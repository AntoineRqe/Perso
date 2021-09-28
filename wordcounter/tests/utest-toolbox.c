#include <check.h>
#include <math.h>

#include "toolbox.h"

static char * test_sentences[] = {
    "je m appelle antoine",     // 4 words
    "Je m appelle Antoine.",    // 4 words
    " ",                        // 0 word
    " Antoine ",                // 1 word
    "",                         // 0 word
    "jemappelleantoine",        // 1 word
    "a",                        // 1 word
    "! an be",                  // 2 words
    "this reaction focused as much on the development of new formal structures and syntheses as", //15 words
    NULL                        // 0 word
};


START_TEST(ParseDictionary)
{
    List* test_list = init();

    ck_assert_int_eq(parse_dict("./ressources/test_dict.txt", test_list), -1);
    ck_assert_int_eq(parse_dict("./ressources/dict.txt", NULL), -1);
    ck_assert_int_eq(parse_dict(NULL, test_list), -1);
    ck_assert_int_eq(parse_dict("./ressources/empty.txt", test_list), 0);
    ck_assert_int_eq(parse_dict("./ressources/space.txt", test_list), 0);
    ck_assert_int_eq(parse_dict("./ressources/comment.txt", test_list), 0);
    ck_assert_int_eq(parse_dict("./ressources/dict.txt", test_list), 3);

    ck_assert_str_eq(test_list->head->word, "test");
    ck_assert_str_eq(test_list->head->next->word, "aurevoir");
    ck_assert_str_eq(test_list->head->next->next->word, "bonjour");
    ck_assert_ptr_eq(test_list->head->next->next->next, NULL);

    fini(test_list);

}
END_TEST

START_TEST(ParseText)
{
    List* test_list = init();

    ck_assert_int_eq(parse_text("./ressources/empty.txt", test_list), 0);
    ck_assert_int_eq(parse_text("./ressources/comment.txt", test_list), 4);
    fini(test_list);
    test_list = init();
    ck_assert_int_eq(parse_text("./ressources/space.txt", test_list), 0);
    ck_assert_int_eq(parse_text("./ressources/dict.txt", test_list), 8);
    fini(test_list);
    test_list = init();
    ck_assert_int_eq(parse_text("./ressources/poetry.txt", test_list), 52);

    ck_assert_str_eq(test_list->head->word, "today.");
    ck_assert_str_eq(test_list->head->next->word, "for");
    ck_assert_str_eq(test_list->head->next->next->word, "live");
    ck_assert_str_eq(test_list->head->next->next->next->word, "love,");

    fini(test_list);

}
END_TEST

START_TEST(WordCounter)
{
    List* test_list = init();

    ck_assert_int_eq(count_words_in_string(test_sentences[0], ' ', test_list), 4);
    ck_assert_int_eq(count_words_in_string(test_sentences[1], ' ', test_list), 4);
    ck_assert_int_eq(count_words_in_string(test_sentences[2], ' ', test_list), 0);
    ck_assert_int_eq(count_words_in_string(test_sentences[3], ' ', test_list), 1);
    ck_assert_int_eq(count_words_in_string(test_sentences[4], ' ', test_list), 0);
    ck_assert_int_eq(count_words_in_string(test_sentences[5], ' ', test_list), 1);
    ck_assert_int_eq(count_words_in_string(test_sentences[6], ' ', test_list), 1);
    ck_assert_int_eq(count_words_in_string(test_sentences[7], ' ', test_list), 2);
    ck_assert_int_eq(count_words_in_string(test_sentences[8], ' ', test_list), 15);
    ck_assert_int_eq(count_words_in_string(test_sentences[9], ' ', test_list), 0);

    ck_assert_int_eq(count_items(test_list->head), 28);

    fini(test_list);
}
END_TEST

/* ------------------------------------------------------------------------*/
/* TESTS */

Suite *create_suite(void)
{
    Suite *s = suite_create("Toolbox");

    TCase *tc_core = tcase_create("Core");

    tcase_add_test(tc_core, ParseDictionary);
    tcase_add_test(tc_core, ParseText);
    tcase_add_test(tc_core, WordCounter);

    suite_add_tcase(s, tc_core);

    return s;
}

int main(void)
{
    int num_fail = -1;

    Suite *  s = create_suite();
    SRunner *sr = srunner_create(s);
    srunner_set_fork_status(sr, CK_NOFORK);
    srunner_run_all(sr, CK_NORMAL);
    num_fail = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (num_fail == 0) ? 0 : -1;
}
