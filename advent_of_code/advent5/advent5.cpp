#include "advent5.h"
#include "advent/advent_utils.h"

#ifdef FORCE_DAY5DBG
#define DAY5DBG 1
#else
#define ENABLE_DAY5DBG 0
#ifdef NDEBUG
#define DAY5DBG 0
#else
#define DAY5DBG ENABLE_DAY5DBG
#endif
#endif

#if DAY5DBG
	#include <iostream>
#endif

namespace
{
#if DAY5DBG
	std::ostream& log = std::cout;
#else
	struct {	template <typename T> auto& operator<<(const T&) const noexcept { return *this; } } log;
#endif
}

#include "parse_utils.h"
#include "sorted_vector.h"
#include "to_value.h"
#include "string_line_iterator.h"
#include "istream_block_iterator.h"
#include "comparisons.h"
#include "combine_maps.h"

#include <map>


namespace
{
	constexpr std::size_t EXPECTED_MAX_PRECEDING_RULES = 24;
	constexpr std::size_t EXPECTED_MAX_UPDATE_SIZE = 24;
	
	using PageNum = int8_t;
	using PageSet = utils::sorted_vector<PageNum, std::less<PageNum>, EXPECTED_MAX_PRECEDING_RULES>;
	using RuleSet = std::map<PageNum, PageSet>; // Update is invalid if key comes AFTER any value pages.
	using Rule = RuleSet::value_type; // Update invalid is first comes AFTER any second value.
	using SingleRule = std::pair<PageNum, PageNum>; // First must come before second
	using Update = utils::small_vector<PageNum, EXPECTED_MAX_UPDATE_SIZE>;

	SingleRule parse_single_rule(std::string_view line)
	{
		const auto page_strs = utils::split_string_at_first(line, '|');
		const PageNum first_page = utils::to_value<PageNum>(page_strs.first);
		const PageNum second_page = utils::to_value<PageNum>(page_strs.second);
		return SingleRule{ first_page, second_page };
	}

	RuleSet combine_rulesets(RuleSet a, RuleSet b)
	{
		auto combine_page_lists = [](PageSet a, const PageSet& b)
			{
				auto is_in_b = [&b](PageNum pn) { return b.contains(pn); };
				AdventCheck(stdr::none_of(a, is_in_b));
				a.insert(begin(b), end(b));
				return a;
			};

		return utils::combine_maps(std::move(a), std::move(b), combine_page_lists);
	}

	RuleSet parse_rules(std::string_view rules)
	{
		auto single_line_result = [](std::string_view line)
			{
				const SingleRule rule = parse_single_rule(line);
				return RuleSet{ std::pair{ rule.first, PageSet{ rule.second } } };
			};

		using SLI = utils::string_line_iterator;
		const RuleSet result = std::transform_reduce(SLI{ rules }, SLI{}, RuleSet{}, combine_rulesets, single_line_result);

#if DAY5DBG
		// Report the longest set of rules to help pick config values for optimisation.
		const auto& longest_rule = utils::ranges::max_transform(result, [](const Rule& r) {return r.second.size(); });
		log << "\nLongest rule is " << longest_rule.second.size();
#endif
		return result;
	}

	Update parse_update(std::string_view line)
	{
		using SLI = utils::string_line_iterator;
		Update result;
		std::transform(SLI{ line,',' }, SLI{}, std::back_inserter(result), utils::to_value<PageNum>);
		return result;
	}

	bool is_pair_valid(const RuleSet& rules, PageNum first, PageNum second)
	{
		const auto rule_it = rules.find(second);
		if (rule_it == end(rules)) return true; // No real requiring second to be before anything.
		const bool result = !rule_it->second.contains(first);
		if (!result)
		{
			log << "\n    Rule violation found: " << int{ first } << '|' << int{ second };
		}
		return result;
	}

	std::pair<Update::const_iterator, Update::const_iterator> get_first_invalid_pair(const RuleSet& rules, const Update& update)
	{
		Update::const_iterator first = begin(update);
		Update::const_iterator second = begin(update);
		for (second; second != end(update); ++second)
		{
			for (first = begin(update); first != second; ++first)
			{
				if (!is_pair_valid(rules, *first, *second))
				{
					return std::pair{ first,second };
				}
			}
		}
		return std::pair{ first,second };
	}

