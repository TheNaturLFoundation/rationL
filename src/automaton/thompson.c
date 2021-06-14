#include "automaton/thompson.h"

#include "automaton/automaton.h"
#include "datatypes/array.h"
#include "datatypes/linked_list.h"
#include "datatypes/map.h"
#include "parsing/lexer.h"
#include "parsing/parsing.h"

int _leaves_self(Automaton * aut, State * src, size_t grp)
{
    Set * set = get_leaving_group(aut, src, NULL, 0, 1);
    if(set != NULL)
        return (map_get(set, &grp) != NULL);
    return 0;
}

void _transfer_leaving_set_to(Automaton * aut, Set * set, State * src, State * dst)
{
    //CAREFULL ONLY WITH EPSILONS
    if(set != NULL)
    {
        map_foreach_key(
            size_t, grp, set,
            {
                automaton_mark_leaving(aut, src, dst, 
                        0, 1, grp);
            }
        )
    }
}

void _transfer_entering_set_to(Automaton * aut, Set * set, State * src, State * dst)
{
    //CAREFULL ONLY WITH EPSILONS
    if(set != NULL)
    {
        map_foreach_key(
            size_t, grp, set,
            {
                automaton_mark_entering(aut, src, dst, 
                        0, 1, grp);
            }
        )
    }
}

void _build_epsilon(Automaton * aut, State * src, State * dst, int src_grp, size_t dst_grp)
{
    automaton_add_transition(aut, src, dst, 'e', 1);
    Map * set;
   // printf("%lu, %d\n", src->id, _leaves_self(aut, src, src_grp));
    if(src_grp != dst_grp)
    {
        if(src_grp > dst_grp)
        {
            set = get_leaving_group(aut, src, NULL, 0, 1);
            _transfer_leaving_set_to(aut, set, src, dst);
        }
        if(src_grp < dst_grp)
        {
            set = get_entering_groups(aut, NULL, dst, 0, 1);
            _transfer_entering_set_to(aut, set, src, dst);
        }
    }
}

void concatenate(Automaton *aut, size_t left_grp, size_t right_grp, size_t current_grp)
{
    State *entry_to_patch = *(State **)array_get(
        aut->starting_states, aut->starting_states->size - 2);
    Set * set;
    for (int i = aut->states->size - 1; i >= 0; i--)
    {
        State *end_to_patch = *(State **)array_get(aut->states, i);
        if (end_to_patch->terminal)
        {
            _build_epsilon(aut, end_to_patch, entry_to_patch, left_grp, right_grp);
            if((left_grp < right_grp) && (current_grp != right_grp) && (current_grp != left_grp))
            {
                set = get_leaving_group(aut, end_to_patch, NULL, 0, 1);
                _transfer_leaving_set_to(aut, set, end_to_patch, entry_to_patch);
            }
            else if(left_grp < right_grp)
            {
                end_to_patch->terminal = 0;
                set = get_leaving_group(aut, end_to_patch, NULL, 0, 1);
                if(set != NULL)
                {
                    State * trg;
                    for (int i = aut->states->size - 1; i >= 0; i--)
                    {
                        trg = *(State **)array_get(aut->states, i);
                        if(trg->terminal != 0)
                            break;
                    }
                    _transfer_leaving_set_to(aut, set, trg, NULL);
                }  
                end_to_patch->terminal = 1; 
            }
            automaton_clear_state_terminal(aut, end_to_patch);
            break;
        }
    }
    automaton_clear_state_entry(aut, aut->starting_states->size - 2);
}

