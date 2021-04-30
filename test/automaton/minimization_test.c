#include "automaton/minimization.h"
#include <criterion/criterion.h>
#include <criterion/assert.h>
#include "utils.h"

Test(minimization, tmp)
{
    Automaton *a_end = automaton_from_daut(
        TEST_PATH "automaton/determine_daut/a?b*a.daut", 3);

    //automaton_to_dot(a_end);

    Automaton *minimized = minimize(a_end);

    automaton_to_dot(minimized);

    automaton_free(a_end);
    //automaton_free(minimized);
}
