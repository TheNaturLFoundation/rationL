#include <stdio.h>

#include "bin_tree.h"
#include "linked_list.h"
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

char __get_symbol_value(Symbol *symbol)
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
        case MAYBE:
            return '?';
        case EXISTS:
            return '+';
        }
    else
        return symbol->value.letter;
}


void bin_tree_to_dot(BinTree *tree, FILE* file)
{
    fprintf(file, "digraph {\n");
    LinkedList *queue = LinkedList(BinTree*);
    if (tree)
    {
        list_push_back(queue, &tree);
        fprintf(file, "  %zu [label=\"%c\"]\n", (size_t) tree, __get_symbol_value(tree->data));
    }
    while(queue->next)
    {
        LinkedList* node_list = list_pop_front(queue);
        BinTree *node = *(BinTree**)(node_list->data);
        if(node->left)
        {
            fprintf(file, "  %zu[label=\"%c\"]\n", (size_t) node->left, __get_symbol_value(node->left->data));
            fprintf(file, "  %zu  ->  %zu\n", (size_t) node, (size_t) node->left);
            list_push_back(queue, &node->left);
        }
        if(node->right)
        {
            fprintf(file, "  %zu[label=\"%c\"]\n", (size_t) node->right, __get_symbol_value(node->right->data));
            fprintf(file, "  %zu  ->  %zu\n", (size_t) node, (size_t) node->right);
            list_push_back(queue, &node->right);
        }
        list_free(node_list);
    }
    list_free(queue);
    fprintf(file, "}\n");
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
