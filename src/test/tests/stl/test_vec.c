#define VEC_IMPLEMENTATION
#include "core/stl/vec.h"
#include "core/c_test.h"
#include <stdio.h>

static void test_basic(void) {
    vec_t v = vec_create();
    ASSERT_TRUE(vec_size(v) == 0);
    ASSERT_TRUE(vec_capacity(v) == 0);

    vec_push(v, int, 10);
    vec_push(v, int, 20);
    vec_push(v, int, 30);

    ASSERT_TRUE(vec_size(v) == 3);
    ASSERT_TRUE(vec_capacity(v) >= 3);
    ASSERT_TRUE(((int *)v)[0] == 10 && ((int *)v)[1] == 20 && ((int *)v)[2] == 30);

    vec_free(v);
}

static void test_insert_erase(void) {
    vec_t v = vec_create();

    for (int i = 0; i < 4; ++i) {
        vec_push(v, int, i);
    }
    vec_insert(v, int, 2, 42);

    int expect[] = {0, 1, 42, 2, 3};
    for (size_t i = 0; i < 5; ++i)
        ASSERT_TRUE(((int *)v)[i] == expect[i]);

    vec_erase(v, sizeof(int), 2, 1);
    ASSERT_TRUE(vec_size(v) == 4);
    ASSERT_TRUE(((int *)v)[2] == 2);

    vec_pop(v);
    ASSERT_TRUE(vec_size(v) == 3);

    vec_free(v);
}

static void test_insert_array(void) {
    vec_t v = vec_create();

    int init[] = {0, 1, 2, 3};
    for (size_t i = 0; i < 4; ++i) {
        vec_push(v, int, init[i]);
    }

    int to_insert[] = {42, 43, 44};
    vec_insert_array(&v, sizeof(int), 2, to_insert, 3);

    int expect[] = {0, 1, 42, 43, 44, 2, 3};
    for (size_t i = 0; i < 7; ++i)
        ASSERT_TRUE(((int *)v)[i] == expect[i]);

    vec_free(v);
}

static void test_reserve_and_copy(void) {
    vec_t v = vec_create();
    vec_reserve(&v, sizeof(double), 100);
    ASSERT_TRUE(vec_capacity(v) >= 100);

    for (double d = 0.5; d < 10.5; d += 1.0) {
        double *p = (double *)vec_add_dst(&v, sizeof(double));
        *p = d;
    }

    vec_t c = vec_copy(v, sizeof(double));
    ASSERT_TRUE(vec_size(c) == vec_size(v));
    ASSERT_TRUE(vec_capacity(c) == vec_size(c));
    ASSERT_TRUE(c != v);
    for (size_t i = 0; i < vec_size(v); ++i)
        ASSERT_TRUE(((double *)v)[i] == ((double *)c)[i]);

    vec_free(v);
    vec_free(c);
}

static void test_empty_ops(void) {
    vec_t v = vec_create();
    ASSERT_TRUE(vec_size(v) == 0);

    if (vec_size(v)) vec_pop(v);
    vec_erase(v, sizeof(char), 0, 0);

    vec_free(v);
}

static void test_char(void) {
    vec_t v = vec_create();

    vec_push(v, char, 'a');
    vec_push(v, char, 'b');

    vec_reserve(&v, sizeof(char), vec_size(v) + 1);
    ((char *)v)[vec_size(v)] = '\0';
    printf("%s\n", (char *)v);

    vec_free(v);
}

static void test_vec_at(void) {
    vec_t v = vec_create();
    vec_push(v, int, 10);
    vec_push(v, int, 20);
    vec_push(v, int, 30);
    ASSERT_TRUE(vec_at(v, int, 0) == 10);
    ASSERT_TRUE(vec_at(v, int, 1) == 20);
    ASSERT_TRUE(vec_at(v, int, 2) == 30);
    vec_free(v);
}

TEST(vec, test)
{
    test_basic();
    test_insert_erase();
    test_reserve_and_copy();
    test_insert_array();
    test_empty_ops();
    test_char();
    test_vec_at();
    puts("All tests passed.");
}