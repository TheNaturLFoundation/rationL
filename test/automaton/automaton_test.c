#include <criterion/criterion.h>
#include <stdio.h>
#include <criterion/redirect.h>
#include <criterion/internal/assert.h>

#include "automaton/automaton.h"
#include "automaton/thompson.h"
#include "datatypes/bin_tree.h"
#include "parsing/parsing.h"


/*
Internal tests
*/

Test(automaton_internal, count_digits)
{
    cr_assert_eq(_digit_count(0), 1);
    cr_assert_eq(_digit_count(100), 3);
    cr_assert_eq(_digit_count(589741), 6);
}

/*
    Initialization tests
*/

Test(automaton, empty_init_dimens)
{
    Automaton *automaton = Automaton(1, 0);
    cr_assert_eq(automaton->transition_table, NULL);
    cr_assert_eq(automaton->size, 0);
    cr_assert_eq(automaton->capacity, 1);
    cr_assert_eq(automaton->lookup_used, 0);
	for(int i = 0; i < 257; i++)
	{
		cr_assert_eq(automaton->lookup_table[i], -1);
	}
    automaton_free(automaton);
}


Test(automaton, empty_init)
{
    Automaton *automaton = Automaton(2, 1);
    for (size_t i = 0; i < automaton->transition_table->width; i++)
        cr_assert_eq(matrix_get(automaton->transition_table, i, 1), NULL);
    automaton_free(automaton);
}


/*
    automaton_add_state tests
*/

Test(automaton, add_order_update)
{
    Automaton *automaton = Automaton(1, 0);
    cr_assert_eq(automaton->size, 0);
    State *to_add = State(0);
    automaton_add_state(automaton, to_add, 0);
    cr_assert_eq(automaton->size, 1);
    automaton_free(automaton);
}

Test(automaton, add_classic_state)
{
    Automaton *automaton = Automaton(1, 0);
    State *to_add = State(0);
    automaton_add_state(automaton, to_add, 0);
    cr_assert_eq(to_add->id, 0);
    automaton_free(automaton);
}

Test(automaton, add_classic_state_states)
{
    
    //Checks if the state is correctly appended to the array of
    //states

    Automaton *automaton = Automaton(1, 0);
    State *to_add = State(0);
    automaton_add_state(automaton, to_add, 0);
    State **s = array_get(automaton->states, 0);
    cr_assert_eq(*s, to_add);
    automaton_free(automaton);
}

Test(automaton, add_starting_state)
{
    //Checks if the entry state is correctly added to the
    //starting_states array of automaton

    Automaton *automaton = Automaton(1, 0);
    State *to_add = State(0);
    automaton_add_state(automaton, to_add, 1);
    State **elt = array_get(automaton->starting_states, 0);
    cr_assert_eq(*elt, to_add, "got %p expected %p\n", (void *)elt,
                 (void *)to_add);
    automaton_free(automaton);
}

Test(automaton, add_multpile)
{
    size_t entries = 0;
    Automaton *automaton = Automaton(100, 0);
    State *s;
    for (size_t i = 0; i < 100; i++)
    {
        s = State(0);
        if (i % 10 == 0)
        {
            automaton_add_state(automaton, s, 1);
            entries++;
        }
        else
        {
            automaton_add_state(automaton, s, 0);
        }
    }
    cr_assert_eq(automaton->size, 100);
    cr_assert_eq(automaton->states->size, 100);
    cr_assert_eq(automaton->starting_states->size, entries);
    automaton_free(automaton);
}

Test(automaton, automaton_add_state_overbooked)
{
    Automaton * automaton = Automaton(2, 1);
    State * s = State(0);
    automaton_add_state(automaton, s, 0);
    s = State(0);
    automaton_add_state(automaton, s, 0);
    s = State(0);
    automaton_add_state(automaton, s, 0);

    cr_assert_eq(automaton->capacity, 3);
    cr_assert_eq(automaton->transition_table->height, 3);
    cr_assert_eq(automaton->transition_table->width, 1);
    automaton_free(automaton);
}

/*
    Add Transition tests:
*/

