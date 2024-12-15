#pragma once

#include "advent/advent_types.h"

namespace day_fourteen_internal
{
	ResultType p1_a(int num_steps);
}


template <int NUM_STEPS>
ResultType day_fourteen_p1_a()
{
	return day_fourteen_internal::p1_a(NUM_STEPS);
}

ResultType day_fourteen_p1_b();

ResultType advent_fourteen_p1();
ResultType advent_fourteen_p2();
