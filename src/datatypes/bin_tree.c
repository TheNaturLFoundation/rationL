#include "bin_tree.h"
#include <stdio.h>

BinTree * create_node(size_t data_size, const void * data)
{
  BinTree *t = malloc(sizeof(BinTree));
  t->data = malloc(data_size);
  memcpy(t->data, data, data_size);
  t->left = NULL;
  t->right = NULL;
  return t;
}

void free_bintree(BinTree * tree)
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
