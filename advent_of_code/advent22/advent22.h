#pragma once

#include "advent/advent_types.h"

namespace advent_22
{
	ResultType p1(std::istream& input, std::size_t steps);
}

template <std::size_t STEPS>
ResultType day_twentytwo_p1(std::istream& input) { return advent_22::p1(input, STEPS); }

ResultType advent_twentytwo_p1();
ResultType advent_twentytwo_p2();
