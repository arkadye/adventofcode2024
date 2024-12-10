#include "advent10.h"
#include "advent/advent_utils.h"

#ifdef FORCE_DAY10DBG
#define DAY10DBG 1
#else
#define ENABLE_DAY10DBG 1
#ifdef NDEBUG
#define DAY10DBG 0
#else
#define DAY10DBG ENABLE_DAY10DBG
#endif
#endif

#if DAY10DBG
	#include <iostream>
#endif

namespace
{
#if DAY10DBG
	std::ostream& log = std::cout;
#else
	struct {	template <typename T> auto& operator<<(const T&) const noexcept { return *this; } } log;
#endif
}

#include "grid.h"
#include "range_contains.h"
#include "to_value.h"
#include "sorted_vector.h"
#include "int_range.h"

namespace
{
	using Height = int8_t;
	using Grid = utils::grid<Height>;
	using Coords = utils::coords;
	using CoordList = utils::small_vector<utils::coords,1>;

	Height char_to_height(char c)
	{
		AdventCheck(std::isdigit(c) || c == '.');
		if (c == '.')
		{
			return std::numeric_limits<char>::min();
		}
		const Height result = c - '0';
		AdventCheck(utils::range_contains_inc(result, 0, 9));
		return result;
	}

	Grid parse_grid(std::istream& input)
	{
		return utils::grid_helpers::build(input, char_to_height);
	}

	CoordList get_trailheads(const Grid& grid)
	{
		return grid.get_all_coordinates(0);
	}

	template <AdventDay day>
	struct PositionsHolder {};

	template <>
	struct PositionsHolder<AdventDay::one>
	{
		utils::sorted_vector<Coords> data;
		void unique() { data.unique(); }
	};

	template <>
	struct PositionsHolder<AdventDay::two>
	{
		utils::small_vector<Coords,1> data;
		void unique() {  }
	};

	template <AdventDay day>
	std::size_t score_trailhead(const Grid& grid, Coords trailhead)
	{
		AdventCheck(grid.is_on_grid(trailhead));
		AdventCheck(grid[trailhead] == 0);

		PositionsHolder<day> current_positions{ { trailhead } };
		PositionsHolder<day> next_positions;

		for (Height height : utils::int_range<Height>{ 1,10 })
		{
			for (Coords pos : current_positions.data)
			{
				for (Coords candidate : pos.neighbours())
				{
					if (grid.is_on_grid(candidate) && grid[candidate] == height)
					{
						next_positions.data.push_back(candidate);
					}
				}
			}
			next_positions.unique();
			next_positions.data.swap(current_positions.data);
			next_positions.data.clear();
		}

		const std::size_t result = current_positions.data.size();
		log << "\nTrailhead at " << trailhead << " score=" << result;
		return result;
	}

	template <AdventDay day>
	std::size_t solve_generic(std::istream& input)
	{
		const Grid grid = parse_grid(input);
		const CoordList trailheads = get_trailheads(grid);
		const std::size_t result = std::transform_reduce(begin(trailheads), end(trailheads), std::size_t{ 0u }, std::plus<std::size_t>{},
			[&grid](Coords th) { return score_trailhead<day>(grid, th); });
		return result;
	}

	std::size_t solve_p1(std::istream& input)
	{
		return solve_generic<AdventDay::one>(input);
	}

	std::size_t solve_p2(std::istream& input)
	{
		return solve_generic<AdventDay::two>(input);
	}
}

ResultType day_ten_p1_a(std::istream& input)
{
	return solve_p1(input);
}

ResultType day_ten_p2_a(std::istream& input)
{
	return solve_p2(input);
}

ResultType advent_ten_p1()
{
	auto input = advent::open_puzzle_input(10);
	return solve_p1(input);
}

ResultType advent_ten_p2()
{
	auto input = advent::open_puzzle_input(10);
	return solve_p2(input);
}

#undef DAY10DBG
#undef ENABLE_DAY10DBG