Test(automaton, automaton_add_transition_update_lookup_noEpsilon)
{
    Automaton * automaton = Automaton(2,1);
    State * s1 = State(0);
    State * s2 = State(0);
    
    automaton_add_state(automaton, s1, 0);
    automaton_add_state(automaton, s2, 0);
    
    automaton_add_transition(automaton, s1, s2, 'A', 0);

    cr_assert_eq(automaton->lookup_table['A'], 0);
    cr_assert_eq(automaton->lookup_used, 1);
    
    for(size_t i = 0; i < NUMBER_OF_SYMB; i++)
    {
        if(i != 'A')
        {
            cr_assert_eq(automaton->lookup_table[i], -1);
        }
    }

    automaton_free(automaton);
}

Test(automaton, automaton_add_transition_update_lookup_Epsilon)
{
    Automaton * automaton = Automaton(2,1);
    State * s1 = State(0);
    State * s2 = State(0);
    
    automaton_add_state(automaton, s1, 0);
    automaton_add_state(automaton, s2, 0);
    
    automaton_add_transition(automaton, s1, s2, 'A', 1);

    cr_assert_eq(automaton->lookup_table[EPSILON_INDEX], 0);
    cr_assert_eq(automaton->lookup_used, 1);

    for(size_t i = 0; i < NUMBER_OF_SYMB; i++)
    {
        if(i != EPSILON_INDEX)
        {
            cr_assert_eq(automaton->lookup_table[i], -1);
        }
    }

    automaton_free(automaton);    
}

Test(automaton, automaton_add_multiple_same_value)
{
    Automaton * automaton = Automaton(4, NUMBER_OF_SYMB);
 
    State * s1 = State(0);
    State * s2 = State(0);
    State * s3 = State(0);
    State * s4 = State(0);

    automaton_add_state(automaton, s1, 0);
    automaton_add_state(automaton, s2, 0);
    automaton_add_state(automaton, s3, 0);
    automaton_add_state(automaton, s4, 0);

    automaton_add_transition(automaton, s1, s2, 'A', 0);
    
    automaton_add_transition(automaton, s2, s4, 'A', 0);
    cr_assert_eq(automaton->lookup_used, 1);

    automaton_add_transition(automaton, s1, s2, 'A', 0);
    cr_assert_eq(automaton->lookup_used, 1);

    automaton_free(automaton);

}

Test(automaton, automaton_add_transition_updata_lookup_multiple)
{
    Automaton * automaton = Automaton(4, NUMBER_OF_SYMB);
 
    State * s1 = State(0);
    State * s2 = State(0);
    State * s3 = State(0);
    State * s4 = State(0);

    automaton_add_state(automaton, s1, 0);
    automaton_add_state(automaton, s2, 0);
    automaton_add_state(automaton, s3, 0);
    automaton_add_state(automaton, s4, 0);

    for (size_t i = 0; i < NUMBER_OF_SYMB; i++)
    {
        if (i == EPSILON_INDEX)
        {
            automaton_add_transition(automaton, s1, s2, ' ', 1);
        }
        else if ((i % 2) == 0)
        {
            automaton_add_transition(automaton, s1, s2, i, 0);
        }
        else
        {
            automaton_add_transition(automaton, s3, s4, i, 0);
        }
    }

    for (size_t i = 0; i < NUMBER_OF_SYMB; i++)
    {
        cr_assert_eq(automaton->lookup_table[i], i,
                     "Got %d but expected %lu\n", automaton->lookup_table[i],
                     i);
    }

    automaton_free(automaton);
}

Test(automaton, add_transition_no_epsilion2)
{
    Automaton *automaton = Automaton(2, 2);
	State *s1 = State(0);
    State *s2 = State(0);
    
	automaton_add_state(automaton, s1, 0);
    automaton_add_state(automaton, s2, 0);
    automaton_add_transition(automaton, s2, s1, 'A', 0);

    int i = automaton->lookup_table['A'];
	
    LinkedList *list = matrix_get(automaton->transition_table, i, s2->id);
    LinkedList *elt = list->next;
    State *target = *(State **)elt->data;

    cr_assert_eq(elt->next, NULL, "got %p expected NULL\n", (void *)elt->next);
    cr_assert_eq(target->id, s1->id, "got %zu expected %zu\n", target->id,
                 s1->id);

    automaton_free(automaton);
}


