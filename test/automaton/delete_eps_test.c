#include <criterion/criterion.h>
#include <stdarg.h>

#include "utils.h"
#include "automaton/thompson.h"
#include "datatypes/bin_tree.h"
#include "parsing/parsing.h"
#include "automaton/delete_eps.h"


void _assert_list_content(LinkedList * list, size_t n_args, ...)
{
    if(n_args == 0)
    {
        cr_assert_eq(list, NULL);
    }
    else
    {
        va_list ptr;
        State * trg;
        va_start(ptr, n_args);
        for(size_t i = 0; i < n_args; i++)
        {
            cr_assert_neq(list->next, NULL);
            trg = *(State **)list->next->data;
            cr_assert_eq(trg, va_arg(ptr, State *));
            list = list->next;
        }

        cr_assert_eq(list->next, NULL, "Expected NULL but go list with next = %p",
            list->next);
    }
}

void _assert_non_eps_integrity(Automaton * automaton, Automaton * automaton_int)
{
    State * t;
    int epsilon_col = automaton->lookup_table[EPSILON_INDEX];
    for(size_t x = 0; x < automaton->transition_table->width; x++)
    {
        for(size_t y = 0; y < automaton->transition_table->height; y++)
        {
            //printf("State : %p\n\n", matrix_get(automaton->transition_table, x, y));
            if(matrix_get(automaton_int->transition_table, x, y) != NULL)
            {
                if(x == epsilon_col)
                {
                    cr_assert_eq(matrix_get(automaton->transition_table, x, y), NULL);
                }
                else
                {
                    cr_assert_neq(matrix_get(automaton->transition_table, x, y), NULL);
                }
            }
            else
            {
                t = *(State **)matrix_get(automaton->transition_table, x, y)->next->data;  
                cr_assert_eq(matrix_get(automaton->transition_table, x, y), NULL,
                    "Expected NULL but go cancer ie : %lu\n", t->id);
            }
        }
    }
}

/*
    Test for transfer_all_transitions
*/

Test(delete_eps, test_transfer_all_transitions_into_empty)
{
    Automaton * automaton = Automaton(3, 1);

    State * s1 = State(0);
    State * s2 = State(0);
    State * s3 = State(0);

    automaton_add_state(automaton, s1, 0);
    automaton_add_state(automaton, s2, 0);
    automaton_add_state(automaton, s3, 0);

    automaton_add_transition(automaton, s1, s3, 'A', 0);

    transfer_all_transitions(automaton, s1, s2);

    LinkedList * list;
    State * trg;

    //Check the integrity of s1:
    list = get_matrix_elt(automaton, s1->id, 'A', 0);
    _assert_list_content(list, 1, s3);

    //Check the integrity of s3:
    list = get_matrix_elt(automaton, s3->id, 'A', 0);
    _assert_list_content(list, 0);

    //check the good copy in empty s2:
    list = get_matrix_elt(automaton, s2->id, 'A', 0);
    _assert_list_content(list, 1, s3);

    automaton_free(automaton);
}

Test(delete_eps, test_transfer_all_transitions_into_non_empty)
{
    Automaton * automaton = Automaton(3, 1);

    State * s1 = State(0);
    State * s2 = State(0);
    State * s3 = State(0);

    automaton_add_state(automaton, s1, 0);
    automaton_add_state(automaton, s2, 0);
    automaton_add_state(automaton, s3, 0);

    automaton_add_transition(automaton, s1, s3, 'A', 0);
    automaton_add_transition(automaton, s2, s1, 'A', 0);

    transfer_all_transitions(automaton, s1, s2);

    LinkedList * list;
    State * trg;

    //Check the integrity of s1:
    list = get_matrix_elt(automaton, s1->id, 'A', 0);
    _assert_list_content(list, 1, s3);

    //Check the integrity of s3:
    list = get_matrix_elt(automaton, s3->id, 'A', 0);
    _assert_list_content(list, 0);


    //check the good copy in empty s2:
    list = get_matrix_elt(automaton, s2->id, 'A', 0);
    _assert_list_content(list, 2, s1, s3);

    automaton_free(automaton);
}

