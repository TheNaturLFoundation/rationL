#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include "automaton/stringify.h"

Test(stringify, a_or_b)
{
    Automaton *aut = automaton_from_daut(TEST_PATH "automaton/a+b.daut", 3);

    char *regex = stringify(aut);
    cr_assert_str_eq(regex, "(?:a|b)");
    free(regex);

    automaton_free(aut);
}

Test(stringify, a_end)
{
    Automaton *aut = automaton_from_daut(
        TEST_PATH "automaton/sum_mod_3.daut", 3);

    char *regex = stringify(aut);
    cr_assert_str_eq(regex, "(?:0*|0*10*1(?:(?:0|10*10*1))*10*)");
    free(regex);

    automaton_free(aut);
}
