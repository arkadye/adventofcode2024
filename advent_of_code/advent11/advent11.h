#pragma once

#include "advent/advent_types.h"

namespace day_eleven
{
	ResultType p1_a(std::istream& input, int num_blinks);
}

template <int NumBlinks>
inline ResultType day_eleven_p1_a(std::istream& input)
{
	return day_eleven::p1_a(input, NumBlinks);
}

ResultType advent_eleven_p1();
ResultType advent_eleven_p2();