Test(delete_eps, test_transfer_all_transitions_into_empty_epsilon)
{
    Automaton * automaton = Automaton(3, 1);

    State * s1 = State(0);
    State * s2 = State(0);
    State * s3 = State(0);

    automaton_add_state(automaton, s1, 0);
    automaton_add_state(automaton, s2, 0);
    automaton_add_state(automaton, s3, 0);

    automaton_add_transition(automaton, s1, s3, 'A', 1);

    transfer_all_transitions(automaton, s1, s2);

    LinkedList * list;
    State * trg;

    //Check the integrity of s1:
    list = get_matrix_elt(automaton, s1->id, 'A', 1);
    _assert_list_content(list, 1, s3);

    //Check the integrity of s3:
    list = get_matrix_elt(automaton, s3->id, 'A', 1);
    _assert_list_content(list, 0);

    //check the good copy in empty s2:
    list = get_matrix_elt(automaton, s2->id, 'A', 1);
    _assert_list_content(list, 1, s3);

    automaton_free(automaton);
}

Test(delete_eps, test_transfer_all_transitions_into_non_empty_epsilon)
{
    Automaton * automaton = Automaton(3, 1);

    State * s1 = State(0);
    State * s2 = State(0);
    State * s3 = State(0);

    automaton_add_state(automaton, s1, 0);
    automaton_add_state(automaton, s2, 0);
    automaton_add_state(automaton, s3, 0);

    automaton_add_transition(automaton, s1, s3, 'A', 1);
    automaton_add_transition(automaton, s2, s1, 'A', 1);

    transfer_all_transitions(automaton, s1, s2);

    LinkedList * list;
    State * trg;

    //Check the integrity of s1:
    list = get_matrix_elt(automaton, s1->id, 'A', 1);
    _assert_list_content(list, 1, s3);

    //Check the integrity of s3:
    list = get_matrix_elt(automaton, s3->id, 'A', 1);
    _assert_list_content(list, 0);


    //check the good copy in empty s2:
    list = get_matrix_elt(automaton, s2->id, 'A', 1);
    _assert_list_content(list, 2, s1, s3);

    automaton_free(automaton);
}

Test(delete_eps, transfer_multiple_elt_in_tr_list)
{
    Automaton * automaton = Automaton(4, 1);
    
    State * s1 = State(0);
    State * s2 = State(0);
    State * s3 = State(0);
    State * s4 = State(0);

    automaton_add_state(automaton, s1, 0);
    automaton_add_state(automaton, s2, 0);
    automaton_add_state(automaton, s3, 0);
    automaton_add_state(automaton, s4, 0);

    automaton_add_transition(automaton, s1, s3, 'A', 0);
    automaton_add_transition(automaton, s1, s4, 'A', 0);

    transfer_all_transitions(automaton, s1, s2);

    LinkedList * list = get_matrix_elt(automaton, s2->id, 'A', 0);
    _assert_list_content(list, 2, s3, s4);

    automaton_free(automaton);
}

Test(delete_eps, transfer_one_tr_into_multiple_check_integrity_of_reciever)
{
    Automaton * automaton = Automaton(4, 2);
    
    State * s1 = State(0);
    State * s2 = State(0);
    State * s3 = State(0);
    State * s4 = State(0);

    automaton_add_state(automaton, s1, 0);
    automaton_add_state(automaton, s2, 0);
    automaton_add_state(automaton, s3, 0);
    automaton_add_state(automaton, s4, 0);   

    automaton_add_transition(automaton, s1, s3, 'A', 0);
    automaton_add_transition(automaton, s2, s4, 'B', 1);

    transfer_all_transitions(automaton, s1, s2);

    LinkedList * list = get_matrix_elt(automaton, s2->id, 'A', 0);
    _assert_list_content(list, 1, s3);

    list = get_matrix_elt(automaton, s2->id, 'A', 1);
    _assert_list_content(list, 1, s4);

    automaton_free(automaton);
}

Test(delete_eps, transfer_multiple_lists)
{
    Automaton * automaton = Automaton(3, NUMBER_OF_SYMB);

    State * s1 = State(0);
    State * s2 = State(0);
    State * s3 = State(0);

    automaton_add_state(automaton, s1, 0);
    automaton_add_state(automaton, s2, 0);
    automaton_add_state(automaton, s3, 0);

    LinkedList * list;

    for(size_t i = 0; i < NUMBER_OF_SYMB; i++)
    {
        automaton_add_transition(automaton, s1, s3, i, i == EPSILON_INDEX);
    }

    transfer_all_transitions(automaton, s1, s2);

    for(size_t i = 0; i < NUMBER_OF_SYMB; i++)
    {
        //printf("%lu, %d\n\n", i, i == EPSILON_INDEX);
        list = get_matrix_elt(automaton, s2->id, i, i == EPSILON_INDEX);
        _assert_list_content(list, 1, s3);
    }

    automaton_free(automaton);
}

