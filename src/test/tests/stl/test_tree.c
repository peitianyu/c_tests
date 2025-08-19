#define TREE_IMPLEMENTATION
#include "core/stl/tree.h"
#include "core/c_test.h"
#include <stdio.h>
#include <stdlib.h>

static void free_int(void *data) {
    free((int *)data);
}

static int *int_new(int value) {
    int *p = (int *)malloc(sizeof(int));
    *p = value;
    return p;
}

static void test_create_and_destroy(void) {
    tree_node_t *root = tree_node_new(int_new(42));
    ASSERT_TRUE(root != NULL);
    ASSERT_TRUE(root->parent == NULL);
    ASSERT_TRUE(root->kid_cnt == 0);
    ASSERT_TRUE(root->data != NULL && *(int *)root->data == 42);
    tree_destroy(root, free_int);
}

static void test_insert_child(void) {
    tree_node_t *root = tree_node_new(int_new(0));
    tree_node_t *child1 = tree_node_new(int_new(1));
    tree_node_t *child2 = tree_node_new(int_new(2));

    ASSERT_TRUE(tree_insert_child(root, child1) == 0);
    ASSERT_TRUE(tree_insert_child(root, child2) == 0);
    ASSERT_TRUE(root->kid_cnt == 2);
    ASSERT_TRUE(root->kids[0] == child1 && root->kids[1] == child2);
    ASSERT_TRUE(child1->parent == root && child2->parent == root);

    tree_destroy(root, free_int);
}

static void test_remove_node(void) {
    tree_node_t *root = tree_node_new(int_new(0));
    tree_node_t *child1 = tree_node_new(int_new(1));
    tree_node_t *child2 = tree_node_new(int_new(2));

    tree_insert_child(root, child1);
    tree_insert_child(root, child2);

    ASSERT_TRUE(tree_remove(child1) == 0);
    ASSERT_TRUE(root->kid_cnt == 1);
    ASSERT_TRUE(root->kids[0] == child2);
    ASSERT_TRUE(child1->parent == NULL);

    tree_destroy(root, free_int);
    tree_destroy(child1, free_int); 
}

static void test_edge_cases(void) {
    tree_node_t *root = tree_node_new(NULL);
    ASSERT_TRUE(root != NULL);
    ASSERT_TRUE(tree_insert_child(root, NULL) == -1);
    ASSERT_TRUE(tree_remove(root) == -1);
    ASSERT_TRUE(tree_remove(NULL) == -1);

    tree_destroy(root, NULL);
}

struct capture {
    char buf[128];
    size_t len;
};

static void visit_append(tree_node_t *n, void *user)
{
    /* 把节点里的 int 值追加到字符串末尾 */
    struct capture *c = (struct capture *)user;
    int val = *(int *)n->data;
    int nwr = snprintf(c->buf + c->len,
                       sizeof(c->buf) - c->len,
                       "%d", val);
    if (nwr > 0 && (size_t)nwr < sizeof(c->buf) - c->len)
        c->len += (size_t)nwr;
}

/* ---------- 先序遍历测试 ---------- */
static void test_foreach_preorder(void)
{
    /*
     *      1
     *    /   \
     *   2     3
     *  / \   /
     * 4   5 6
     * 先序：1 2 4 5 3 6
     */
    tree_node_t *root = tree_node_new(int_new(1));
    tree_node_t *n2   = tree_node_new(int_new(2));
    tree_node_t *n3   = tree_node_new(int_new(3));
    tree_node_t *n4   = tree_node_new(int_new(4));
    tree_node_t *n5   = tree_node_new(int_new(5));
    tree_node_t *n6   = tree_node_new(int_new(6));

    tree_insert_child(root, n2);
    tree_insert_child(root, n3);
    tree_insert_child(n2, n4);
    tree_insert_child(n2, n5);
    tree_insert_child(n3, n6);

    struct capture cap = { .len = 0, .buf = "" };
    tree_foreach_preorder(root, visit_append, &cap);

    ASSERT_TRUE(cap.len == 6);
    ASSERT_STREQ(cap.buf, "124536");

    tree_destroy(root, free_int);
}

/* ---------- 后序遍历测试 ---------- */
static void test_foreach_postorder(void)
{
    /*
     * 同一棵树：
     * 后序：4 5 2 6 3 1
     */
    tree_node_t *root = tree_node_new(int_new(1));
    tree_node_t *n2   = tree_node_new(int_new(2));
    tree_node_t *n3   = tree_node_new(int_new(3));
    tree_node_t *n4   = tree_node_new(int_new(4));
    tree_node_t *n5   = tree_node_new(int_new(5));
    tree_node_t *n6   = tree_node_new(int_new(6));

    tree_insert_child(root, n2);
    tree_insert_child(root, n3);
    tree_insert_child(n2, n4);
    tree_insert_child(n2, n5);
    tree_insert_child(n3, n6);

    struct capture cap = { .len = 0, .buf = "" };
    tree_foreach_postorder(root, visit_append, &cap);

    ASSERT_TRUE(cap.len == 6);
    ASSERT_STREQ(cap.buf, "452631");

    tree_destroy(root, free_int);
}

TEST(tree, test) {
    test_create_and_destroy();
    test_insert_child();
    test_remove_node();
    test_edge_cases();
    test_foreach_preorder();
    test_foreach_postorder();
    puts("All tree tests passed.");
}
