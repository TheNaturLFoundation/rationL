#include "bin_tree.h"

#include <stdio.h>

#include "utils/memory_utils.h"

BinTree *create_node(BinTree *macro_tree_data)
{
    BinTree *bintree = SAFEMALLOC(sizeof(BinTree));
    bintree->size = macro_tree_data->size;
    memcpy(bintree->data, macro_tree_data->data, macro_tree_data->size);
    bintree->left = macro_tree_data->left;
    bintree->right = macro_tree_data->right;
    return bintree;
}

void free_bintree(BinTree *tree)
{
    if (tree != NULL)
    {
        free(tree->data);
        free_bintree(tree->left);
        free_bintree(tree->right);
        free(tree);
    }
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
