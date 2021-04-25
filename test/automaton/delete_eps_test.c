#include <criterion/criterion.h>
#include <stdarg.h>

#include "automaton/automaton.h"
#include "automaton/thompson.h"
#include "datatypes/bin_tree.h"
#include "parsing/parsing.h"
#include "automaton/delete_eps.h"

#define ASSERT_AUTOMATON_EQ(a1, a2) _assert_automaton_eq(__LINE__, a1, a2)

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

void _assert_automaton_eq(size_t line, Automaton *a1, Automaton *a2)
{
    // Test starting states
    cr_assert_eq(a1->starting_states->size, a2->starting_states->size,
                 "First automaton has %zu starting states, "
                 "second has %zu (line %zu)",
                 a1->starting_states->size, a2->starting_states->size, line);
    for (size_t i = 0; i < a1->starting_states->size; i++)
    {
        State *s1 = *(State **)array_get(a1->starting_states, i);
        int found = 0;
        arr_foreach(State *, s2, a2->starting_states)
            if (s2->id == s1->id)
            {
                found = 1;
                break;
            }
        cr_assert(found, "First automaton has state %zu as a starting state,"
                      " second hasn't (line %zu)", s1->id, line);
    }
    cr_assert_eq(a1->states->size, a2->states->size,
                 "First automaton has %zu states, second has %zu (line %zu)",
                 a1->states->size, a2->states->size, line);

    size_t terminal1 = 0;
    size_t terminal2 = 0;
    arr_foreach(State *, s1, a1->states)
        terminal1 += s1->terminal;
    arr_foreach(State *, s2, a2->states)
        terminal2 += s2->terminal;
    cr_assert_eq(terminal1, terminal2,
                 "First automaton has %zu terminal states, "
                 "second has %zu (line %zu)",
                 terminal1, terminal2, line);
    for (size_t i = 0; i < a1->states->size; i++)
    {
        State *s1 = *(State **)array_get(a1->states, i);
        if (!s1->terminal)
            continue;
        int found = 0;
        arr_foreach(State *, s2, a2->states)
            if (s2->terminal && s2->id == s1->id)
            {
                found = 1;
                break;
            }
        cr_assert(found, "First automaton has state %zu as a terminal state,"
                         " second hasn't (line %zu)", s1->id, line);
    }

    cr_assert_eq(a1->size, a2->size,
                 "First automaton has size %zu, second has size %zu (line %zu)",
                 a1->size, a2->size, line);

    cr_assert_eq(a1->lookup_used, a2->lookup_used,
                 "First table has %d different states, "
                 "second has width %d (line %zu)",
                 a1->lookup_used, a2->lookup_used, line);

    for (Letter c = 0; c < a1->lookup_used; c++)
        for (size_t src = 0; src < a1->size; src++)
        {
            int is_epsilon = src == 0;
            LinkedList *list1 = get_matrix_elt(a1, src, c, is_epsilon);
            LinkedList *list2 = get_matrix_elt(a2, src, c, is_epsilon);
            if (list1 == NULL || list2 == NULL)
                cr_assert_eq(list1, list2, "%p != %p", list1, list2);
            else
            {
                list1 = list1->next;
                list2 = list2->next;
            }
            while (list1 != NULL && list2 != NULL)
            {
                State *s1 = *(State **)list1->data;
                State *s2 = *(State **)list2->data;
                cr_assert_eq(s1->id, s2->id);
                cr_assert_eq(s1->terminal, s2->terminal);

                list1 = list1->next;
                list2 = list2->next;
            }
            cr_assert_eq(list1, list2);
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
