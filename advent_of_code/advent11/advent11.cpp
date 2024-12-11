#include "advent11.h"
#include "advent/advent_utils.h"

#ifdef FORCE_DAY11DBG
#define DAY11DBG 1
#else
#define ENABLE_DAY11DBG 1
#ifdef NDEBUG
#define DAY11DBG 0
#else
#define DAY11DBG ENABLE_DAY11DBG
#endif
#endif

#if DAY11DBG
	#include <iostream>
#endif

namespace
{
#if DAY11DBG
	std::ostream& log = std::cout;
#else
	struct {	template <typename T> auto& operator<<(const T&) const noexcept { return *this; } } log;
#endif
}

#include "count_digits.h"
#include "small_vector.h"
#include "split_string.h"
#include "to_value.h"
#include "int_range.h"

#include <map>

namespace
{
	using Stone = uint64_t;

	utils::small_vector<Stone, 8> parse_input(std::istream& input)
	{
		using It = std::istream_iterator<Stone>;
		utils::small_vector<Stone, 8> result;
		std::copy(It{input}, It{}, std::back_inserter(result));
		return result;
	}

	struct MemoKey
	{
		Stone stone;
		int num_blinks;
		auto operator<=>(const MemoKey&) const noexcept = default;
	};

	using Memo = std::map<MemoKey, std::size_t>;

	utils::small_vector<Stone, 2> apply_blink(Stone in)
	{
		if (in == 0) return { 1 };
		const std::size_t num_digits = utils::count_digits(in);
		if (num_digits % 2 == 0)
		{
			const uint64_t splitter = [sp = num_digits / 2]()
				{
					uint64_t result = 1;
					for (auto i : utils::int_range{ sp })
					{
						result *= 10;
					}
					return result;
				}();
			const uint64_t l = in / splitter;
			const uint64_t r = in % splitter;
			return { l , r };
		}
		return { 2024u * in };
	}

	// Vector is FURTHEST to NEAREST steps
	std::vector<std::size_t> fill_memo(Memo& memo, Stone stone, int num_blinks)
	{
		AdventCheck(num_blinks >= 0);
		std::vector<std::size_t> result;
		if (num_blinks == 0) return result;
		if (memo.contains({ stone,num_blinks }))
		{
			result.reserve(num_blinks);
			utils::int_range blink_range{ num_blinks,0,-1 };
			std::transform(begin(blink_range), end(blink_range), std::back_inserter(result),
				[&memo, stone](int blinks)
				{
					auto entry = memo.find({ stone,blinks });
					AdventCheck(entry != end(memo));
					return entry->second;
				});
			return result;
		}

		const utils::small_vector<Stone, 2> step = apply_blink(stone);
		AdventCheck(!step.empty());
		result = fill_memo(memo, step.front(), num_blinks - 1);
		for (Stone s : stdv::drop(step, 1))
		{
			const std::vector<std::size_t> subresults = fill_memo(memo, s, num_blinks - 1);
			AdventCheck(subresults.size() == result.size());
			std::transform(begin(result), end(result), begin(subresults), begin(result), std::plus<std::size_t>{});
		}
		result.push_back(step.size());

		AdventCheck(std::ssize(result) == num_blinks);
		for (int b : utils::int_range{ 1,num_blinks + 1 })
		{
			const std::size_t result_idx = result.size() - b;
			const std::size_t memo_result = result[result_idx];
			memo.insert(std::pair{ MemoKey{ stone, b }, memo_result });
		}
		return result;
	}

	std::size_t advance_stone(Memo& memo, Stone stone, int num_blinks)
	{
		AdventCheck(num_blinks > 0);
		const auto entry = memo.find({ stone,num_blinks });
		if (entry != end(memo))
		{
			return entry->second;
		}

		const auto all_results = fill_memo(memo, stone, num_blinks);
		AdventCheck(std::ssize(all_results) == num_blinks);
		return all_results.front();
	}

	std::size_t solve_generic(const utils::small_vector<Stone, 8>& input, int num_blinks)
	{
		Memo memo;
		const std::size_t result = std::transform_reduce(begin(input), end(input), std::size_t{ 0u }, std::plus<std::size_t>{},
			[&memo, num_blinks](Stone s)
			{
				return advance_stone(memo, s, num_blinks);
			});
		return result;
	}

	std::size_t solve_generic(std::istream& input, int num_blinks)
	{
		return solve_generic(parse_input(input), num_blinks);
	}
}

ResultType day_eleven::p1_a(std::istream& input, int num_blinks)
{
	return solve_generic(input, num_blinks);
}

ResultType advent_eleven_p1()
{
	auto input = advent::open_puzzle_input(11);
	return solve_generic(input, 25);
}

ResultType advent_eleven_p2()
{
	auto input = advent::open_puzzle_input(11);
	return solve_generic(input, 75);
}

#undef DAY11DBG
#undef ENABLE_DAY11DBG
