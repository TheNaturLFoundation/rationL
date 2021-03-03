#ifndef BIN_TREE_H
#define BIN_TREE_H

#define BinTree(T, VALUE)\
	T x = VALUE;\
	create_node(sizeof(T), &x)

/*
 * @struct BinTree
 * @debrif Basic generic binary tree implementation
 * @author Vlad Argatu
 * @date 02/03/2020
*/
typedef struct BinTree {
	void *data;
	bintree * left;
	bintree * right;
} BinTree;

/*
 * @author Vlad Argatu
 * @date 02/03/2020
 * @param data_size: The size of the data stored in the array.
 * @param data: Pointer to the data.

 Initializes a binary tree node with both soons as NULL pointers.
*/

BinTree * create_node(size_t data_size, const void * data);
//use memcopy to copy the data and not ugly cast

/*
 * @author Vlad Argatu
 * @date 03/03/2020
 * @param tree: Binary tree that will be freed.

Frees the binary tree.
*/

void free_bintree(BinTree * tree);

#endif
