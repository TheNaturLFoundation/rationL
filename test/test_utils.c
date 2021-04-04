#include "test_utils.h"

#include <criterion/criterion.h>

#include "rationl/bin_tree.h"
#include "rationl/parsing.h"

void check_ast_from_file(Array *arr, const char *path)
{
    BinTree *got = parse_symbols(arr);
    BinTree *expected = dot_to_bin_tree(path);
    cr_assert(bintree_compare(expected, got));
    bintree_free(got);
    bintree_free(expected);
}
