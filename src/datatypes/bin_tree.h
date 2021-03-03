#ifndef BIN_TREE_H
#define BIN_TREE_H

#include <stddef.h>
#include <stdlib.h>
#include <string.h>


#define BinTree(T, data, ...)                                           \
  create_node(&(BinTree)                                                \
              {.size = sizeof(T), .data = &(T){data}, __VA_ARGS__})

/*
 * @struct BinTree
 * @debrief Basic generic binary tree implementation
 * @author Vlad Argatu
 * @date 02/03/2020
 */
typedef struct BinTree
{
    void *data;
    size_t size;
    struct BinTree *left;
    struct BinTree *right;
} BinTree;

/*
 * @author Antoine Sicard
 * @date 03/03/2020 11H42
 * @param data_size: The size of the data stored in the array.
 * @param data: Pointer to the data.
 * @return a new BinTree node

 Initializes a binary tree node with both soons as NULL pointers.
*/

BinTree *create_node(BinTree *macro_defined_tree);
// use memcopy to copy the data and not ugly cast

/*
 * @author Antoine Sicard
 * @date 03/03/2020 11h42
 * @param tree: Binary tree that will be freed.

Frees the binary tree.
*/

void free_bintree(BinTree *tree);

#endif
