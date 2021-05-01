#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include "automaton/stringify.h"

Test(stringify, a_or_b, .disabled = 1)
{
    Automaton *aut = automaton_from_daut(TEST_PATH "automaton/a+b.daut", 2);

    char *regex = stringify(aut);
    cr_assert_str_eq(regex, "a|b");
    free(regex);

    automaton_free(aut);
}