Test(automaton, add_transition_epsilon)
{
    Automaton *automaton = Automaton(2, 2);
    State *s1 = State(0);
    State *s2 = State(0);
    automaton_add_state(automaton, s1, 0);
    automaton_add_state(automaton, s2, 0);
    automaton_add_transition(automaton, s1, s2, 'A', 1);
	
    int i_epsilon = automaton->lookup_table[EPSILON_INDEX];

	// Epsilon transition
    LinkedList *list = matrix_get(automaton->transition_table, i_epsilon, s1->id);
    LinkedList *elt = list->next;
    State *target = *(State **)elt->data;

    cr_assert_eq(elt->next, NULL, "got %p expected NULL\n", (void *)elt->next);
    cr_assert_eq(target->id, s2->id, "got %zu expected %zu\n", target->id,
                 s2->id);

    automaton_free(automaton);
}

Test(automaton, add_multiple_tr)
{
    Automaton *automaton = Automaton(500, 2);
    State *s;
    State *s4vj;
    State **state_collector;

    for (size_t i = 0; i < 500; i++)
    {
        s = State(0);
        automaton_add_state(automaton, s, 0);
    }
    state_collector = array_get(automaton->states, 42);
    s = *state_collector;
    cr_assert_eq(s->id, 42, "got %zu, expected 42", s->id);
    for (size_t i = 0; i < 500; i++)
    {
        state_collector = array_get(automaton->states, i);
        s4vj = *state_collector;
        automaton_add_transition(automaton, s, s4vj, 'A', 0);
    }
    int i_A = automaton->lookup_table['A'];
    
	LinkedList *list = matrix_get(automaton->transition_table, i_A, s->id);
    size_t list_size = 0;
    list_foreach(State *, _s, list) list_size++;
    cr_assert_eq(list_size, 500, "got %zu, expected 500", list_size);

    for (size_t i = 0; i < 500; i == s->id - 1 ? i += 2 : i++)
    {
        list = matrix_get(automaton->transition_table, i_A, i);
        cr_assert(list_empty(list), "list isn't empty!");
    }

    automaton_free(automaton);
}

Test(automaton, automaton_add_transition_only_targetted)
{
    Automaton * automaton = Automaton(2, 5);
    
    State * s1 = State(0);
    State * s2 = State(0);

    automaton_add_state(automaton, s1, 0);
    automaton_add_state(automaton, s2, 0);

    automaton_add_transition(automaton, s1, s2, 'K', 0);

    Matrix * mat = automaton->transition_table;
    int index = automaton->lookup_table['K'];

    for(size_t x = 0; x < mat->width; x++)
    {
        for(size_t y = 0; y < mat->height; y++)
        {
            if(x == index && y == s1->id)
            {
                cr_assert_neq(matrix_get(mat, x, y), NULL);
            }
            else
            {
                cr_assert_eq(matrix_get(mat, x, y), NULL);
            }
        }
    }

    automaton_free(automaton);
}

/*
    get_matrix_elt tests:
*/

Test(automaton, get_matrix_elt_test1)
{
	Automaton * automaton = Automaton(2, 3);
	State *s1 = State(0);
    State *s2 = State(0);
    automaton_add_state(automaton, s1, 0);
    automaton_add_state(automaton, s2, 0);
    automaton_add_transition(automaton, s1, s2, 'A', 0);
	
	size_t i = s1->id;

	LinkedList * list1 = get_matrix_elt(automaton, i, 'A', 0);
	LinkedList * list2 = get_matrix_elt(automaton, i, 'A', 1);
	LinkedList * list3 = get_matrix_elt(automaton, i, 'G', 0);

	cr_assert_neq(list1, NULL);
	cr_assert_eq(list2, NULL);
	cr_assert_eq(list3, NULL);

	automaton_free(automaton);
}

