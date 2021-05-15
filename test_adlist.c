#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>
#include <stdio.h>
#include "adlist.h"

void *__wrap_zmalloc(size_t size);
void *__real_zmalloc(size_t size);
void *__wrap_zmalloc(size_t size) {
    return (void*) mock();
}

static void test_list_create_fail(void **state) {
    will_return(__wrap_zmalloc, NULL);
    list *l = listCreate();
    assert_null(l);
}

static void test_list_create_release(void **state) {
    will_return(__wrap_zmalloc, __real_zmalloc(sizeof(list)));
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
    will_return(__wrap_zmalloc, __real_zmalloc(sizeof(list)));
    list *l = listCreate();
    int a[] = {1,2,3,4}, i;
    listNode *p;
    listIter *li;

    for (i = 0; i < 4; ++i) {
        will_return(__wrap_zmalloc, __real_zmalloc(sizeof(listNode)));
        listAddNodeHead(l, &a[i]);
    }
    assert_int_equal(listLength(l), 4);

    will_return(__wrap_zmalloc, __real_zmalloc(sizeof(listIter)));
    li = listGetIterator(l, 0);
    p = listNextElement(li);
    assert_int_equal(*(int*)listNodeValue(p), 4);
    p = listNextElement(li);
    assert_int_equal(*(int*)listNodeValue(p), 3);
    listReleaseIterator(li);

    will_return(__wrap_zmalloc, __real_zmalloc(sizeof(listIter)));
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

    listRelease(l);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_list_create_release),
        cmocka_unit_test(test_list_create_fail),
        //cmocka_unit_test(test_list_add),
        cmocka_unit_test(test_list_iter),
        //cmocka_unit_test(test_list_dup_search),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}

