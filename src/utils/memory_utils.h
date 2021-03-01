#pragma once
#include <stddef.h>
#include <stdlib.h>
#include <err.h>

#define SAFEMALLOC(n) safe_malloc(n, __LINE__)

void* safe_malloc(size_t n, unsigned long line);
