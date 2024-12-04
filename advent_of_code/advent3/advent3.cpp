#include "advent3.h"
#include "advent/advent_utils.h"

#ifdef FORCE_DAY3DBG
#define DAY3DBG 1
#else
#define ENABLE_DAY3DBG 1
#ifdef NDEBUG
#define DAY3DBG 0
#else
#define DAY3DBG ENABLE_DAY3DBG
#endif
#endif

#if DAY3DBG
	#include <iostream>
#endif

namespace
{
#if DAY3DBG
	std::ostream& log = std::cout;
#else
	struct {	template <typename T> auto& operator<<(const T&) const noexcept { return *this; } } log;
#endif
}

#include "parse_utils.h"
#include "to_value.h"
#include "small_vector.h"

#include <array>
#include <int_range.h>

namespace
{
	// A placeholder to tell my function to parse a number (or to try to)
	struct Number {};

	bool get_pattern_impl(std::string_view& input, utils::small_vector<int, 2>& result) { return true; }

	template <typename...Args>
	bool get_pattern_impl(std::string_view& input, utils::small_vector<int, 2>& result, std::string_view target, Args...args)
	{
		if (input.size() < target.size()) return false;
		if (!input.starts_with(target)) return false;
		input.remove_prefix(target.size());
		return get_pattern_impl(input, result, args...);
	}

	template <typename...Args>
	bool get_pattern_impl(std::string_view& input, utils::small_vector<int,2>& result, Number target, Args...args)
	{
		if (input.empty()) return false;
		if (!std::isdigit(input.front())) return false;
		const std::size_t num_end = input.find_first_not_of("0123456789");
		const std::string_view digits = input.substr(0, num_end);
		input.remove_prefix(num_end);
		result.push_back(utils::to_value<int>(digits));
		return get_pattern_impl(input, result, args...);
	}

	template <typename...Args>
	auto get_pattern(std::string_view input, Args... args)
	{
		using ArrayT = utils::small_vector<int, 2>;
		using RetType = std::optional<ArrayT>;
		ArrayT result;
		const bool success = get_pattern_impl(input, result, args...);
		RetType ret;
		if (success)
		{
			ret = result;
		}
		return ret;
	}

	auto get_multiplication(std::string_view input)
	{
		return get_pattern(input, "mul(", Number{}, ",", Number{}, ")");
	}

	int get_multiples(std::string_view input, bool allow_disable_instructions)
	{
		int result = 0;
		while (!input.empty())
		{
			if (allow_disable_instructions)
			{
				if (input.starts_with("don't()"))
				{
					const auto find_result = input.find("do()");
					if (find_result >= input.size())
					{
						return result;
					}
					input.remove_prefix(find_result);
					continue;
				}
			}

			const auto mul = get_multiplication(input);
			if (mul.has_value())
			{
				const auto& m = mul.value();
				result += (m[0] * m[1]);
			}

			input.remove_prefix(1);
		}
		return result;
	}

	std::string read_input(std::istream& input)
	{
		using CharIt = std::istreambuf_iterator<char>;
		return std::string{ CharIt{input},CharIt{} };
	}

	int get_multiples(std::istream& input, bool allow_disable_instructions)
	{
		const std::string sInput = read_input(input);
		return get_multiples(sInput, allow_disable_instructions);
	}

	int64_t solve_p1(std::istream& input)
	{
		return get_multiples(input, false);
	}
}

namespace
{
	int64_t solve_p2(std::istream& input)
	{
		return get_multiples(input, true);
	}
}

ResultType day_three_p1_a(std::istream& input)
{
	return solve_p1(input);
}

ResultType day_three_p2_a(std::istream& input)
{
	return solve_p2(input);
}

ResultType advent_three_p1()
{
	auto input = advent::open_puzzle_input(3);
	return solve_p1(input);
}

ResultType advent_three_p2()
{
	auto input = advent::open_puzzle_input(3);
	return solve_p2(input);
}

#undef DAY3DBG
#undef ENABLE_DAY3DBG
