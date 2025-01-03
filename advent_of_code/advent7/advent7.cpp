#include "advent7.h"
#include "advent/advent_utils.h"

#ifdef FORCE_DAY7DBG
#define DAY7DBG 1
#else
#define ENABLE_DAY7DBG 0
#ifdef NDEBUG
#define DAY7DBG 0
#else
#define DAY7DBG ENABLE_DAY7DBG
#endif
#endif

#if DAY7DBG
	#include <iostream>
#endif

namespace
{
#if DAY7DBG
	std::ostream& log = std::cout;
#else
	struct {	template <typename T> auto& operator<<(const T&) const noexcept { return *this; } } log;
#endif
}

#include "small_vector.h"
#include "parse_utils.h"
#include "string_line_iterator.h"
#include "istream_line_iterator.h"
#include "to_value.h"
#include "int_range.h"
#include "count_digits.h"

namespace
{
	enum class Operation : uint8_t
	{
		add,
		mul,
		cat
	};

	using TargetType = int64_t;
	using ValueType = int16_t;
	constexpr std::size_t MAX_VALUES = 16;
	using ValueList = utils::small_vector<ValueType, MAX_VALUES>;

	struct Puzzle
	{
		TargetType target = 0;
		ValueList numbers;
	};

	Puzzle parse_puzzle(std::string_view line)
	{
		using SLI = utils::string_line_iterator;
		Puzzle result;
		auto [target_str, values_str] = utils::split_string_at_first(line, ": ");
		result.target = utils::to_value<TargetType>(target_str);
		std::transform(SLI{ values_str,' ' }, SLI{}, std::back_inserter(result.numbers), utils::to_value<ValueType>);
		return result;
	}

	template <AdventDay Day>
	constexpr auto get_ops_list() {}

	template <AdventDay Day>
	bool has_solution_impl(TargetType partial_solution, ValueList::const_reverse_iterator vals_begin, ValueList::const_reverse_iterator vals_end)
	{
		if (vals_begin == vals_end) return (partial_solution == TargetType{ 0 });
		if (partial_solution < 0) return false;

		const ValueType value = *vals_begin;

		// Unconcat.
		if constexpr (Day == AdventDay::two)
		{
			const std::size_t digits = utils::count_digits(value);
			const TargetType mod_arg = static_cast<TargetType>(std::pow(10, digits));
			if (partial_solution > mod_arg)
			{
				const TargetType mod = partial_solution % mod_arg;
				if (mod == value)
				{
					const TargetType candidate = partial_solution / mod_arg;
					const bool solved = has_solution_impl<Day>(candidate, vals_begin + 1, vals_end);
					if (solved) return true;
				}
			}
		}

		// Unmultiply
		if (partial_solution % value == 0)
		{
			const TargetType candidate = partial_solution / value;
			const bool solved = has_solution_impl<Day>(candidate, vals_begin + 1, vals_end);
			if (solved) return true;
		}

		// Unadd
		const TargetType candidate = partial_solution - value;
		const bool solved = has_solution_impl<Day>(candidate, vals_begin + 1, vals_end);
		return solved;
	}

	template <AdventDay Day>
	bool has_solution(const Puzzle& puzzle)
	{
		AdventCheck(!puzzle.numbers.empty());
		return has_solution_impl<Day>(puzzle.target, crbegin(puzzle.numbers), crend(puzzle.numbers));
	}

	template <AdventDay Day>
	TargetType get_line_value(std::string_view line)
	{
		const Puzzle puzzle = parse_puzzle(line);
		return has_solution<Day>(puzzle) ? puzzle.target : 0;
	}

	template <AdventDay Day>
	TargetType solve_generic(std::istream& input)
	{
		using ILI = utils::istream_line_iterator;
		return std::transform_reduce(ILI{ input }, ILI{}, TargetType{ 0 }, std::plus<TargetType>{}, get_line_value<Day>);
	}

	TargetType solve_p1(std::istream& input)
	{
		return solve_generic<AdventDay::one>(input);
	}

	int64_t solve_p2(std::istream& input)
	{
		return solve_generic<AdventDay::two>(input);
	}
}

ResultType day_seven_p1_a(std::istream& input)
{
	return solve_p1(input);
}

ResultType day_seven_p2_a(std::istream& input)
{
	return solve_p2(input);
}

ResultType advent_seven_p1()
{
	auto input = advent::open_puzzle_input(7);
	return solve_p1(input);
}

ResultType advent_seven_p2()
{
	auto input = advent::open_puzzle_input(7);
	return solve_p2(input);
}

#undef DAY7DBG
#undef ENABLE_DAY7DBG
