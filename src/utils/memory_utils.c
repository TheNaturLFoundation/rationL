#include "utils/memory_utils.h"

static void* safe_malloc(size_t n, unsigned long line)
{
    void* p = malloc(n);
    if (!p)
    {
        errx(1, "[%s:%ul]Out of memory(%ul bytes)\n",
                __FILE__, line, (unsigned long)n);
    }
    return p;
}