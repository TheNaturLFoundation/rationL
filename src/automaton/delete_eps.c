#include "delete_eps.h"
#include "err.h"

void transfer_all_transitions(Automaton * automaton, State * src, State * new_src)
{
    LinkedList * src_list;
    State * trg;
    LinkedList * new_src_list;
    Letter c;

    for(size_t i = 0; i < NUMBER_OF_SYMB; i++)
    {
        c = (i == EPSILON_INDEX) ? 'A' : i;
        src_list = get_matrix_elt(automaton, src->id, c, i == EPSILON_INDEX);
        new_src_list = get_matrix_elt(automaton, new_src->id, c, i == EPSILON_INDEX);
        if(src_list != NULL)
        {
            if(new_src_list == NULL)
            {
                new_src_list = LinkedList(State *);
                matrix_set(automaton->transition_table, automaton->lookup_table[i],
                    new_src->id, new_src_list);    
            }

            while(src_list->next != NULL)
            {
                trg = *(State **)src_list->next->data;
                if (!list_push_back(new_src_list, &trg))
                {   
                    errx(
                        EXIT_FAILURE, // LCOV_EXCL_LINE
                        "Unable to append to the list at address %p letter = %c", // LCOV_EXCL_LINE
                        (void *)new_src_list, (int)i);
                }
                src_list = src_list->next;
            }
        }
    }
}

void automaton_delete_epsilon_tr(Automaton * automaton)
{
    //TODO
    //I guess you will be able to do it in place...
    LinkedList * eps_list;
    State * dst;

    arr_foreach(State *, s, automaton->states)
    {
        eps_list = get_matrix_elt(automaton, s->id, 'e', 1);
        if(eps_list != NULL)
        {
            eps_list = eps_list->next;
            while(eps_list != NULL)
            {
                dst = *(State **)eps_list->data;

                transfer_all_transitions(automaton, dst, s);
                eps_list = eps_list->next;
                automaton_remove_transition(automaton, s, dst, 'e', 1);
                
                if(dst->terminal)
                {
                    s->terminal = 1;
                }
                
                if(state_is_entry(automaton, s) == 1)
                {
                   array_append(automaton->starting_states, &dst);
                }

            }
        }
    }
}