void unite(Automaton *aut, int curr_grp, int first_grp, int second_grp)
{
    State *new_start = State(0);
    State *new_end = State(0);
    int grps[] = {first_grp, second_grp};
    State *first_start = *(State **)array_get(aut->starting_states,
                                              aut->starting_states->size - 1);
    State *second_start = *(State **)array_get(aut->starting_states,
                                               aut->starting_states->size - 2);

    automaton_add_state(aut, new_start, 0);
    automaton_add_state(aut, new_end, 0);
    int count = 0;
    for (int i = aut->states->size - 1; i >= 0; i--)
    {
        State *state = *(State **)array_get(aut->states, i);
        if (state->terminal)
        {
            //automaton_add_transition(aut, state, new_end, 'e', 1);
            _build_epsilon(aut, state, new_end, grps[count], curr_grp);
            automaton_clear_state_terminal(aut, state);
            if (count == 1)
                break;
            else
                count++;
        }
    }
    //automaton_add_transition(aut, new_start, first_start, 'e', 1);
    _build_epsilon(aut, new_start, first_start, curr_grp, first_grp);
    //automaton_add_transition(aut, new_start, second_start, 'e', 1);
    _build_epsilon(aut, new_start, second_start, curr_grp, second_grp);
    if(curr_grp > 0)
    {
        automaton_mark_leaving(aut, new_end, NULL, 0, 1, curr_grp);
        automaton_mark_entering(aut, NULL, new_start, 0, 1, curr_grp);
    }
    automaton_clear_state_entry(aut, aut->starting_states->size - 1);
    automaton_clear_state_entry(aut, aut->starting_states->size - 1);
    array_append(aut->starting_states, &new_start);
    new_end->terminal = 1;
}

void kleene(Automaton *aut)
{
    State *new_start = State(0);
    State *new_end = State(0);
    State *current_start = *(State **)array_get(aut->starting_states,
                                                aut->starting_states->size - 1);
    Set * set;
    automaton_add_state(aut, new_start, 0);
    automaton_add_state(aut, new_end, 0);
    for (int i = aut->states->size - 1; i >= 0; i--)
    {
        State *state = *(State **)array_get(aut->states, i);
        if (state->terminal)
        {
            automaton_add_transition(aut, state, current_start, 'e', 1);
            automaton_add_transition(aut, state, new_end, 'e', 1);
            set = get_leaving_group(aut, state, NULL, 0, 1);
            _transfer_leaving_set_to(aut, set, new_end, NULL);
            automaton_clear_state_terminal(aut, state);
            break;
        }
    }
    automaton_add_transition(aut, new_start, current_start, 'e', 1);
    automaton_add_transition(aut, new_start, new_end, 'e', 1);
    set = get_entering_groups(aut, NULL, current_start, 0, 1);
    _transfer_entering_set_to(aut, set, NULL, new_start);
    automaton_clear_state_entry(aut, aut->starting_states->size - 1);
    array_append(aut->starting_states, &new_start);
    new_end->terminal = 1;
}

void exists(Automaton *aut)
{
    State *new_end = State(0);
    State *current_start = *(State **)array_get(aut->starting_states,
                                                aut->starting_states->size - 1);
    Set * set;
    automaton_add_state(aut, new_end, 0);
    automaton_add_transition(aut, new_end, current_start, 'e', 1);
    for (int i = aut->states->size - 1; i >= 0; i--)
    {
        State *state = *(State **)array_get(aut->states, i);
        if (state->terminal)
        {
            automaton_add_transition(aut, state, new_end, 'e', 1);
            set = get_leaving_group(aut, state, NULL, 0, 1);
            _transfer_leaving_set_to(aut, set, new_end, NULL);
            automaton_clear_state_terminal(aut, state);
            break;
        }
    }
    new_end->terminal = 1;
}

void maybe(Automaton *aut)
{
    State *new_start = State(0);
    State *new_end = State(0);
    State *start = *(State **)array_get(aut->starting_states,
                                        aut->starting_states->size - 1);
    Set * set;
    automaton_add_state(aut, new_start, 0);
    automaton_add_state(aut, new_end, 0);
    for (int i = aut->states->size - 1; i >= 0; i--)
    {
        State *state = *(State **)array_get(aut->states, i);
        if (state->terminal)
        {
            automaton_add_transition(aut, state, new_end, 'e', 1);
            set = get_leaving_group(aut, state, NULL, 0, 1);
            _transfer_leaving_set_to(aut, set, new_end, NULL);
            automaton_clear_state_terminal(aut, state);
            break;
        }
    }
    automaton_add_transition(aut, new_start, start, 'e', 1);
    set = get_entering_groups(aut, NULL, start, 0, 1);
    _transfer_entering_set_to(aut, set, NULL, new_start);
    automaton_clear_state_entry(aut, aut->starting_states->size - 1);
    array_append(aut->starting_states, &new_start);
    new_end->terminal = 1;
    automaton_add_transition(aut, new_start, new_end, 'e', 1);
}

