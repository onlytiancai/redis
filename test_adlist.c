#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>
#include "adlist.h"

static void test_list_create_release(void **state) {
    list *l = listCreate();
    listRelease(l);
}

static void test_list_length(void **state) {
    list *l = listCreate();
    int i = 1, *p = &i;
    listAddNodeHead(l, p);
    listAddNodeHead(l, p);
    assert_int_equal(listLength(l), 2);
    listRelease(l);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_list_create_release),
        cmocka_unit_test(test_list_length),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}

