#include "advent19.h"
#include "advent/advent_utils.h"

#ifdef FORCE_DAY19DBG
#define DAY19DBG 1
#else
#define ENABLE_DAY19DBG 1
#ifdef NDEBUG
#define DAY19DBG 0
#else
#define DAY19DBG ENABLE_DAY19DBG
#endif
#endif

#if DAY19DBG
	#include <iostream>
#endif

namespace
{
#if DAY19DBG
	std::ostream& log = std::cout;
#else
	struct {	template <typename T> auto& operator<<(const T&) const noexcept { return *this; } } log;
#endif
}

#include "trim_string.h"
#include "string_line_iterator.h"
#include "istream_line_iterator.h"
#include "binary_find.h"
#include "sorted_vector.h"

#include <ranges>
#include <algorithm>
#include <execution>

namespace
{
	using Color = char;
	using Towel = std::string_view;
	using TowelList = std::vector<Towel>;

	void validate_color(char c)
	{
		constexpr std::string_view allowed_colors{ "wubrg" };
		AdventCheck(allowed_colors.contains(c));
	}

	void validate_towel(Towel t)
	{
		stdr::for_each(t, validate_color);
	}

	template <AdventDay day>
	TowelList parse_towels(std::string_view towel_line)
	{
		TowelList result;
		result.reserve(stdr::count(towel_line, ',') + 1);

		stdr::copy(utils::string_line_range{ towel_line, ", " }, std::back_inserter(result));
		stdr::for_each(result, validate_towel);

		stdr::sort(result);
		return result;
	}

	auto get_matching_towel_range(auto&& input_range, std::string_view pattern)
	{
		auto lower_bound = stdr::upper_bound(input_range, pattern.substr(0,1));
		if (lower_bound != begin(input_range))
		{
			std::advance(lower_bound, -1);
		}
		auto upper_bound = stdr::upper_bound(input_range, pattern);
		return stdr::subrange(lower_bound, upper_bound);
	}

	bool dumb_method_for_p1(const TowelList& towels, std::string_view pattern)
	{
		if (pattern.empty()) return true;
		for (Towel towel : towels)
		{
			if (!pattern.starts_with(towel)) continue;
			if (dumb_method_for_p1(towels, pattern.substr(towel.size()))) return true;
		}
		return false;
	}

	// Day 1: return 0 or 1.
	// Day 2: return actual count
	template <AdventDay day>
	int64_t count_ways_to_make_pattern(utils::flat_map<std::string_view,int64_t>& memo, const TowelList& towels, std::string_view pattern)
	{
		// Because ... what?
		AdventCheck(pattern.size() == std::strlen(pattern.data()));

		if (pattern.empty()) return 1;
		{
			auto find_result = memo.find_by_key(pattern);
			if (find_result != end(memo))
			{
				return find_result->second;
			}
		}
		auto recurse = [&towels, &memo](std::string_view pattern_fragment)
			{
				const int64_t result = count_ways_to_make_pattern<day>(memo, towels, pattern_fragment);
				AdventCheck(day == AdventDay::two || result <= 1);
				return result;
			};

		int64_t result = 0;

		const auto candidates = get_matching_towel_range(towels, pattern);
		for (Towel towel : candidates)
		{
			if (!pattern.starts_with(towel)) continue;
			result += recurse(pattern.substr(towel.size()));
			if (day == AdventDay::one && result > 0)
			{
				break;
			}
		}

		memo.insert(std::pair{ pattern,result });
		return result;
	}

	std::string get_towel_input(std::istream& input)
	{
		std::string result;
		std::getline(input, result);
		std::string dummy;
		std::getline(input, dummy);
		AdventCheck(dummy.empty());
		return result;
	}

	template <AdventDay day>
	int64_t solve_generic(std::istream& patterns)
	{
		const std::string towel_line = get_towel_input(patterns);
		const TowelList towels = parse_towels<day>(towel_line);
		std::vector<std::string> pattern_list;
		stdr::transform(utils::istream_line_range{ patterns }, std::back_inserter(pattern_list), [](std::string_view p) {return std::string(p); });

		utils::flat_map<std::string_view, int64_t> memo;

		auto func = [&memo, &towels](std::string_view pattern)
			{
				const int64_t result = count_ways_to_make_pattern<day>(memo, towels, pattern);
				AdventCheck(day == AdventDay::two || bool(result) == dumb_method_for_p1(towels, pattern));
				return result;
			};
		if constexpr (day == AdventDay::one)
		{
			return stdr::count_if(pattern_list, func);
		}
		if constexpr (day == AdventDay::two)
		{
			return std::transform_reduce(begin(pattern_list), end(pattern_list), int64_t{ 0 }, std::plus<int64_t>{}, func);
		}
		AdventUnreachable();
		return -1;
	}

	int64_t solve_p1(std::istream& input)
	{
		return solve_generic<AdventDay::one>(input);
	}

	int64_t solve_p2(std::istream& input)
	{
		return solve_generic<AdventDay::two>(input);
	}
}

ResultType day_nineteen_p1(std::istream& input)
{
	return solve_p1(input);
}

ResultType day_nineteen_p2(std::istream& input)
{
	return solve_p2(input);
}

ResultType advent_nineteen_p1()
{
	auto input = advent::open_puzzle_input(19);
	return solve_p1(input);
}

ResultType advent_nineteen_p2()
{
	auto input = advent::open_puzzle_input(19);
	return solve_p2(input);
}

#undef DAY19DBG
#undef ENABLE_DAY19DBG
