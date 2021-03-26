#include "utils/memory_utils.h"

//LCOV_EXCL_START
void* safe_malloc(size_t n, unsigned long line)
{
    void* p = malloc(n);
    if (!p)
    {
        errx(1, "[%s:%lu] Out of memory (%lu bytes)\n",
                __FILE__, line, (unsigned long)n);
    }
    return p;
}

void *safe_realloc(void *p, size_t n, unsigned long line)
{
    void *new_p = realloc(p, n);

    if (!new_p)
        errx(1, "[%s:%lu] Out of memory (%lu bytes)\n",
             __FILE__, line, (unsigned long)n);

    return new_p;
}
void* safe_calloc(size_t n, size_t m, unsigned long line)
{
    void* p = calloc(n, m);
    if (!p)
        errx(1, "[%s:%lu] Out of memory (%lu bytes)\n",
             __FILE__, line, (unsigned long)n);

    return p;
}
//LCOV_EXCL_STOP
