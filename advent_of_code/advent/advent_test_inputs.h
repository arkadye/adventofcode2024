#pragma once

#include <sstream>
#include <string_view>

namespace advent
{
	namespace combine_inputs_internal
	{
		inline std::string impl(std::string_view joiner, std::ostringstream& collector)
		{
			return collector.str();
		}

		template <typename...StringViews>
		inline std::string impl(std::string_view joiner, std::ostringstream& collector, std::string_view next_string, StringViews...remaining)
		{
			collector << joiner << next_string;
			return impl(joiner, collector,remaining...);
		}
	}

	template <std::size_t NUM_NEWLINES, typename...StringViews>
	inline std::string combine_inputs(std::string_view first_string, StringViews...remaining)
	{
		std::ostringstream collector;
		std::string joiner(NUM_NEWLINES, '\n');
		collector << first_string;
		return combine_inputs_internal::impl(joiner, collector, remaining...);
	}
}

/*

Define inline definitions of test inputs here for use in the testcases.

For example:

constexpr const char* TEST_ONE_A = "test1_input";

Use can also combine strings together with combine_inputs, for example, which will also add newlinse automatically using the template parameter to decide how many:

constexpr const char* TEST_ONE_B = "test2";
static const auto TEST_ONE = advent::combine_inputs<2>(TEST_ONE_A, TEST_ONE_B);
will set TEXT_ONE to "test1_input\n\ntest2"

*/

static const char* DAY_ONE_A = R"(3   4
4   3
2   5
1   3
3   9
3   3)";

constexpr const char* DAY_TWO_A = "7 6 4 2 1";
constexpr const char* DAY_TWO_B = "1 2 7 8 9";
constexpr const char* DAY_TWO_C = "9 7 6 2 1";
constexpr const char* DAY_TWO_D = "1 3 2 4 5";
constexpr const char* DAY_TWO_E = "8 6 4 4 1";
constexpr const char* DAY_TWO_F = "1 3 6 7 9";
static const auto DAY_TWO_G = advent::combine_inputs<1>(DAY_TWO_A, DAY_TWO_B, DAY_TWO_C, DAY_TWO_D, DAY_TWO_E, DAY_TWO_F);