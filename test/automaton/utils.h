#pragma once
#include "automaton/automaton.h"

#define ASSERT_AUTOMATON_EQ(a1, a2) assert_automaton_eq_(__LINE__, a1, a2)

void assert_automaton_eq_(size_t line, Automaton *a1, Automaton *a2);