void thompson_recur(BinTree *tree, Automaton *aut)
{
    int left_grp, right_grp, curr_grp;
    if (tree->left == NULL && tree->right == NULL)
    {
        curr_grp = ((Symbol *)(tree->data))->group;
        State *entry_state = State(0);
        State *letter_state = State(1);
        automaton_add_state(aut, entry_state, 1);
        automaton_add_state(aut, letter_state, 0);
        if(curr_grp > 0)
        {
            automaton_mark_leaving(aut, letter_state, NULL, 0, 1, curr_grp);
            automaton_mark_entering(aut, NULL, entry_state, 0, 1,
                curr_grp);
        }
        Symbol sym = *(Symbol *)tree->data;
        if (sym.type == LETTER)
        {
            automaton_add_transition(aut, entry_state, letter_state,
                                     sym.value.letter, 0);
        }
        else
        {
            arr_foreach(Letter, c, sym.value.letters)
            {
                automaton_add_transition(aut, entry_state, letter_state,
                                         c, 0);
            }
        }
        return;
    }

    Operator operator=((Symbol *)tree->data)->value.operator;

    switch (operator)
    {
    case CONCATENATION: {
        thompson_recur(tree->right, aut);
        thompson_recur(tree->left, aut);
        left_grp = ((Symbol *)(tree->left->data))->group;
        right_grp = ((Symbol *)(tree->right->data))->group;
        curr_grp = ((Symbol *)(tree->data))->group;
        concatenate(aut, left_grp, right_grp, curr_grp);
        ((Symbol *)(tree->data))->group = left_grp;
        break;
    }
    case UNION: {
        thompson_recur(tree->left, aut);
        thompson_recur(tree->right, aut);
        left_grp = ((Symbol *)(tree->left->data))->group;
        right_grp = ((Symbol *)(tree->right->data))->group;
        curr_grp = ((Symbol *)(tree->data))->group;
        unite(aut, curr_grp, right_grp, left_grp);
        break;
    }
    case KLEENE_STAR: {
        thompson_recur(tree->left, aut);
        kleene(aut);
        ((Symbol *)(tree->data))->group = ((Symbol *)(tree->left->data))->group;
        break;
    }
    case MAYBE: {
        thompson_recur(tree->left, aut);
        maybe(aut);
        ((Symbol *)(tree->data))->group = ((Symbol *)(tree->left->data))->group;
        break;
    }
    case EXISTS: {
        thompson_recur(tree->left, aut);
        exists(aut);
        ((Symbol *)(tree->data))->group = ((Symbol *)(tree->left->data))->group;
        break;
    }
    }
}

void count_symbols(BinTree *tree, size_t *size, size_t *letter_count)
{
    if (tree->left == NULL && tree->right == NULL)
    {
        Symbol sym = *(Symbol*)tree->data;
        if(sym.type == LETTER)
            *letter_count += 1;
        else
            *letter_count += sym.value.letters->size;
        *size += 1;
    }
    else
    {
        if (((Symbol *)tree->data)->value.operator!= CONCATENATION)
            *size += 1;
    }
    if (tree->left)
        count_symbols(tree->left, size, letter_count);
    if (tree->right)
        count_symbols(tree->right, size, letter_count);
}

Automaton *thompson(BinTree *tree)
{
    if (tree == NULL)
        return NULL;
    size_t size = 0;
    size_t letter_count = 0;
    count_symbols(tree, &size, &letter_count);
    Automaton *aut = Automaton(2 * size,
                               letter_count + 1 > 256 ? 256 : letter_count + 1);
    thompson_recur(tree, aut);
    return aut;
}