Test(automaton, get_matrix_elt_epsilon_is_lit)
{
	Automaton * automaton = Automaton(2, 12);
	State *s1 = State(0);
    State *s2 = State(0);
    automaton_add_state(automaton, s1, 0);
    automaton_add_state(automaton, s2, 0);
    
	automaton_add_transition(automaton, s1, s2, 'S', 1);
	automaton_add_transition(automaton, s1, s2, '4', 1);
	automaton_add_transition(automaton, s1, s2, 'V', 1);
	automaton_add_transition(automaton, s1, s2, 'J', 1);
	
	automaton_add_transition(automaton, s1, s2, 'I', 1);
	automaton_add_transition(automaton, s1, s2, 'S', 1);

	automaton_add_transition(automaton, s1, s2, 'L', 1);
	automaton_add_transition(automaton, s1, s2, 'I', 1);
	automaton_add_transition(automaton, s1, s2, 'T', 1);
	
	size_t i = s1->id;

	LinkedList * list = get_matrix_elt(automaton, i, 'S', 1);
	cr_assert_neq(list, NULL);
	LinkedList * list1 = get_matrix_elt(automaton, i, 'M', 1);
	cr_assert_neq(list1, NULL);
	cr_assert_eq(list1, list);

	automaton_free(automaton);	
}

/*
    automaton_remove_transition tests:
*/

Test(automaton, automaton_remove_transition_succes_check_lookup_and_ret)
{
    Automaton *automaton = Automaton(2, 1);
    State *s1 = State(0);
    State *s2 = State(0);
    automaton_add_state(automaton, s1, 0);
    automaton_add_state(automaton, s2, 0);
    automaton_add_transition(automaton, s1, s2, 'A', 0);

    int ret = automaton_remove_transition(automaton, s1, s2, 'A', 0);

	int index = automaton->lookup_table['A'];
	cr_assert_eq(index, 0);
    cr_assert_eq(automaton->lookup_used, 1);
    cr_assert_eq(ret, 0);

    automaton_free(automaton);
}

Test(automaton, automaton_remove_transion_test_list_is_deleted)
{
    Automaton *automaton = Automaton(2, 2);
    State *s1 = State(0);
    State *s2 = State(1);
    automaton_add_state(automaton, s1, 0);
    automaton_add_state(automaton, s2, 0);

    automaton_add_transition(automaton, s1, s2, 'A', 0);

    int ret = automaton_remove_transition(automaton, s1, s2, 'A', 0);

    LinkedList * list = get_matrix_elt(automaton, s1->id, 'A', 0);
    cr_assert_eq(list, NULL);

    automaton_free(automaton);
}

Test(automaton, automaton_remove_test_list_is_deleted_epsilon)
{
    Automaton *automaton = Automaton(2, 2);
    State *s1 = State(0);
    State *s2 = State(1);
    automaton_add_state(automaton, s1, 0);
    automaton_add_state(automaton, s2, 0);

    automaton_add_transition(automaton, s1, s2, ' ', 1);

    int ret = automaton_remove_transition(automaton, s1, s2, 's', 1);

    LinkedList * list = get_matrix_elt(automaton, s1->id, 'k', 1);
    cr_assert_eq(list, NULL);

    automaton_free(automaton);
}

Test(automaton, automaton_remove_transition_check_list_with_more_than_one)
{
    Automaton *automaton = Automaton(2, 2);
    State *s1 = State(0);
    State *s2 = State(1);
    automaton_add_state(automaton, s1, 0);
    automaton_add_state(automaton, s2, 0);
    automaton_add_transition(automaton, s1, s2, ' ', 1);
    automaton_add_transition(automaton, s1, s2, 'A', 1);

    int ret = automaton_remove_transition(automaton, s1, s2, 's', 1);

    LinkedList * list = get_matrix_elt(automaton, s1->id, 'a', 1);
    State * dst = *(State **)list->next->data;
    cr_assert_neq(list, NULL);
    cr_assert_eq(dst, s2);
    automaton_free(automaton);
}

Test(automaton, automaton_remove_transition_fail1)
{
    Automaton *automaton = Automaton(2, 1);
    State *s1 = State(0);
    State *s2 = State(1);
    automaton_add_state(automaton, s1, 0);
    automaton_add_state(automaton, s2, 0);

    int ret = automaton_remove_transition(automaton, s1, s2, 'A', 1);

    cr_assert_eq(ret, 1);

    automaton_free(automaton);
}

