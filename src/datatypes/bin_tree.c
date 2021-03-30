#include "bin_tree.h"

#include <stdio.h>

#include "parsing/parsing.h"
#include "utils/memory_utils.h"

BinTree *create_node(BinTree *macro_tree_data)
{
    BinTree *bintree = SAFEMALLOC(sizeof(BinTree));
    bintree->data = SAFEMALLOC(macro_tree_data->size);
    bintree->size = macro_tree_data->size;
    memcpy(bintree->data, macro_tree_data->data, macro_tree_data->size);
    bintree->left = macro_tree_data->left;
    bintree->right = macro_tree_data->right;
    return bintree;
}

void bintree_free(BinTree *tree)
{
    if (tree != NULL)
    {
        free(tree->data);
        bintree_free(tree->left);
        bintree_free(tree->right);
        free(tree);
    }
}
char get_val(Symbol *symbol)
{
    if (symbol->type == OPERATOR)
        switch (symbol->value.operator)
        {
        case KLEENE_STAR:
            return '*';
        case UNION:
            return '|';
        case CONCATENATION:
            return '.';
        case EXISTS:
            return '?';
        case MAYBE:
            return '+';
        }
    else
        return symbol->value.letter;
}

void __tree_to_dot(BinTree *tree, size_t current)
{
    printf("  node%zu [label=\"%c\"];\n", current, get_val(tree->data));
    if (tree->left != NULL)
    {
        printf("  node%zu -> node%zu;\n", current, 2 * current);
        __tree_to_dot(tree->left, 2 * current);
    }
    if (tree->right != NULL)
    {
        printf("  node%zu -> node%zu;\n", current, 2 * current + 1);
        __tree_to_dot(tree->right, 2 * current + 1);
    }
}

void tree_to_dot(BinTree *tree)
{
    printf("digraph {\n");
    __tree_to_dot(tree, 1);
    printf("}\n");
}

/*
int main()
{
  int x = 3;
  BinTree *t = create_node(sizeof(int), &x);

  printf("tree->data = %d\n", *((int *)(t->data)));

  free_bintree(t);
}
*/
