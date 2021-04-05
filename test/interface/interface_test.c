#include <criterion/criterion.h>
#include "rationl.h"

Test(interface_test, compile)
{
    regex_t regexp = regex_compile("aa*");
    cr_assert_neq(regexp.aut, NULL);
    regex_free(regexp);
}

Test(interface_test, match_email)
{
    regex_t email = regex_compile("\\w+(\\.\\w+)?@\\w+(\\.[a-z]{2,})+");

    cr_assert(regex_match(email, "rostan.tabet@gmail.com"));
    cr_assert(regex_match(email, "rostan.tabet@gmail.fr.edu"));
    cr_assert(!regex_match(email, ".tabet@gmail.com"));
    cr_assert(!regex_match(email, "rostan.tabet@gmail.c"));
    cr_assert(!regex_match(email, "rostan.tabet.gmail.com"));
    cr_assert(!regex_match(email, ""));
    regex_free(email);
}