Test(automaton, automaton_remove_transition_fail2)
{
    Automaton *automaton = Automaton(2, 1);
    State *s1 = State(0);
    State *s2 = State(1);
    automaton_add_state(automaton, s1, 0);
    automaton_add_state(automaton, s2, 0);

    int ret = automaton_remove_transition(automaton, s1, s2, 'A', 0);

    cr_assert_eq(ret, 1);

    automaton_free(automaton);
}

Test(automaton, automaton_remove_transition_fail3)
{
    Automaton *automaton = Automaton(2, 1);
    State *s1 = State(0);
    State *s2 = State(1);
    automaton_add_state(automaton, s1, 0);
    automaton_add_state(automaton, s2, 0);
    automaton_add_transition(automaton, s2, s1, 'L', 0);

    int ret = automaton_remove_transition(automaton, s1, s2, 'A', 0);

    cr_assert_eq(ret, 1);

    automaton_free(automaton);
}

Test(automaton, automaton_remove_transition_frees_from_entering)
{
    Automaton *automaton = Automaton(2, 1);
    State *s1 = State(0);
    State *s2 = State(0);
    automaton_add_state(automaton, s1, 0);
    automaton_add_state(automaton, s2, 0);
    automaton_add_transition(automaton, s2, s1, 'L', 0);

    automaton_mark_entering(automaton, s1, s2, 'L', 0, 5);

    automaton_remove_transition(automaton, s1, s2, 'L', 0);

    cr_assert_eq(automaton->entering_transitions->size, 0);

    automaton_free(automaton);
}

Test(automaton, automaton_remove_transition_frees_from_leaving)
{
    Automaton *automaton = Automaton(2, 1);
    State *s1 = State(0);
    State *s2 = State(0);
    automaton_add_state(automaton, s1, 0);
    automaton_add_state(automaton, s2, 0);
    automaton_add_transition(automaton, s2, s1, 'L', 0);

    automaton_mark_leaving(automaton, s1, s2, 'L', 0, 2);

    automaton_remove_transition(automaton, s1, s2, 'L', 0);

    cr_assert_eq(automaton->leaving_transitions->size, 0);

    automaton_free(automaton);
}

Test(automaton, automaton_remove_transition_frees_from_entering_and_leaving)
{
    Automaton *automaton = Automaton(2, 1);
    State *s1 = State(0);
    State *s2 = State(0);
    automaton_add_state(automaton, s1, 0);
    automaton_add_state(automaton, s2, 0);
    automaton_add_transition(automaton, s2, s1, 'L', 0);

    automaton_mark_entering(automaton, s1, s2, 'L', 0, 1);
    automaton_mark_leaving(automaton, s1, s2, 'L', 0, 1);

    automaton_remove_transition(automaton, s1, s2, 'L', 0);

    cr_assert_eq(automaton->entering_transitions->size, 0);
    cr_assert_eq(automaton->leaving_transitions->size, 0);

    automaton_free(automaton);
}

/*
    automaton_remove_transition test
*/

Test(automaton, automaton_remove_state_no_entry)
{
    Automaton *automaton = Automaton(2, 0);
    State *s1 = State(0);
    State *s2 = State(0);
    automaton_add_state(automaton, s1, 0);
    automaton_add_state(automaton, s2, 1);

    automaton_remove_state(automaton, s1);

    cr_assert_eq(automaton->size, 1, "got %lu, expected 1\n", automaton->size);
    cr_assert_eq(automaton->states->size, 1);
    cr_assert_eq(automaton->starting_states->size, 1);

    automaton_free(automaton);
}


Test(automaton, automaton_remove_state_entry)
{
    Automaton *automaton = Automaton(2, 0);
    State *s1 = State(0);
    State *s2 = State(0);
    automaton_add_state(automaton, s1, 0);
    automaton_add_state(automaton, s2, 1);

    automaton_remove_state(automaton, s2);

    cr_assert_eq(automaton->size, 1);
    cr_assert_eq(automaton->states->size, 1);
    cr_assert_eq(automaton->starting_states->size, 0);

    automaton_free(automaton);
}