/*
    Tests for automaton_delete_epsilon_tr
*/

Test(delete_eps, automaton_single_delete_epsilon_tr_check_eps_null)
{
    Automaton * automaton = automaton_from_daut("automaton/single_check_delete.daut", 2);

    automaton_delete_epsilon_tr(automaton);

    LinkedList * list;
    State * target = *(State **)array_get(automaton->states, 0);
    
    list = get_matrix_elt(automaton, 1, 'a', 0);
    cr_assert_neq(list, NULL);

    list = get_matrix_elt(automaton, 0, ' ', 1);
    _assert_list_content(list, 0);

    automaton_free(automaton);
}

Test(delete_eps, automaton_multiple_delete_epsilon_tr_check_eps_null)
{
    Automaton * automaton = automaton_from_daut("automaton/multiple_check_delete.daut", 6);

    automaton_delete_epsilon_tr(automaton);

    LinkedList * list = get_matrix_elt(automaton, 1, 'e', 1);
    
    cr_assert_eq(list, NULL);

    automaton_free(automaton);
}

Test(delete_eps, corrrect_call_to_transfer)
{
    Automaton * automaton = automaton_from_daut("automaton/simple_3_states.daut", 3);
    
    automaton_delete_epsilon_tr(automaton);

    LinkedList * list;
    State * target;

    target = *(State **)array_get(automaton->states, 2);

    list = get_matrix_elt(automaton, 0, 'a', 0);
    _assert_list_content(list, 1, target);

    list = get_matrix_elt(automaton, 0, 'b', 0);
    _assert_list_content(list, 1, target);

    automaton_free(automaton);
}

Test(delete_eps, could_be_cursed_double_epsilon)
{
    Automaton * automaton = automaton_from_daut("automaton/cursed.daut", 4);

    automaton_delete_epsilon_tr(automaton);

    LinkedList * list;
    State * target = *(State **)array_get(automaton->states, 3);

    list = get_matrix_elt(automaton, 0, 'a', 0);
    _assert_list_content(list, 1, target);
    list = get_matrix_elt(automaton, 0, 'b', 0);
    _assert_list_content(list, 1, target);

    list = get_matrix_elt(automaton, 1, 'a', 0);
    _assert_list_content(list, 1, target);
    list = get_matrix_elt(automaton, 1, 'b', 0);
    _assert_list_content(list, 1, target);

    list = get_matrix_elt(automaton, 2, 'a', 0);
    _assert_list_content(list, 1, target);
    list = get_matrix_elt(automaton, 2, 'b', 0);
    _assert_list_content(list, 1, target);

    automaton_free(automaton);
}

Test(delete_eps, entries_redirection)
{
    Automaton * automaton = automaton_from_daut("automaton/simple_3_states.daut", 3);

    automaton_delete_epsilon_tr(automaton);

    cr_assert_eq(automaton->starting_states->size, 2);
 
    State * test = *(State **)array_get(automaton->starting_states, 1);

    cr_assert_eq(test->id, 1);

    automaton_free(automaton);
}

Test(delete_eps, terminal_redirection)
{
    Automaton * automaton = automaton_from_daut("automaton/multiple_check_delete.daut", 6);

    automaton_delete_epsilon_tr(automaton);

    State * test = *(State **)array_get(automaton->states, 1);
    cr_assert_eq(test->terminal, 1);

    automaton_free(automaton);
}

/*
    Application hehehe
*/

Test(delete_eps, after_easy_thompson)
{
    Array *arr = tokenize("ab*");
    BinTree *b = parse_symbols(arr);
    Automaton *aut = thompson(b);
    Automaton * automaton = automaton_from_daut("automaton/res.daut", 6);
    
    automaton_delete_epsilon_tr(aut);
    
    automaton->lookup_used++;

    ASSERT_AUTOMATON_EQ(automaton, aut);

    automaton_free(aut);
    automaton_free(automaton);
    array_free(arr);
    bintree_free(b);
}
