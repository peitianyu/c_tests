#ifndef __TREE_H__
#define __TREE_H__

#include <stddef.h>
#include <stdlib.h>

typedef struct tree_node_t tree_node_t;
struct tree_node_t {
    tree_node_t *parent;

    tree_node_t **kids;  
    size_t kid_cnt, kid_cap; 

    void *data;    
};

tree_node_t *tree_node_new(void *data);
void tree_destroy(tree_node_t *root, void (*free_fn)(void *));
int tree_insert_child(tree_node_t *parent, tree_node_t *child);
int tree_remove(tree_node_t *node);
void tree_foreach_preorder(tree_node_t *root, void (*visit)(tree_node_t *, void *), void *user);
void tree_foreach_postorder(tree_node_t *root, void (*visit)(tree_node_t *, void *), void *user);

#ifdef TREE_IMPLEMENTATION

tree_node_t *tree_node_new(void *data)
{
    tree_node_t *n = (tree_node_t *)malloc(sizeof(tree_node_t));
    if (!n) return NULL;

    n->parent  = NULL;
    n->kids    = NULL;
    n->kid_cnt = 0;
    n->kid_cap = 0;
    n->data    = data;
    return n;
}

static int tree_reserve_kids(tree_node_t *n, size_t want)
{
    if (n->kid_cap >= want) return 0;
    size_t new_cap = n->kid_cap ? n->kid_cap * 2 : 4;
    if (new_cap < want) new_cap = want;

    tree_node_t **tmp = (tree_node_t **)realloc(n->kids,
                                                    new_cap * sizeof(tree_node_t *));
    if (!tmp) return -1;
    n->kids    = tmp;
    n->kid_cap = new_cap;
    return 0;
}

int tree_insert_child(tree_node_t *parent, tree_node_t *child)
{
    if (!parent || !child) return -1;
    if (tree_reserve_kids(parent, parent->kid_cnt + 1) != 0)
        return -1;

    parent->kids[parent->kid_cnt++] = child;
    child->parent = parent;
    return 0;
}

int tree_remove(tree_node_t *node)
{
    if (!node || !node->parent) return -1;
    tree_node_t *p = node->parent;

    /* 找到 node 在 kids 中的位置并删除 */
    size_t i = 0;
    while (i < p->kid_cnt && p->kids[i] != node) ++i;
    if (i == p->kid_cnt) return -1; /* 不应该发生 */

    /* 把最后一个元素挪过来或直接 memmove */
    p->kids[i] = p->kids[--p->kid_cnt];
    node->parent = NULL;
    return 0;
}

void tree_destroy(tree_node_t *root, void (*free_fn)(void *))
{
    if (!root) return;

    /* 后序遍历释放，先孩子后自己 */
    for (size_t i = 0; i < root->kid_cnt; ++i)
        tree_destroy(root->kids[i], free_fn);

    if (free_fn) free_fn(root->data);
    free(root->kids);
    free(root);
}

/* 先序遍历 */
void tree_foreach_preorder(tree_node_t *root, void (*visit)(tree_node_t *, void *), void *user)
{
    if (!root || !visit) return;
    visit(root, user);
    for (size_t i = 0; i < root->kid_cnt; ++i)
        tree_foreach_preorder(root->kids[i], visit, user);
}

/* 后序遍历 */
void tree_foreach_postorder(tree_node_t *root, void (*visit)(tree_node_t *, void *), void *user)
{
    if (!root || !visit) return;
    for (size_t i = 0; i < root->kid_cnt; ++i)
        tree_foreach_postorder(root->kids[i], visit, user);
    visit(root, user);
}

#endif /* TREE_IMPLEMENTATION */
#endif /* __TREE_H__ */