Test(automaton, automaton_remove_state_upgrade)
{
    Automaton *automaton = Automaton(4, 1);
    State *s1 = State(0);
    State *s2 = State(0);
    State *s3 = State(1);
    State *s4vj = State(0);

    automaton_add_state(automaton, s1, 0);
    automaton_add_state(automaton, s2, 1);
    automaton_add_state(automaton, s3, 0);
    automaton_add_state(automaton, s4vj, 1);

    automaton_add_transition(automaton, s2, s1, 'C', 0);

    automaton_remove_state(automaton, s1);

    cr_assert_eq(s2->id, 0);
    cr_assert_eq(s3->id, 1);
    cr_assert_eq(s4vj->id, 2);

    automaton_free(automaton);
}

Test(automaton, automaton_remove_state_transitions_checking)
{
    Automaton *automaton = Automaton(4, 3);
    State *s1 = State(0);
    State *s2 = State(0);
    State *s3 = State(1);
    State *s4vj = State(0);

    automaton_add_state(automaton, s1, 0);
    automaton_add_state(automaton, s2, 1);
    automaton_add_state(automaton, s3, 0);
    automaton_add_state(automaton, s4vj, 1);

    automaton_add_transition(automaton, s3, s4vj, 'D', 0);
    automaton_add_transition(automaton, s3, s2, ' ', 1);
    
    automaton_add_transition(automaton, s2, s1, 'C', 0);
    automaton_add_transition(automaton, s3, s1, ' ', 1);

    automaton_remove_state(automaton, s1);

    for (size_t c = 0; c < automaton->transition_table->width; c++)
    {
        LinkedList *list = matrix_get(automaton->transition_table, c, s2->id);
        cr_assert_eq(list, NULL, "got %p but expected NULL", list);
    }

    LinkedList *list = get_matrix_elt(automaton, s3->id, ' ', 1);
    cr_assert_neq(list, NULL);

    State *target = *(State **)list->next->data;
    cr_assert_eq(target, s2);
    cr_assert_eq(list->next->next, NULL);

    automaton_free(automaton);
}

Test(automaton, automaton_remove_state_write_elt_deleted)
{
    Automaton *automaton = Automaton(4, 3);
    State *s1 = State(0);
    State *s2 = State(0);
    State *s3 = State(1);
    State *s4vj = State(0);

    automaton_add_state(automaton, s1, 0);
    automaton_add_state(automaton, s2, 1);
    automaton_add_state(automaton, s3, 0);
    automaton_add_state(automaton, s4vj, 1);

    automaton_add_transition(automaton, s3, s4vj, 'D', 1);
    automaton_add_transition(automaton, s3, s2, ' ', 1);
    
    automaton_add_transition(automaton, s2, s1, 'C', 0);
    automaton_add_transition(automaton, s3, s1, ' ', 1);

    automaton_remove_state(automaton, s2);



    LinkedList *list = get_matrix_elt(automaton, s3->id, ' ', 1);
    cr_assert_neq(list, NULL);

    State *target = *(State **)list->next->data;
    cr_assert_eq(target, s4vj);
    target = *(State **)list->next->next->data;
    cr_assert_eq(target, s1);
    cr_assert_eq(list->next->next->next, NULL);

    automaton_free(automaton);
}

Test(automaton, automaton_remove_state_check_matrix_size_changed)
{
    Automaton *automaton = Automaton(2, 1);
    State *s1 = State(0);
    State *s2 = State(0);
    automaton_add_state(automaton, s1, 0);
    automaton_add_state(automaton, s2, 1);

    size_t former_matrix_width = automaton->transition_table->width;
    size_t former_matrix_height = automaton->transition_table->height;

    automaton_remove_state(automaton, s1);

    cr_assert_eq(former_matrix_width, automaton->transition_table->width);
    cr_assert_eq(former_matrix_height - 1,
        automaton->transition_table->height);


    automaton_free(automaton);
}

