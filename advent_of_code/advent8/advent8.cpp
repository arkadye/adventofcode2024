#include "advent8.h"
#include "advent/advent_utils.h"

#ifdef FORCE_DAY8DBG
#define DAY8DBG 1
#else
#define ENABLE_DAY8DBG 0
#ifdef NDEBUG
#define DAY8DBG 0
#else
#define DAY8DBG ENABLE_DAY8DBG
#endif
#endif

#if DAY8DBG
	#include <iostream>
#endif

namespace
{
#if DAY8DBG
	std::ostream& log = std::cout;
#else
	struct {	template <typename T> auto& operator<<(const T&) const noexcept { return *this; } } log;
#endif
}

#include "coords.h"
#include "small_vector.h"
#include "sorted_vector.h"
#include "range_contains.h"
#include "istream_line_iterator.h"
#include <map>

namespace
{
	using ElemType = int8_t;
	using Coords = utils::basic_coords<ElemType>;

	template <std::size_t SIZE>
	using CoordsList = utils::small_vector<Coords, SIZE>;
	using LocationList = CoordsList<4>;

	using Antenna = char;

	bool is_antenna(char c) { return std::isalnum(c); }
	void validate_antenna(Antenna a) { AdventCheck(is_antenna(a)); }

	template <AdventDay DAY>
	class Roof
	{
		std::map<Antenna, LocationList> antennae;
		utils::sorted_vector<Coords> antinodes;
		Coords limit;

		auto get_antinode_locations(Coords, Coords) const;

		void get_antinode_locations(utils::sorted_vector<Coords>& partial_result, LocationList::const_iterator ref_start, LocationList::const_iterator loc_to_test) const
		{
			const Coords me = *loc_to_test;
			for (auto it = ref_start; it != loc_to_test; ++it)
			{
				const auto antinodes = get_antinode_locations(*it, me);
				stdr::copy(antinodes, std::back_inserter(partial_result));
			}
		}

		void get_antinode_locations(utils::sorted_vector<Coords>& partial_result, const LocationList& locs) const
		{
			for (auto it = begin(locs); it != end(locs); ++it)
			{
				get_antinode_locations(partial_result, begin(locs), it);
			}
		}

	public:
		void add_antenna(Antenna a, Coords loc)
		{
			const auto list_it = antennae.find(a);
			if (list_it == end(antennae))
			{
				antennae.insert(std::pair{ a,LocationList{loc} });
				return;
			}

			LocationList& locs = list_it->second;
			AdventCheck(stdr::find(locs, loc) == end(locs));
			locs.push_back(loc);
		}

		void set_limit(Coords l)
		{
			limit = l;
		}

		bool is_on_roof(Coords loc) const
		{
			const bool result = utils::range_contains_exc(loc.x, 0, limit.x) && utils::range_contains_exc(loc.y, 0, limit.y);
			log << "\n        Location " << loc << " is " << (result ? "" : "not ") << "on the roof";
			return result;
		}

		utils::sorted_vector<Coords> get_antinode_locations() const
		{
			utils::sorted_vector<Coords> result;
			for (const auto [type, list] : antennae)
			{
				log << "\nGetting antinodes for '" << type << '\'';
				const auto prev = result.size();
				get_antinode_locations(result, list);
				const auto new_antinodes = result.size() - prev;
				log << "\n    Found " << new_antinodes << " new antinodes for '" << type << '\'';
			}
			const auto prev = result.size();
			
			result.unique();
			const auto new_total = result.size();
			log << "\nFound " << prev << " antinodes in total. Removed " << prev - new_total << " duplicates. Result=" << new_total;
			return result;
		}

		void dump_info() const
		{
			std::size_t most_types = 0u;
			for (const auto& [a, list] : antennae)
			{
				const std::size_t s = list.size();
				most_types = std::max(most_types, s);
				log << "\nType '" << a << "': Num=" << s;
			}
			log << "\nBiggest has " << most_types << " antennae";
			log << "\nRoof limit is " << limit;
		}
	};

	template <>
	auto Roof<AdventDay::one>::get_antinode_locations(Coords a, Coords b) const
	{
		AdventCheck(is_on_roof(a));
		AdventCheck(is_on_roof(b));
		AdventCheck(a != b);
		const Coords offset = b - a;
		std::array<Coords, 2> candidates{ a - offset , b + offset };
		log << "\n    Checking " << a << " and " << b << " got antinode candidates at " << candidates[0] << " and " << candidates[1];
		CoordsList<2> result;
		stdr::copy_if(candidates, std::back_inserter(result), [this](Coords c) {return is_on_roof(c); });
		log << ". Added " << result.size() << " antinode(s).";
		return result;
	}

	template <>
	auto Roof<AdventDay::two>::get_antinode_locations(Coords a, Coords b) const
	{
		AdventCheck(is_on_roof(a));
		AdventCheck(is_on_roof(b));
		AdventCheck(a != b);
		const Coords offset = b - a;
		CoordsList<64> result;
		auto add_antinodes = [this, offset, &result](Coords start, int stride)
			{
				int mul = 0;
				while (true)
				{
					const Coords test_loc = start + (offset * mul);
					if (is_on_roof(test_loc))
					{
						result.push_back(test_loc);
					}
					else
					{
						return;
					}
					mul += stride;
				}
				AdventUnreachable();
			};
		add_antinodes(a, -1);
		add_antinodes(b, +1);
		return result;
	}

	template <AdventDay day>
	Roof<day> parse_roof(std::istream& input)
	{
		Roof<day> result;
		Coords loc{ 0,0 };
		for (std::string_view line : utils::istream_line_range{ input })
		{
			loc.x = 0;
			for (char c : line)
			{
				if (is_antenna(c))
				{
					result.add_antenna(c, loc);
				}
				++loc.x;
			}
			++loc.y;
		}
		result.set_limit(loc);
		result.dump_info();
		return result;
	}

	template <AdventDay day>
	uint64_t solve_generic(std::istream& input)
	{
		const Roof<day> roof = parse_roof<day>(input);
		const auto antinode_locs = roof.get_antinode_locations();
		return antinode_locs.size();
	}

	uint64_t solve_p1(std::istream& input)
	{
		return solve_generic<AdventDay::one>(input);
	}
}

namespace
{
	uint64_t solve_p2(std::istream& input)
	{
		return solve_generic<AdventDay::two>(input);
	}
}

ResultType day_eight_p1_a(std::istream& input)
{
	return solve_p1(input);
}

ResultType day_eight_p2_a(std::istream& input)
{
	return solve_p2(input);
}

ResultType advent_eight_p1()
{
	auto input = advent::open_puzzle_input(8);
	return solve_p1(input);
}

ResultType advent_eight_p2()
{
	auto input = advent::open_puzzle_input(8);
	return solve_p2(input);
}

#undef DAY8DBG
#undef ENABLE_DAY8DBG