	bool is_update_valid(const RuleSet& rules, const Update& update)
	{
		auto [first, second] = get_first_invalid_pair(rules, update);
		return second == end(update);
	}

	PageNum get_update_value(const Update& update)
	{
		AdventCheck(update.size() % 2 == 1u);
		const auto midpoint = std::midpoint(begin(update), end(update));
		return *midpoint;
	}

	template <AdventDay DAY>
	PageNum evaluate_update(const RuleSet& rules, const Update& update);

	template <>
	PageNum evaluate_update<AdventDay::one>(const RuleSet& rules, const Update& update)
	{
		return is_update_valid(rules, update) ? get_update_value(update) : 0;
	}

	std::pair<Update::iterator, Update::iterator> get_first_invalid_pair(const RuleSet& rules, Update& update)
	{
		auto [first, second] = get_first_invalid_pair(rules, const_cast<const Update&>(update));
		return std::pair{ const_cast<Update::iterator>(first), const_cast<Update::iterator>(second) };
	}

	Update fix_update(const RuleSet& rules, Update update)
	{
		auto [first, second] = get_first_invalid_pair(rules, update);
		if (second != end(update))
		{
			std::swap(*first, *second);
			return fix_update(rules, std::move(update));
		}
		return update;
	}

	template <>
	PageNum evaluate_update<AdventDay::two>(const RuleSet& rules, const Update& update)
	{
		if (is_update_valid(rules, update)) return 0;
		auto print_update = [](const Update& u)
			{
				for (PageNum p : u)
				{
					log << ' ' << int{ p };
				}
			};
		const Update fixed = fix_update(rules, update);
		log << "\nFixed update:\n    From:";
		print_update(update);
		log << "\n    To  :";
		print_update(fixed);
		return get_update_value(fixed);
	}

	template <AdventDay DAY>
	PageNum evaluate_line(const RuleSet& rules, std::string_view line)
	{
		log << "\nEvaluating line '" << line << '\'';
		const Update update = parse_update(line);
		const int result = evaluate_update<DAY>(rules, update);
		log << "\n    Result=" << result;
		return result;
	}

	template <AdventDay DAY>
	int evalue_updates(const RuleSet& rules, std::string_view update_block)
	{
		auto reduce = std::plus<int>{};
		auto transform = [&rules](std::string_view line) { return evaluate_line<DAY>(rules, line); };
		using SLI = utils::string_line_iterator;

		const int result = std::transform_reduce(SLI{ update_block }, SLI{}, 0, reduce, transform);
		return result;
	}

	std::size_t get_longest_update(std::string_view update_block)
	{
#if DAY5DBG
		std::size_t result = 0;
		for (std::string_view line : utils::string_line_range{ update_block })
		{
			const Update u = parse_update(line);
			result = std::max(u.size(), result);
		}
		return result;
#else
		return 0u;
#endif
	}

	template <AdventDay DAY>
	int64_t solve_generic(std::istream& input)
	{
		using IBI = utils::istream_block_iterator;
		IBI block_it{ input };
		const RuleSet rules = parse_rules(*block_it);
		log << "\nGot rules ('|' = 'must come before'):";
		for (const Rule& r : rules)
		{
			log << "\n    " << int{ r.first } << " |";
			for (PageNum p : r.second)
			{
				log << ' ' << int{ p };
			}
		}
		++block_it;
		log << "\nLongest update=" << get_longest_update(*block_it);
		const int result = evalue_updates<DAY>(rules, *block_it);
		return result;
	}

	int64_t solve_p1(std::istream& input)
	{
		return solve_generic<AdventDay::one>(input);
	}
}

namespace
{
	int64_t solve_p2(std::istream& input)
	{
		return solve_generic<AdventDay::two>(input);
	}
}

ResultType day_five_p1_a(std::istream& input)
{
	return solve_p1(input);
}

ResultType day_five_p2_a(std::istream& input)
{
	return solve_p2(input);
}

ResultType advent_five_p1()
{
	auto input = advent::open_puzzle_input(5);
	return solve_p1(input);
}

ResultType advent_five_p2()
{
	auto input = advent::open_puzzle_input(5);
	return solve_p2(input);
}

#undef DAY5DBG
#undef ENABLE_DAY5DBG
