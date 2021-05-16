#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "adlist.h"

static int mock_malloc = 0;

void *__wrap_zmalloc(size_t size);
void *__real_zmalloc(size_t size);
void *__wrap_zmalloc(size_t size) {
    if (mock_malloc) {
        return (void*) mock();
    } else {
        return __real_zmalloc(size);
    }
}

static void test_list_create_fail(void **state) {
    mock_malloc = 1;
    will_return(__wrap_zmalloc, NULL);
    list *l = listCreate();
    assert_null(l);
    mock_malloc = 0;
}

static void test_list_add_fail(void **state) {
    int a = 1;
    list *l = listCreate(), *l2 = NULL, *l3 = NULL;

    mock_malloc = 1;

    will_return(__wrap_zmalloc, NULL);
    l2 = listAddNodeHead(l, &a);
    assert_null(l2);

    will_return(__wrap_zmalloc, NULL);
    l3 = listAddNodeTail(l, &a);
    assert_null(l3);

    mock_malloc = 0;
    listRelease(l);
}

void* my_dup(void* p) {
    char **p1 = (char**)p;
    int size = strlen(*p1);
    char *p2 = (char*)malloc(size+1);
    char **p3 = (char**)malloc(sizeof(char**));
    *p3 = p2;
    strcpy(p2, *p1);
    return p3;
}

void my_free(void* p) {
    char **p1 = (char**)p;
    free(*p1);
    free(p1);
}
static void test_list_dup_func(void **state) {
    list *l = listCreate(), *l2;
    char *a[] = {"aaa", "bbb", "ccc", "ddd"}, i;

    for (i = 0; i < 4; ++i) {
        listAddNodeHead(l, &a[i]);
    }
    assert_int_equal(listLength(l), 4);
    assert_string_equal(*(char**)listLast(l)->value, "aaa");

    listSetDupMethod(l, my_dup);
    l2 = listDup(l);
    listSetFreeMethod(l2, my_free);
    assert_int_equal(listLength(l2), 4);
    assert_string_equal(*(char**)listLast(l2)->value, "aaa");

    a[0] = "111";
    assert_string_equal(*(char**)listLast(l)->value, "111");
    assert_string_equal(*(char**)listLast(l2)->value, "aaa");


    listRelease(l);
    listRelease(l2);
}


static void test_list_create_release(void **state) {
    list *l = listCreate();
    assert_non_null(l);
    listRelease(l);
}

static void test_list_dup_search(void **state) {

    list *l = listCreate(), *l2;
    int a[] = {1,2,3,4}, i;
    listNode *p;
    listIter *li;

    for (i = 0; i < 4; ++i) {
        listAddNodeHead(l, &a[i]);
    }
    assert_int_equal(listLength(l), 4);

    l2 = listDup(l);
    assert_int_equal(listLength(l), 4);

    p = listSearchKey(l2, &a[1]);
    assert_int_equal(*(int*)listNodeValue(p), 2);
    p = listNextNode(p);
    assert_int_equal(*(int*)p->value, 1);

    p = listIndex(l2, 2);
    assert_int_equal(*(int*)listNodeValue(p), 2);
    p = listNextNode(p);
    assert_int_equal(*(int*)p->value, 1);

    listRelease(l);
    listRelease(l2);


}
static void test_list_iter(void **state) {
    list *l = listCreate();
    int a[] = {1,2,3,4}, i;
    listNode *p;
    listIter *li;

    for (i = 0; i < 4; ++i) {
        listAddNodeHead(l, &a[i]);
    }
    assert_int_equal(listLength(l), 4);

    li = listGetIterator(l, 0);
    p = listNextElement(li);
    assert_int_equal(*(int*)listNodeValue(p), 4);
    p = listNextElement(li);
    assert_int_equal(*(int*)listNodeValue(p), 3);
    listReleaseIterator(li);

    li = listGetIterator(l, 1);
    p = listNextElement(li);
    assert_int_equal(*(int*)listNodeValue(p), 1);
    p = listNextElement(li);
    assert_int_equal(*(int*)listNodeValue(p), 2);
    listReleaseIterator(li);

    listRelease(l);
}

static void test_list_add(void **state) {
    list *l = listCreate();
    int a = 1, b = 2, c = 3, d = 4;
    listNode *p;

    listAddNodeHead(l, &a);
    listAddNodeHead(l, &b);
    assert_int_equal(listLength(l), 2);

    listAddNodeTail(l, &c);
    listAddNodeTail(l, &d);
    assert_int_equal(listLength(l), 4);

    assert_int_equal(*(int*)l->head->value, 2);
    assert_int_equal(*(int*)l->head->next->value, 1);
    assert_int_equal(*(int*)l->head->next->next->value, 3);
    assert_int_equal(*(int*)l->head->next->next->next->value, 4);

    assert_int_equal(*(int*)listFirst(l)->value, 2);
    assert_int_equal(*(int*)listLast(l)->value, 4);

    p = listNextNode(l->head);
    assert_int_equal(*(int*)p->value, 1);
    p = listPrevNode(p);
    assert_int_equal(*(int*)p->value, 2);

    assert_int_equal(*(int*)listNodeValue(p), 2);

    listDelNode(l, p);
    assert_int_equal(listLength(l), 3);
    assert_int_equal(*(int*)l->head->value, 1);

    listDelNode(l, listLast(l));
    assert_int_equal(listLength(l), 2);

    listRelease(l);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_list_create_release),
        cmocka_unit_test(test_list_create_fail),
        cmocka_unit_test(test_list_add_fail),
        cmocka_unit_test(test_list_add),
        cmocka_unit_test(test_list_iter),
        cmocka_unit_test(test_list_dup_search),
        cmocka_unit_test(test_list_dup_func),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}

