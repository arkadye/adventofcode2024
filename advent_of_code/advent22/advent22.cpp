#include "advent22.h"
#include "advent/advent_utils.h"

#ifdef FORCE_DAY22DBG
#define DAY22DBG 1
#else
#define ENABLE_DAY22DBG 0
#ifdef NDEBUG
#define DAY22DBG 0
#else
#define DAY22DBG ENABLE_DAY22DBG
#endif
#endif

#if DAY22DBG
	#include <iostream>
#endif

namespace
{
#if DAY22DBG
	std::ostream& log = std::cout;
#else
	struct {	template <typename T> auto& operator<<(const T&) const noexcept { return *this; } } log;
#endif
}

#include "int_range.h"

#include <numeric>
#include <execution>

namespace
{
	using Secret = uint64_t;

	Secret mix_and_prune(Secret in, uint64_t mixer)
	{
		constexpr uint64_t PRUNE_MAGIC = 16777216;
		AdventCheck(in < PRUNE_MAGIC);

		const uint64_t mix = in ^ mixer;
		const uint64_t prune = mix % PRUNE_MAGIC;
		return prune;
	}

	Secret randomise_step(Secret in)
	{
		const Secret step64 = mix_and_prune(in, in * 64);
		const Secret step32 = mix_and_prune(step64, step64 / 32);
		const Secret step2048 = mix_and_prune(step32, step32 * 2048);
		return step2048;
	}

	Secret randomise(Secret in, std::size_t steps)
	{
		return stdr::fold_left(utils::int_range{ steps }, in, [](const Secret in, std::size_t step) { step; return randomise_step(in); });
	}

	uint64_t solve_p1(std::istream& input, std::size_t steps)
	{
#if DAY22DBG
		auto policy = std::execution::seq;
#else
		auto policy = std::execution::par_unseq;
#endif
		using SII = std::istream_iterator<Secret>;
		std::vector<Secret> secrets;
		secrets.reserve(2000);
		std::copy(SII{ input }, SII{}, std::back_inserter(secrets));
		return std::transform_reduce(policy, begin(secrets), end(secrets), Secret{0u}, std::plus<Secret>{}, [steps](Secret s) {return randomise(s, steps); });
	}
}

namespace
{
	int64_t solve_p2(std::istream& input)
	{
		return 0;
	}
}

ResultType advent_22::p1(std::istream& input, std::size_t steps)
{
	return solve_p1(input, steps);
}

ResultType advent_twentytwo_p1()
{
	auto input = advent::open_puzzle_input(22);
	return solve_p1(input, 2000u);
}

ResultType advent_twentytwo_p2()
{
	auto input = advent::open_puzzle_input(22);
	return solve_p2(input);
}

#undef DAY22DBG
#undef ENABLE_DAY22DBG
