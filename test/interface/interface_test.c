#include <criterion/criterion.h>
#include "rationl.h"

Test(interface_test, compile)
{
    reg_t regexp = regex_compile("aa*");
    cr_assert_neq(regexp.aut, NULL);
    regex_free(regexp);
}

Test(interface_test, match_email)
{
    reg_t email = regex_compile("\\w+(\\.\\w+)?@\\w+(\\.[a-z]{2,})+");

    cr_assert(regex_match(email, "rostan.tabet@gmail.com"));
    cr_assert(regex_match(email, "rostan.tabet@gmail.fr.edu"));
    cr_assert(!regex_match(email, ".tabet@gmail.com"));
    cr_assert(!regex_match(email, "rostan.tabet@gmail.c"));
    cr_assert(!regex_match(email, "rostan.tabet.gmail.com"));
    cr_assert(!regex_match(email, ""));
    regex_free(email);
}

Test(interface_test, search_numbers)
{
    reg_t int_reg = regex_compile("\\d+");

    char **groups = NULL;
    size_t n = regex_search(int_reg, "M3h... iT 41n't s0 7rIckY.", &groups);

    cr_assert_eq(n, 4, "expected 4, got %zu", n);
    cr_assert_str_eq(groups[0], "3");
    cr_assert_str_eq(groups[1], "41");
    cr_assert_str_eq(groups[2], "0");
    cr_assert_str_eq(groups[3], "7");

    for (size_t i = 0; i < n; i++)
        free(groups[i]);
    free(groups);

    regex_free(int_reg);
}
