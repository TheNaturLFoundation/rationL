#include <criterion/criterion.h>
#include <stdio.h>
#include <criterion/redirect.h>
#include <criterion/internal/assert.h>
#include "utils/memory_utils.h"
#include "automaton/prune.h"
#include "automaton/automaton.h"
#include "automaton/thompson.h"
#include "datatypes/bin_tree.h"
#include "parsing/parsing.h"
#include "automaton/delete_eps.h"

/*
    Usefull functions for testing
*/

void _assert_array_content_int(int * arr, size_t n_args, ...)
{
    va_list ptr;
    size_t elt;
    va_start(ptr, n_args);
    for(size_t i = 0; i < n_args; i++)
    {
        elt = va_arg(ptr, int);
        cr_assert_eq(arr[i], elt , "index: %lu, exepected %zu but got %d\n", i, elt, arr[i]);
    }
}


void _assert_array_content(size_t * arr, size_t n_args, ...)
{
    va_list ptr;
    size_t elt;
    va_start(ptr, n_args);
    for(size_t i = 0; i < n_args; i++)
    {
        elt = va_arg(ptr, size_t);
        cr_assert_eq(arr[i], elt , "index: %lu, exepected %lu but got %lu\n", i, elt, arr[i]);
    }
}

/*
    Test _prune_recursive function
*/

Test(prune, basic_reach_test)
{
    Automaton * automaton = automaton_from_daut("automaton/prune_daut/no_escape.daut", 2);
    size_t pref[] = {0, 0};
    int escape[] = {0, 0};
    State * higher[2] = {NULL, };
    size_t cpt = 0;

    State *s = *(State **)array_get(automaton->states, 0);

    _automaton_prune(automaton, s, pref, escape, higher, &cpt);

    _assert_array_content(pref, 2, 1, 2);

    automaton_free(automaton);
}

Test(prune, basic_out_check_neg)
{
    Automaton * automaton = automaton_from_daut("automaton/prune_daut/no_escape.daut", 2);
    size_t pref[] = {0, 0};
    int escape[] = {0, 0};
    State * higher[2] = {NULL,};
    size_t cpt = 0;


    State *s = *(State **)array_get(automaton->states, 0);
    
    _automaton_prune(automaton, s, pref, escape, higher, &cpt);

    _assert_array_content_int(escape, 2, 0, 0);

    automaton_free(automaton);
}

Test(prune, reaching_neighbourghs)
{
    Automaton * automaton = automaton_from_daut("automaton/prune_daut/star.daut", 8);
    size_t pref[8] = {0, };
    int escape[8] = {0,};
    State * higher[8] = {NULL,};
    size_t cpt = 0;

    State *s = *(State **)array_get(automaton->states, 0);
    
    _automaton_prune(automaton, s, pref, escape, higher, &cpt);

    for(size_t i = 0; i < automaton->size; i++)
    {
        cr_assert_neq(higher[i], NULL);
    }

    automaton_free(automaton);
}

Test(prune, basic_perfect)
{
    Automaton * automaton = automaton_from_daut("automaton/prune_daut/perfect.daut", 5);
    size_t pref[5] = {0, };
    int escape[5] = {0,};
    State * higher[5] = {NULL, };
    size_t cpt = 0;

    State *s = *(State **)array_get(automaton->states, 0);
    
    _automaton_prune(automaton, s, pref, escape, higher, &cpt);

    for(size_t i = 0; i < automaton->size; i++)
    {
        s = *(State **)array_get(automaton->states, i);
        cr_assert_eq(pref[i], i + 1);
        cr_assert_eq(higher[i], s, "Failed for i = %lu, expected %lu but got %lu",
                i, s->id, higher[i]->id);
    }
    _assert_array_content_int(escape, 5, 1, 1, 1, 1, 1);
    
    automaton_free(automaton);
}

Test(prune, beyound_limits)
{
    Automaton * automaton = automaton_from_daut("automaton/prune_daut/after_end.daut", 5);
    size_t pref[5] = {0, };
    int escape[5] = {0,};
    State * higher[5] = {NULL, };
    size_t cpt = 0;

    State *s = *(State **)array_get(automaton->states, 0);
    
    _automaton_prune(automaton, s, pref, escape, higher, &cpt);

    for(size_t i = 0; i < automaton->size; i++)
    {
        cr_assert_neq(higher[i], NULL);
    }
    _assert_array_content_int(escape, 5, 1, 1, 0, 0, 0);

    automaton_free(automaton);    
}


Test(prune, unreached)
{
    Automaton * automaton = automaton_from_daut("automaton/prune_daut/strange_state.daut", 5);

    size_t pref[5] = {0, };
    int escape[5] = {0,};
    State * higher[5] = {NULL, };
    size_t cpt = 0;

    State *s = *(State **)array_get(automaton->states, 0);
    
    _automaton_prune(automaton, s, pref, escape, higher, &cpt);

    for(size_t i = 0; i < 4; i++)
    {
        if(i < 2)
            cr_assert_neq(higher[i], NULL);
        else
            cr_assert_eq(higher[i], NULL);
    }

    _assert_array_content_int(escape, 4, 1, 1, 0, 0);

    automaton_free(automaton);
}

Test(prune, higher_is_good)
{
    Automaton * automaton = automaton_from_daut("automaton/prune_daut/rostan.daut", 6);

    size_t pref[6] = {0, };
    int escape[6] = {0,};
    State * higher[6] = {NULL, };
    size_t cpt = 0;

    State *s = *(State **)array_get(automaton->states, 0);
    
    _automaton_prune(automaton, s, pref, escape, higher, &cpt);

    State * s1 = *(State **)array_get(automaton->states, 1);
    State * s5 = *(State **)array_get(automaton->states, 5);

    cr_assert_eq(higher[0], s);
    cr_assert_eq(higher[1], s);
    cr_assert_eq(higher[2], s1);
    cr_assert_eq(higher[3], s1);
    cr_assert_eq(higher[4], s);
    cr_assert_eq(higher[5], s5);

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

Test(prune, test_check)
{
    Automaton * automaton = automaton_from_daut("automaton/prune_daut/rostan.daut", 4);

    
    //automaton_to_dot(automaton);
    automaton_prune(automaton);

    //automaton_to_dot(automaton);

    automaton_free(automaton);
}

Test(flex, flex)
{
    Array *arr = tokenize("ab*");
    BinTree *b = parse_symbols(arr);
    Automaton *aut = thompson(b);
    
     //automaton_to_dot(aut);

    automaton_delete_epsilon_tr(aut);

    //automaton_to_dot(aut);
   
    automaton_prune(aut);

   // automaton_to_dot(aut);

    automaton_free(aut);
    array_free(arr);
    bintree_free(b);
}
