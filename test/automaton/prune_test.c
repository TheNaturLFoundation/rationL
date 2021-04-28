#include <criterion/criterion.h>
#include <stdio.h>
#include <criterion/redirect.h>
#include <criterion/internal/assert.h>

#include "automaton/prune.h"
#include "automaton/automaton.h"
#include "automaton/thompson.h"
#include "datatypes/bin_tree.h"
#include "parsing/parsing.h"
#include "automaton/delete_eps.h"

/*
    Usefull functions for testing
*/

void _assert_array_content(int * arr, size_t n_args, ...)
{
    va_list ptr;
    va_start(ptr, n_args);
    for(size_t i = 0; i < n_args; i++)
    {
        cr_assert_eq(arr[i], va_arg(ptr, int));
    }
}

/*
    Test _prune_recursive function
*/

Test(prune, basic_reach_test)
{
    Automaton * automaton = automaton_from_daut("automaton/prune_daut/no_escape.daut", 2);
    int access[] = {0, 0};
    int escape[] = {0, 0};
    State *s = *(State **)array_get(automaton->states, 0);

    _automaton_prune(automaton, s, access, escape);

    _assert_array_content(access, 2, 1, 1);

    automaton_free(automaton);
}

Test(prune, basic_out_check_neg)
{
    Automaton * automaton = automaton_from_daut("automaton/prune_daut/no_escape.daut", 2);
    int access[] = {0, 0};
    int escape[] = {0, 0};

    State *s = *(State **)array_get(automaton->states, 0);
    
    _automaton_prune(automaton, s, access, escape);

    _assert_array_content(escape, 2, 0, 0);

    automaton_free(automaton);
}

Test(prune, reaching_neighbourghs)
{
    Automaton * automaton = automaton_from_daut("automaton/prune_daut/star.daut", 8);
    int access[8] = {0,};
    int escape[8] = {0,};

    State *s = *(State **)array_get(automaton->states, 0);

    _automaton_prune(automaton, s, access, escape);

    _assert_array_content(access, 8, 1, 1, 1, 1, 1, 1, 1, 1);

    automaton_free(automaton);
}

Test(prune, basic_perfect)
{
    Automaton * automaton = automaton_from_daut("automaton/prune_daut/perfect.daut", 5);
    int access[5] = {0,};
    int escape[5] = {0,};

    State *s = *(State **)array_get(automaton->states, 0);

    _automaton_prune(automaton, s, access, escape);

    _assert_array_content(access, 5, 1, 1, 1, 1, 1);
    _assert_array_content(escape, 5, 1, 1, 1, 1, 1);

    automaton_free(automaton);
}

Test(prune, beyound_limits)
{
    Automaton * automaton = automaton_from_daut("automaton/prune_daut/after_end.daut", 5);
    int access[5] = {0,};
    int escape[5] = {0,};

    State *s = *(State **)array_get(automaton->states, 0);

    _automaton_prune(automaton, s, access, escape);

    _assert_array_content(access, 5, 1, 1, 1, 1, 1);
    _assert_array_content(escape, 5, 1, 1, 0, 0, 0);

    automaton_free(automaton);    
}

Test(prune, unreached)
{
    Automaton * automaton = automaton_from_daut("automaton/prune_daut/strange_state.daut", 5);

    int access[4] = {0,};
    int escape[4] = {0,};

    State *s = *(State **)array_get(automaton->states, 0);

    _automaton_prune(automaton, s, access, escape);

    _assert_array_content(access, 4, 1, 1, 0, 0);
    _assert_array_content(escape, 4, 1, 1, 0, 0);

    automaton_free(automaton);
}

Test(prune, functional)
{
    Automaton * automaton = automaton_from_daut("automaton/prune_daut/func.daut", 11);
    int access[11] = {0,};
    int escape[11] = {0,};

    State *s = *(State **)array_get(automaton->states, 0);
    _automaton_prune(automaton, s, access, escape);

    s = *(State **)array_get(automaton->states, 4);
    _automaton_prune(automaton, s, access, escape);

    _assert_array_content(access, 11, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1);
    _assert_array_content(escape, 11, 1, 1, 0, 1, 1, 1, 1, 0, 1, 0, 0);

    automaton_free(automaton);
}

/*
    functional test for the prune algo
*/

Test(prune, removed_states)
{
    Automaton * automaton = automaton_from_daut("automaton/prune_daut/func.daut", 11);

    State * removed[4];
    removed[0] = *(State **)array_get(automaton->states, 2);
    removed[1] = *(State **)array_get(automaton->states, 7);
    removed[2] = *(State **)array_get(automaton->states, 9);
    removed[3] = *(State **)array_get(automaton->states, 10);
     
    automaton_prune(automaton);
    
    cr_assert_eq(automaton->size, 7, "Got %lu expected 7", automaton->size);


    arr_foreach(State *, s, automaton->states)
    {
        for(size_t i = 0; i < 4; i++)
            cr_assert_neq(s, removed[i], "found node %lu which was supposed to be deleted", 
                removed[i]->id);
    }

    automaton_free(automaton);
}

Test(flex, flex)
{
    Array *arr = tokenize("ab*");
    BinTree *b = parse_symbols(arr);
    Automaton *aut = thompson(b);
    
    // automaton_to_dot(aut);

    automaton_delete_epsilon_tr(aut);

    // automaton_to_dot(aut);
    
    automaton_prune(aut);

    // automaton_to_dot(aut);

    automaton_free(aut);
    array_free(arr);
    bintree_free(b);
}
