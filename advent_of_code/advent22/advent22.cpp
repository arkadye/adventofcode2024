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
#include "range_contains.h"
#include "small_vector.h"
#include "sorted_vector.h"
#include "comparisons.h"

#include <numeric>
#include <execution>

namespace
{
#if DAY22DBG
	auto policy = std::execution::seq;
#else
	auto policy = std::execution::par_unseq;
#endif
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

	std::vector<Secret> get_all_initial_secrets(std::istream& input)
	{
		std::vector<Secret> secrets;
		secrets.reserve(2000);
		stdr::copy(stdr::istream_view<Secret>{input}, std::back_inserter(secrets));
		return secrets;
	}

	uint64_t solve_p1(std::istream& input, std::size_t steps)
	{
		using SII = std::istream_iterator<Secret>;
		const std::vector<Secret> secrets = get_all_initial_secrets(input);
		return std::transform_reduce(policy, begin(secrets), end(secrets), Secret{0u}, std::plus<Secret>{}, [steps](Secret s) {return randomise(s, steps); });
	}
}

namespace
{
	// Price delta is always in the range [-9,+9], so it can be represented in 5 bits. There's little advantage to 20 bits over 32, though.
	using Price = int8_t;
	using PriceDelta = int8_t;

	struct PriceDeltaSequence
	{
	private:
		uint32_t data;
	public:
		auto operator<=>(const PriceDeltaSequence& other) const noexcept = default;
		PriceDeltaSequence(const PriceDeltaSequence&) = default;
		PriceDeltaSequence() : data{ 0xFFFFFFFF } {}
		void add_delta(PriceDelta next_delta)
		{
			AdventCheck(utils::range_contains_inc(next_delta, -9, +9));
			data = (data << 8) + next_delta + 10;
		}
		bool is_valid() const { return 0xFF000000 && data != 0xFF000000; }
	};

	Price price(Secret s) { return static_cast<Price>(s % 10); }

	using MerchantSummary = utils::flat_map<PriceDeltaSequence, Price>;

	MerchantSummary get_all_prices(Secret initial_secret)
	{
		Secret current_secret = initial_secret;
		Price latest_price = price(current_secret);
		PriceDeltaSequence current_sequence;

		MerchantSummary result;
		for (auto i : utils::int_range{ 2000 })
		{
			current_secret = randomise_step(current_secret);
			const Price new_price = price(current_secret);
			const PriceDelta delta = new_price - latest_price;
			latest_price = new_price;
			current_sequence.add_delta(delta);

			const auto insert_candidate = result.lower_bound_by_key(current_sequence);
			if (current_sequence.is_valid() && ((insert_candidate == end(result)) || (insert_candidate->first != current_sequence)))
			{
				result.insert(insert_candidate, { current_sequence,latest_price });
			}
		}
		return result;
	}

	std::vector<MerchantSummary> get_all_merchant_summaries(std::istream& input)
	{
		const std::vector<Secret> initial_secrets = get_all_initial_secrets(input);
		std::vector<MerchantSummary> result;
		result.resize(initial_secrets.size());
		std::transform(policy, begin(initial_secrets), end(initial_secrets), begin(result), get_all_prices);
		return result;
	}

	utils::sorted_vector<PriceDeltaSequence> get_all_sequences(const std::vector<MerchantSummary>& all_price_sequences)
	{
		const std::size_t total_size = stdr::fold_left(all_price_sequences | stdv::transform(&MerchantSummary::size), 0u, std::plus<std::size_t>{});
		utils::sorted_vector<PriceDeltaSequence> result;
		result.reserve(total_size);
		for (const MerchantSummary& ms : all_price_sequences)
		{
			stdr::transform(ms, std::back_inserter(result), [](MerchantSummary::value_type val) {return val.first; });
		}
		result.unique();
		return result;
	}

	Price get_single_price(const MerchantSummary& ms, PriceDeltaSequence sequence)
	{
		auto result_it = ms.find_by_key(sequence);
		return result_it != end(ms) ? result_it->second : 0;
	}

	int64_t get_combined_price(const std::vector<MerchantSummary>& all_summaries, PriceDeltaSequence sequence)
	{
		return stdr::fold_left(all_summaries | stdv::transform([sequence](const MerchantSummary& ms) {return get_single_price(ms, sequence); }), int64_t{ 0 }, std::plus<int64_t>{});
	}

	int64_t solve_p2(std::istream& input)
	{
		const std::vector<MerchantSummary> all_summaries = get_all_merchant_summaries(input);
		const utils::sorted_vector<PriceDeltaSequence> sequences_to_check = get_all_sequences(all_summaries);

		return std::transform_reduce(policy, begin(sequences_to_check), end(sequences_to_check), int64_t{ 0 }, utils::Larger<int64_t>{}, [&all_summaries](PriceDeltaSequence pds) {return get_combined_price(all_summaries, pds); });
	}
}

ResultType advent_22::p1(std::istream& input, std::size_t steps)
{
	return solve_p1(input, steps);
}

ResultType day_twentytwo_p2(std::istream& input)
{
	return solve_p2(input);
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
