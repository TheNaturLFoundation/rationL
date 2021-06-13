#include "delete_eps.h"
#include "err.h"

void add_entering_groups_to_src(Automaton * automaton, State * src, State * dst, 
    Letter value, int epsilon, Map * set)
{

}

void add_leaving_group_to_src(Automaton * automaton, State * src, State * dst,
    Letter value, int epsilon, size_t * grp)
{

}

Array * build_pred_lists(Automaton * automaton)
{
    Array * pred_lists = Array(LinkedList *);
    LinkedList * tr_list;
    for(size_t i = 0; i < automaton->size; i++)
    {
        tr_list = LinkedList(Transition);
        array_append(pred_lists, &tr_list);
    }

    Transition tr;
    int index;
    LinkedList * pred;
    arr_foreach(State *, state, automaton->states)
    {
        for(size_t c = 0; c < NUMBER_OF_SYMB; c++)
        {
            index = automaton->lookup_table[c];
            if(index != -1)
            {
                tr_list = matrix_get(automaton->transition_table, index, state->id);
                list_foreach(State *, trg, tr_list)
                {
                    pred = *(LinkedList **)array_get(pred_lists, trg->id);
                    tr = _generate_transition(state, trg, c, c == EPSILON_INDEX);
                    list_push_front(pred, &tr);
                }
            }
        }
    }

    return pred_lists;
}

void free_pred_lists(Array * pred_lists)
{
    for(size_t i = 0; i < pred_lists->size; i++)
    {
        list_free(*(LinkedList **)array_get(pred_lists, i));
    }
    array_free(pred_lists);
}

void add_to_pred_lists(Array * pred_lists, State * src, State * trg, Letter value,
    int epsilon)
{
    Transition tr =  _generate_transition(src, trg, value, epsilon);
    LinkedList * pred = *(LinkedList **)array_get(pred_lists, trg->id);
    list_push_front(pred, &tr);
}

void transfer_all_transitions(Automaton * automaton, State * src, 
    State * new_src, Map * eps_entering, Array * pred_lists)
{
    LinkedList * src_list;
    State * trg;
    LinkedList * new_src_list;
    Letter c;
    Map * tr_entering;
    Map * tr_leaving;

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
                add_to_pred_lists(pred_lists, new_src, trg, c, i == EPSILON_INDEX);
                
                tr_entering = get_entering_groups(automaton, src, trg, c, i == EPSILON_INDEX);
                tr_leaving = get_leaving_group(automaton, src, trg, c, i == EPSILON_INDEX);
                //deal with entering
                if(eps_entering != NULL)
                {
                    map_foreach_key(
                        size_t, grp, eps_entering,
                        {
                            automaton_mark_entering(automaton, new_src, trg,
                                c, i == EPSILON_INDEX, grp);
                        }
                    )
                }

                if(tr_entering != NULL)
                {
                    map_foreach_key(
                        size_t, grp, tr_entering,
                        {
                            automaton_mark_entering(automaton, new_src, trg,
                                c, i == EPSILON_INDEX, grp);
                        }
                    )
                }

                //deal with leaving
                if(tr_leaving != NULL)
                {
                    map_foreach_key(
                        size_t, grp, tr_leaving,
                        {
                            automaton_mark_leaving(automaton, new_src, trg, 
                                c, i == EPSILON_INDEX, grp);
                        }
                    )
                }
                src_list = src_list->next;
            }
        }
    }
}

void automaton_delete_epsilon_tr(Automaton * automaton)
{
    LinkedList * eps_list;
    State * dst;
    Map * entering_set;
    Map * leaving_set;
    Array * pred_lists = build_pred_lists(automaton);
    LinkedList * pred;
    State * aux;
    /*
        We never delete from pred_lists the deleted transitions...
    */
    arr_foreach(State *, s, automaton->states)
    {
        eps_list = get_matrix_elt(automaton, s->id, 'e', 1);
        if(eps_list != NULL)
        {
            eps_list = eps_list->next;
            while(eps_list != NULL)
            {
                dst = *(State **)eps_list->data;
                entering_set = get_entering_groups(automaton, s, dst, 0, 1);
                leaving_set = get_leaving_group(automaton, s, dst, 0, 1);
                pred = *(LinkedList **)array_get(pred_lists, s->id);
                if(leaving_set != NULL)
                {
                    map_foreach_key(
                        size_t, grp, leaving_set,
                        {
                            list_foreach(Transition, tr, pred)
                            {
                                aux = *(State **)array_get(automaton->states, tr.old_src - 1);
                                if(automaton_is_transition(automaton, aux, s, tr.letter, tr.is_epsilon) == 1)
                                {
                                    _mark_tr_to_map(automaton->leaving_transitions,
                                        &tr, grp);
                                }                
                            }
                        }
                        )
                }
                transfer_all_transitions(automaton, dst, s, entering_set, pred_lists);
                eps_list = eps_list->next;
                
                if(dst->terminal)
                {
                    s->terminal = 1;
                }
                
                if(state_is_entry(automaton, s) == 1)
                {
                    if(entering_set != NULL)
                    {
                        map_foreach_key(
                            size_t, grp, entering_set,
                            {
                                automaton_mark_entering(automaton, NULL, dst,
                                    0, 1, grp);
                            }
                        )
                    }
                    if(state_is_entry(automaton, dst) != 1)
                        array_append(automaton->starting_states, &dst);
                }
                automaton_remove_transition(automaton, s, dst, 'e', 1);

            }
        }
    }

    free_pred_lists(pred_lists);
}
