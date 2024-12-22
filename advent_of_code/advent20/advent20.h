#pragma once

#include "advent/advent_types.h"

namespace advent_20
{
	ResultType p1(std::istream&, int threshold);
}

template <int THRESHOLD>
ResultType day_twenty_p1(std::istream& input) { return advent_20::p1(input, THRESHOLD); }

ResultType advent_twenty_p1();
ResultType advent_twenty_p2();
