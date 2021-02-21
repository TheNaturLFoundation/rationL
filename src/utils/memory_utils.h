#pragma once
#include <stddef.h>

static void* safe_malloc(size_t n, unsigned long line);

#define SAFEMALLOC(n) safe_malloc(n, __LINE__)