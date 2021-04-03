#ifndef BIN_TREE_H
#define BIN_TREE_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BinTree(T, value, ...)                                                 \
    create_node(&(BinTree){ .size = sizeof(T),                                 \
                            .data = value,                                     \
                            __VA_ARGS__ })

#define BinTreeAtomic(T, value, ...)                                           \
    create_node(&(BinTree){ .size = sizeof(T),                                 \
                            .data = &(T){ value },                             \
                            __VA_ARGS__ })
/**
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

/**
 * @author Antoine Sicard
 * @date 03/03/2020 11H42
 * @param data_size: The size of the data stored in the array.
 * @param data: Pointer to the data.
 * @return a new BinTree node

 Initializes a binary tree node with both soons as NULL pointers.
*/

BinTree *create_node(BinTree *macro_defined_tree);
// use memcopy to copy the data and not ugly cast

/**
 * @author Antoine Sicard
 * @date 03/03/2020 11h42
 * @param tree: Binary tree that will be freed.

Frees the binary tree.
*/
void bintree_free(BinTree *tree);


void bin_tree_to_dot(BinTree *t, FILE* file);


BinTree *dot_to_bin_tree(char* path);

/**
 * @brief Check if 2 bintrees filled with Symbols are the same
 * @author Antoine Sicard
 * @date 2/04/2021
 * @param a : first BinTree
 * @param b : second BinTree
 * @return 1 if the bintrees are the same. 0 otherwise
 * */
int bintree_compare(BinTree *a, BinTree *b);

#endif
