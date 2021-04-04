#pragma once


#include "automaton.h"
#include "bin_tree.h"

/**
 * @author Simon Scatton
 * @date 27/03/2021
 * @brief Create a NFA from a regular expression syntax tree.
 * @param tree The syntax tree that comes out of the parser.
*/
Automaton *thompson(BinTree *tree);
