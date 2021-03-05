#include <criterion/criterion.h>
#include <stdio.h>
#include "automaton/automaton.c"

Test(automaton, create_automaton)
{
    Automaton * test_automaton = Automaton();
    cr_assert_eq(0,0);
}