#include <check.h>
#include <math.h>

#include "list.h"

START_TEST(CheckList)
{
    List* top = init();

    ck_assert_ptr_eq(top->head, NULL);
    ck_assert_int_eq(count_items(NULL), 0);
    ck_assert_int_eq(count_items(top->head), 0);

    insert(top, "coucou!");
    insert(top, "coucou!");
    insert(top, "coucou!");
    insert(top, "coucou!");
    insert(top, "bonjour!");
    insert(top, "aurevoir!");

    ck_assert_int_eq(count_items(top->head), 6);
    ck_assert_int_eq(count_occurences(NULL, "coucou!"), -1);
    ck_assert_int_eq(count_occurences(top->head, NULL), -1);
    ck_assert_int_eq(count_occurences(top->head, "coucou!"), 4);
    ck_assert_int_eq(count_occurences(top->head, "bonjour!"), 1);
    ck_assert_int_eq(count_occurences(top->head, "aurevoir!"), 1);
    ck_assert_int_eq(count_occurences(top->head, "!"), 0);
    ck_assert_int_eq(count_occurences(top->head, "couc!"), 0);

    fini(top);

    top = init();
    ck_assert_int_eq(count_occurences(top->head, "couc!"), -1);
    fini(top);

}
END_TEST

/* ------------------------------------------------------------------------*/
/* TESTS */

Suite *create_suite(void)
{
    Suite *s = suite_create("List");

    TCase *tc_core = tcase_create("Core");

    tcase_add_test(tc_core, CheckList);

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
