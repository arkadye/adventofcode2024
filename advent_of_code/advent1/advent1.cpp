#include "advent1.h"
#include "advent/advent_utils.h"

#ifdef FORCE_DAY1DBG
#define DAY1DBG 1
#else
#define ENABLE_DAY1DBG 1
#ifdef NDEBUG
#define DAY1DBG 0
#else
#define DAY1DBG ENABLE_DAY1DBG
#endif
#endif

#if DAY1DBG
	#include <iostream>
#endif

namespace
{
#if DAY1DBG
	std::ostream& log = std::cout;
#else
	struct {	template <typename T> auto& operator<<(const T&) const noexcept { return *this; } } log;
#endif
}

#include <numeric>
namespace
{
	using ValT = int;
	using ValList = std::vector<ValT>;

	std::pair<ValList, ValList> read_vals(std::istream& input)
	{
		constexpr std::size_t RESERVE_AMOUNT = 1000u;
		std::pair<ValList, ValList> result;
		result.first.reserve(RESERVE_AMOUNT);
		result.second.reserve(RESERVE_AMOUNT);

		while (!input.eof())
		{
			AdventCheck(result.first.capacity() == RESERVE_AMOUNT);
			AdventCheck(result.second.capacity() == RESERVE_AMOUNT);
			ValT first{}, second{};
			input >> first >> second;
			result.first.push_back(first);
			result.second.push_back(second);
		}
		AdventCheck(result.first.size() == result.second.size());
		return result;
	}

	int64_t solve_p1(std::istream& input)
	{
		auto [left,right] = read_vals(input);
		auto reduce_op = std::plus<ValT>{};
		auto trans_op = [](ValT l, ValT s) { return std::abs(l - s); };

		stdr::sort(left);
		stdr::sort(right);

		const auto result = std::transform_reduce(
			begin(left), end(left),
			begin(right),
			0,
			reduce_op,
			trans_op);
		return result;
	}
}

namespace
{
	int64_t solve_p2(std::istream& input)
	{
		auto [left, right] = read_vals(input);
		stdr::sort(right);

		auto reduce_op = std::plus<ValT>{};
		auto transform_op = [&right](ValT val)
			{
				const auto [lower, higher] = stdr::equal_range(right, val);
				const auto result = std::distance(lower, higher);
				return val * static_cast<ValT>(result);
			};

		const auto result = std::transform_reduce(
			begin(left), end(left),
			0,
			reduce_op,
			transform_op);
		return result;
	}
}

namespace
{
	std::istringstream testcase_a()
	{
		return std::istringstream{
			"3   4\n"
			"4   3\n"
			"2   5\n"
			"1   3\n"
			"3   9\n"
			"3   3"
		};
	}
}

ResultType day_one_p1_a()
{
	auto input = testcase_a();
	return solve_p1(input);
}

ResultType day_one_p2_a()
{
	auto input = testcase_a();
	return solve_p2(input);
}

ResultType advent_one_p1()
{
	auto input = advent::open_puzzle_input(1);
	return solve_p1(input);
}

ResultType advent_one_p2()
{
	auto input = advent::open_puzzle_input(1);
	return solve_p2(input);
}

#undef DAY1DBG
#undef ENABLE_DAY1DBG