Test(automaton, automaton_remove_state_check_matrix_size_changed_oversized)
{
    Automaton *automaton = Automaton(200, 1);
    State *s1 = State(0);
    State *s2 = State(0);
    automaton_add_state(automaton, s1, 0);
    automaton_add_state(automaton, s2, 1);

    size_t former_matrix_width = automaton->transition_table->width;
    size_t former_matrix_height = automaton->transition_table->height;

    automaton_remove_state(automaton, s1);

    cr_assert_eq(former_matrix_width, automaton->transition_table->width);
    cr_assert_eq(former_matrix_height - 1,
        automaton->transition_table->height);


    automaton_free(automaton);
}

Test(automaton, automaton_remove_entry_marked_as_entering)
{
    Automaton *automaton = Automaton(2, 1);
    State *s1 = State(0);
    State *s2 = State(0);

    automaton_add_state(automaton, s1, 1);
    automaton_add_state(automaton, s2, 0);

    automaton_mark_entering(automaton, NULL, s1, '0', 1, 4);
    Transition tr = {0, s1->id + 1, 0, 1};

    automaton_remove_state(automaton, s1);

    cr_assert_eq(map_get(automaton->entering_transitions, &tr), NULL);

    automaton_free(automaton);
}

Test(automaton, automaton_remove_entry_marked_as_entering_multiple)
{
    Automaton *automaton = Automaton(2, 1);
    State *s1 = State(0);
    State *s2 = State(0);

    automaton_add_state(automaton, s1, 1);
    automaton_add_state(automaton, s2, 0);

    automaton_mark_entering(automaton, NULL, s1, '0', 1, 4);
    automaton_mark_entering(automaton, s2, s1, '0', 1, 4);

    Transition tr = {0, s1->id + 1, 0, 1};

    automaton_remove_state(automaton, s1);

    cr_assert_eq(map_get(automaton->entering_transitions, &tr), NULL);
    cr_assert_eq(automaton->entering_transitions->size, 1);

    automaton_free(automaton);
}

/*
    BONUS
*/

Test(automaton, add_after_remove)
{
    //if it does not crash, it works hehe
    Automaton *automaton = Automaton(2, 0);
    State *s1 = State(0);
    State *s2 = State(0);
    automaton_add_state(automaton, s1, 0);
    automaton_add_state(automaton, s2, 1);

    automaton_remove_state(automaton, s1);

    s1 = State(0);
    automaton_add_state(automaton, s1, 0);
    cr_assert_eq(1, 1);
    automaton_free(automaton);
}

/*
    is_state_entry tests
*/
Test(automaton, state_is_entry)
{
    Automaton *automaton = Automaton(4, 1);
    
    State *s1 = State(0);
    State *s2 = State(0);
    State *s3 = State(1);
    State *s4vj = State(0);

    automaton_add_state(automaton, s1, 0);
    automaton_add_state(automaton, s2, 0);
    automaton_add_state(automaton, s3, 1);
    automaton_add_state(automaton, s4vj, 0);

    cr_assert_eq(state_is_entry(automaton, s1), 0);
    cr_assert_eq(state_is_entry(automaton, s2), 0);
    cr_assert_eq(state_is_entry(automaton, s3), 1);
    cr_assert_eq(state_is_entry(automaton, s4vj), 0);

    automaton_free(automaton);

}

/*
mark_entering tests
*/

Test(automaton, mark_entering_basic)
{
    Automaton *automaton = Automaton(2, 1);
    
    State *s1 = State(0);
    State *s2 = State(0);

    automaton_add_state(automaton, s1, 0);
    automaton_add_state(automaton, s2, 0);

    size_t g1 = 1;

    automaton_add_transition(automaton, s1, s2, 'A', 0);
    automaton_mark_entering(automaton, s1, s2, 'A', 0, g1);

    Transition tr = {s1->id + 1, s2->id + 1, 'A', 0};
    
    Set * res_set = *(Set **)map_get(automaton->entering_transitions, &tr);

    cr_assert_eq(res_set->size, 1, "Expected 1 element inside the set but got: %lu",
        res_set->size);
    
    size_t * set_content = (size_t *)map_get(res_set, &g1);
    
    cr_assert_neq(set_content, NULL);

    automaton_free(automaton);
}

