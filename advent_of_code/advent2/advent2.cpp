#include "advent2.h"
#include "advent/advent_utils.h"

#ifdef FORCE_DAY2DBG
#define DAY2DBG 1
#else
#define ENABLE_DAY2DBG 1
#ifdef NDEBUG
#define DAY2DBG 0
#else
#define DAY2DBG ENABLE_DAY2DBG
#endif
#endif

#if DAY2DBG
	#include <iostream>
#endif

namespace
{
#if DAY2DBG
	std::ostream& log = std::cout;
#else
	struct {	template <typename T> auto& operator<<(const T&) const noexcept { return *this; } } log;
#endif
}

#include <to_value.h>
#include "string_line_iterator.h"
#include "istream_line_iterator.h"
#include "range_contains.h"
#include "small_vector.h"
#include "int_range.h"

namespace
{
	using ValT = int8_t;
	using Reading = utils::small_vector<ValT, 8>;

	constexpr ValT MIN_SAFE_GAP = 1;
	constexpr ValT MAX_SAFE_GAP = 3;

	enum class Direction
	{
		descending, unset, ascending
	};

	Direction get_direction(ValT first, ValT second)
	{
		const auto diff = first - second;
		if (diff < 0) return Direction::ascending;
		if (diff > 0) return Direction::descending;
		AdventCheck(diff == 0);
		return Direction::unset;
	}

	bool is_pair_in_range(ValT first, ValT second)
	{
		const ValT diff = first - second;
		return utils::range_contains_inc(std::abs(diff), MIN_SAFE_GAP, MAX_SAFE_GAP);
	}

	bool is_pair_safe(ValT first, ValT second, Direction required_dir)
	{
		return get_direction(first, second) == required_dir && is_pair_in_range(first, second);
	}

	template <int REMOVAL_ALLOWED>
	bool is_list_safe(const Reading& line);

	template<>
	bool is_list_safe<0>(const Reading& line)
	{
		AdventCheck(line.size() >= 2u);
		const Direction direction = get_direction(line[0], line[1]);
		for (std::size_t low_i : utils::int_range{ line.size() - 1 })
		{
			const std::size_t high_i = low_i + 1;
			if (!is_pair_safe(line[low_i], line[high_i], direction))
			{
				return false;
			}
		}
		return true;
	}

	template <int REMOVAL_ALLOWED>
	bool is_list_safe(const Reading& line)
	{
		static_assert(REMOVAL_ALLOWED > 0);
		if (is_list_safe<0>(line)) return true;

		for (std::size_t i : utils::int_range{ line.size() })
		{
			Reading cpy = line;
			const auto erase_loc = begin(cpy) + i;
			cpy.erase(erase_loc);
			if (is_list_safe<REMOVAL_ALLOWED - 1>(cpy)) return true;
		}
		return false;
	}

	template <int REMOVAL_ALLOWED>
	bool is_line_safe(std::string_view line)
	{
		using SLI = utils::string_line_iterator;
		Reading reading;
		std::transform(SLI{ line , ' ' }, SLI{}, std::back_inserter(reading), utils::to_value<ValT>);
		return is_list_safe<REMOVAL_ALLOWED>(reading);
	}

	template <int REMOVAL_ALLOWED>
	int64_t solve_generic(std::istream& input)
	{
		using ILI = utils::istream_line_iterator;
		return std::count_if(ILI{ input }, ILI{}, is_line_safe<REMOVAL_ALLOWED>);
	}

	int64_t solve_p1(std::istream& input)
	{
		return solve_generic<0>(input);
	}
}

namespace
{
	int64_t solve_p2(std::istream& input)
	{
		return solve_generic<1>(input);
	}
}

ResultType day_two_p1_a(std::istream& input)
{
	return solve_p1(input);
}

ResultType day_two_p2_a(std::istream& input)
{
	return solve_p2(input);
}

ResultType advent_two_p1()
{
	auto input = advent::open_puzzle_input(2);
	return solve_p1(input);
}

ResultType advent_two_p2()
{
	auto input = advent::open_puzzle_input(2);
	return solve_p2(input);
}

#undef DAY2DBG
#undef ENABLE_DAY2DBG
