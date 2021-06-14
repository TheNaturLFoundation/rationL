#include "datatypes/bin_tree.h"

#include <stdio.h>
#include <string.h>

#include "datatypes/array.h"
#include "datatypes/linked_list.h"
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
//LCOV_EXCL_START
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
    SymbolType type;
    int group;
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

    int type_check;
    // TODO secure scanf calls
    if (fscanf(file, "%*[^\n]") == -1)
        errx(1, "fscanf failed"); //LCOV_EXCL_LINE
    Array *corresp_table = Array(struct TreeSerializeData);
    struct TreeSerializeData top_data;
    if (fscanf(file, "  %zu[label=\"%c\" xlabel=\"%i\" peripheries=\"%i\"]\n",
               &top_data.index, &top_data.content, &top_data.group, &type_check) == -1)
        errx(1, "fscanf failed"); //LCOV_EXCL_LINE
    Symbol top;
    if (type_check == 2)
    {
        top.type = OPERATOR;
        top.value.operator = __parse_op(top_data.content);
        top.group = top_data.group;
    }
    else
    {
        top.type = LETTER;
        top.value.letter = top_data.content;
        top.group = top_data.group;
    }
    BinTree *top_node = BinTree(Symbol, &top, .left = NULL, .right = NULL);
    top_data.tree = top_node;
    array_append(corresp_table, &top_data);
    unsigned char result = 0;
    while ((result = fgetc(file)) != '}')
    {
        ungetc(result, file);
        struct TreeSerializeData d;
        if (fscanf(file, " %zu[label=\"%c\" xlabel=\"%i\" peripheries=\"%i\"]\n",
                   &d.index, &d.content, &d.group, &type_check) == -1)
            errx(1, "fscanf failed"); //LCOV_EXCL_LINE
        Symbol symbol;
        if (type_check == 2)
        {
            symbol.type = OPERATOR;
            symbol.value.operator = __parse_op(d.content);
            symbol.group = d.group;
        }
        else
        {
            symbol.type = LETTER;
            symbol.value.letter = d.content;
            symbol.group = d.group;
        }
        BinTree *node = BinTree(Symbol, &symbol, .left = NULL, .right = NULL);
        d.tree = node;
        array_append(corresp_table, &d);
        size_t from, to;
        if (fscanf(file, "  %zu  ->  %zu\n", &from, &to) == -1)
            errx(1, "fscanf failed"); //LCOV_EXCL_LINE
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
        int group = ((Symbol *)(tree->data))->group;
        int peripheries = (((Symbol *)(tree->data))->type == OPERATOR) + 1;
        list_push_back(queue, &tree);
        fprintf(file, "  %zu[label=\"%c\" xlabel=\"%i\" peripheries=\"%i\"]\n", (size_t)tree,
                __get_symbol_value(tree->data), group, peripheries);
    }
    while (queue->next)
    {
        LinkedList *node_list = list_pop_front(queue);
        BinTree *node = *(BinTree **)(node_list->data);
        if (node->left)
        {
            int group = ((Symbol *)(node->left->data))->group;
            int peripheries = (((Symbol *)(node->left->data))->type == OPERATOR) + 1;
            fprintf(file, "  %zu[label=\"%c\" xlabel=\"%i\" peripheries=\"%i\"]\n", (size_t)node->left,
                    __get_symbol_value(node->left->data), group, peripheries);
            fprintf(file, "  %zu  ->  %zu\n", (size_t)node, (size_t)node->left);
            list_push_back(queue, &node->left);
        }
        if (node->right)
        {
            int group = ((Symbol *)(node->right->data))->group;
            int peripheries = (((Symbol *)(node->right->data))->type == OPERATOR) + 1;
            fprintf(file, "  %zu[label=\"%c\" xlabel=\"%i\" peripheries=\"%i\"]\n", (size_t)node->right,
                    __get_symbol_value(node->right->data), group, peripheries);
            fprintf(file, "  %zu  ->  %zu\n", (size_t)node,
                    (size_t)node->right);
            list_push_back(queue, &node->right);
        }
        list_free(node_list);
    }
    list_free(queue);
    fprintf(file, "}\n");
}
//LCOV_EXCL_STOP

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
        if (s_a.type == LETTER && s_a.value.letter == s_b.value.letter && s_a.group == s_b.group)
        {
            return (bintree_compare(a->left, b->left) && bintree_compare(a->right, b->right));
        }
        else if (s_a.type == OPERATOR && s_a.value.operator == s_b.value.operator && s_a.group == s_b.group)
        {
            return (bintree_compare(a->left, b->left) && bintree_compare(a->right, b->right));
        }
        else if (s_a.type == CHARACTER_CLASS)
        {
            return bintree_compare(a->left, b->left) && bintree_compare(a->right, b->right);
        }
    }
    return 0;

}
