#pragma once
#include <stddef.h>
#include <stdlib.h>
#include <err.h>

#ifndef DEBUG
  #define SAFEMALLOC(n) safe_malloc(n, __LINE__)
  #define SAFECALLOC(n, m) safe_calloc(n, m, __LINE__)
  #define SAFEREALLOC(p, n) safe_realloc(p, n, __LINE__)
#else
  #define SAFEMALLOC(n) malloc(n)
  #define SAFECALLOC(n, m) calloc(n, m)
  #define SAFEREALLOC(p, m) realloc(p, m)
#endif

void* safe_malloc(size_t n, unsigned long line);

void *safe_calloc(size_t n, size_t m, unsigned long line);

void *safe_realloc(void *p, size_t n, unsigned long line);