Test(automaton, mark_entering_epsilon)
{
    Automaton *automaton = Automaton(2, 1);
    
    State *s1 = State(0);
    State *s2 = State(0);

    automaton_add_state(automaton, s1, 0);
    automaton_add_state(automaton, s2, 0);

    size_t g1 = 1;

    automaton_add_transition(automaton, s1, s2, 'A', 1);
    automaton_mark_entering(automaton, s1, s2, 'A', 1, g1);

    Transition tr = {s1->id + 1, s2->id + 1, 'A', 1};
    Set * res_set = *(Set **)map_get(automaton->entering_transitions, &tr);

    cr_assert_eq(res_set->size, 1, "Expected 1 element inside the set but got: %lu",
        res_set->size);
    
    size_t * set_content = (size_t *)map_get(res_set, &g1);
    
    cr_assert_neq(set_content, NULL);

    tr.is_epsilon = 0;

    cr_assert_eq(map_get(automaton->entering_transitions, &tr), NULL);
    
    automaton_free(automaton);
}

Test(automaton, mark_entering_multiple_groups)
{
    Automaton *automaton = Automaton(2, 1);
    
    State *s1 = State(0);
    State *s2 = State(0);

    automaton_add_state(automaton, s1, 0);
    automaton_add_state(automaton, s2, 0);

    size_t g1 = 1;
    size_t g2 = 69;

    automaton_add_transition(automaton, s1, s2, 'A', 0);
    automaton_mark_entering(automaton, s1, s2, 'A', 0, g1);
    automaton_mark_entering(automaton, s1, s2, 'A', 0, g2);

    Transition tr = {s1->id + 1, s2->id + 1, 'A', 0};

    Set * res_set = *(Set **)map_get(automaton->entering_transitions, &tr);    
    cr_assert_eq(res_set->size, 2);

    cr_assert_neq(map_get(res_set, &g1), NULL);
    cr_assert_neq(map_get(res_set, &g2), NULL);

    automaton_free(automaton);
}

Test(automaton, mark_entering_entry_state)
{
    Automaton * automaton = Automaton(2, 1);
    State *s1 = State(0);
    State *s2 = State(0);

    automaton_add_state(automaton, s1, 1);
    automaton_add_state(automaton, s2, 0);

    automaton_mark_entering(automaton, NULL, s1, '0', 0, 4);

    Transition tr = {0, s1->id + 1, 0, 1};
    
    Set * res_set = *(Set **)map_get(automaton->entering_transitions, &tr);

    cr_assert_eq(automaton->entering_transitions->size, 1);
    cr_assert_eq(res_set->size, 1);

    automaton_free(automaton);
}

/*
    mark_leaving test
*/

Test(automaton, marking_leaving_basic)
{
    Automaton *automaton = Automaton(2, 1);
    
    State *s1 = State(0);
    State *s2 = State(0);

    automaton_add_state(automaton, s1, 0);
    automaton_add_state(automaton, s2, 0);

    size_t g1 = 1;

    automaton_add_transition(automaton, s1, s2, 'A', 0);
    automaton_mark_leaving(automaton, s1, s2, 'A', 0, g1);

    Transition tr = {s1->id + 1, s2->id + 1, 'A', 0};

    size_t res = *(size_t *)map_get(automaton->leaving_transitions, &tr);

    cr_assert_eq(automaton->leaving_transitions->size, 1);
    cr_assert_eq(res, g1);

    automaton_free(automaton);
}

Test(automaton, marking_leaving_epsilon)
{
    Automaton *automaton = Automaton(2, 1);
    
    State *s1 = State(0);
    State *s2 = State(0);

    automaton_add_state(automaton, s1, 0);
    automaton_add_state(automaton, s2, 0);

    size_t g1 = 1;

    automaton_add_transition(automaton, s1, s2, 'A', 1);
    automaton_mark_leaving(automaton, s1, s2, 'A', 1, g1);

    Transition tr = {s1->id + 1, s2->id + 1, 'A', 1};

    size_t res = *(size_t *)map_get(automaton->leaving_transitions, &tr);

    cr_assert_eq(automaton->leaving_transitions->size, 1);
    cr_assert_eq(res, g1);

    automaton_free(automaton);
}

//Need to test the fact that remove states frees it form both maps:
