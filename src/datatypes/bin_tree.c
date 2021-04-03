#include "bin_tree.h"

#include <stdio.h>
#include <string.h>

#include "array.h"
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
    return symbol->value.letter;
}

struct TreeSerializeData
{
    size_t index;
    char content;
    BinTree *tree;
};

Operator __parse_op(char c)
{
    switch (c)
    {
    case '*':
        return KLEENE_STAR;
    case '|':
        return UNION;
    case '.':
        return CONCATENATION;
    case '+':
        return EXISTS;
    case '?':
        return MAYBE;
    }
    return KLEENE_STAR;
}

BinTree *dot_to_bin_tree(const char *path)
{
    FILE *file = fopen(path, "r");
    if (file == NULL)
        return NULL;

    // TODO secure scanf calls
    fscanf(file, "%*[^\n]");
    Array *corresp_table = Array(struct TreeSerializeData);
    struct TreeSerializeData top_data;
    fscanf(file, "  %zu[label=\"%c\"]\n", &top_data.index, &top_data.content);
    Symbol top = { .type = LETTER, .value = { .letter = top_data.content } };
    BinTree *top_node = BinTree(Symbol, &top, .left = NULL, .right = NULL);
    top_data.tree = top_node;
    array_append(corresp_table, &top_data);
    unsigned char result = 0;
    while ((result = fgetc(file)) != '}')
    {
        ungetc(result, file);
        struct TreeSerializeData d;
        fscanf(file, " %zu[label=\"%c\"]\n", &d.index, &d.content);
        Symbol symbol = { .type = LETTER, .value = { .letter = d.content } };
        BinTree *node = BinTree(Symbol, &symbol, .left = NULL, .right = NULL);
        d.tree = node;
        array_append(corresp_table, &d);
        size_t from, to;
        fscanf(file, "  %zu  ->  %zu\n", &from, &to);
        arr_foreach(struct TreeSerializeData, data, corresp_table)
        {
            if (data.index == from)
            {
                if (data.tree->left == NULL)
                    data.tree->left = node;
                else
                    data.tree->right = node;
                ((Symbol *)data.tree->data)->type = OPERATOR;
                ((Symbol *)data.tree->data)->value =
                    (SymbolValue){ .operator= __parse_op(data.content) };
            }
        }
    }
    //bin_tree_to_dot(top_node, stdout);
    array_free(corresp_table);
    fclose(file);
    return top_node;
}

void bin_tree_to_dot(BinTree *tree, FILE *file)
{
    fprintf(file, "digraph {\n");
    LinkedList *queue = LinkedList(BinTree *);
    if (tree)
    {
        list_push_back(queue, &tree);
        fprintf(file, "  %zu[label=\"%c\"]\n", (size_t)tree,
                __get_symbol_value(tree->data));
    }
    while (queue->next)
    {
        LinkedList *node_list = list_pop_front(queue);
        BinTree *node = *(BinTree **)(node_list->data);
        if (node->left)
        {
            fprintf(file, "  %zu[label=\"%c\"]\n", (size_t)node->left,
                    __get_symbol_value(node->left->data));
            fprintf(file, "  %zu  ->  %zu\n", (size_t)node, (size_t)node->left);
            list_push_back(queue, &node->left);
        }
        if (node->right)
        {
            fprintf(file, "  %zu[label=\"%c\"]\n", (size_t)node->right,
                    __get_symbol_value(node->right->data));
            fprintf(file, "  %zu  ->  %zu\n", (size_t)node,
                    (size_t)node->right);
            list_push_back(queue, &node->right);
        }
        list_free(node_list);
    }
    list_free(queue);
    fprintf(file, "}\n");
}

int bintree_compare(BinTree *a, BinTree *b)
{
    if (a == NULL && b == NULL)
        return 1;

    if((a == NULL && b != NULL) || (a != NULL && b == NULL))
        return 0;

    Symbol s_a = *(Symbol *)(a->data);
    Symbol s_b = *(Symbol *)(b->data);
    if(s_a.type == s_b.type)
    {
        if (s_a.type == LETTER && s_a.value.letter == s_b.value.letter)
        {
            return (bintree_compare(a->left, b->left) && bintree_compare(a->right, b->right));
        }
        if (s_a.value.operator == s_b.value.operator)
        {
            return (bintree_compare(a->left, b->left) && bintree_compare(a->right, b->right));
        }
    }
    return 0;

}
