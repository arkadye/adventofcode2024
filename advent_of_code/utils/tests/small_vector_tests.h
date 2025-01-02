#pragma once

#include "utils/tests/utils_tests.h"

DECLARE_UTILS_TEST("small_vector - insert non-trivial while in stack range", small_vector_insert_non_trivial_at_location_stack, "[aa,bb,cc]");
DECLARE_UTILS_TEST("small_vector - insert non-trivial while on heap without reallocating", small_vector_insert_at_location_heap_no_realloc, "[aa,bb,cc]");
DECLARE_UTILS_TEST("small_vector - insert non-trivial while on heap with reallocating", small_vector_insert_at_location_heap_with_realloc, "[aa,bb,cc]");