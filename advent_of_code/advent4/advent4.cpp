#include "advent4.h"
#include "advent/advent_utils.h"

#ifdef FORCE_DAY4DBG
#define DAY4DBG 1
#else
#define ENABLE_DAY4DBG 0
#ifdef NDEBUG
#define DAY4DBG 0
#else
#define DAY4DBG ENABLE_DAY4DBG
#endif
#endif

#if DAY4DBG
	#include <iostream>
#endif

namespace
{
#if DAY4DBG
	std::ostream& log = std::cout;
#else
	struct {	template <typename T> auto& operator<<(const T&) const noexcept { return *this; } } log;
#endif
}

#include "coords.h"
#include "grid.h"
#include "coords_iterators.h"

namespace
{
	using Grid = utils::grid<char>;

	Grid read_grid(std::istream& input)
	{
		auto conv = [](char c) { return c; };
		Grid result;
		result.build_from_stream(input, conv);
		return result;
	}

	bool has_string_at_point_in_dir(const Grid& grid, std::string_view target, utils::coords location, utils::coords direction)
	{
		if (target.empty()) return true;
		if (!grid.is_on_grid(location)) return false;
		if (grid[location] != target.front()) return false;

		return has_string_at_point_in_dir(grid, target.substr(1), location + direction, direction);
	}

	int count_string_at_point(const Grid& grid, std::string_view target, utils::coords location)
	{
		AdventCheck(grid.is_on_grid(location));
		AdventCheck(!target.empty());

		const auto direction_steps = utils::coords{ 0,0 }.neighbours_plus_diag();
		const auto result = stdr::count_if(direction_steps, [&grid, target, location](utils::coords dir)
			{
				const bool result = has_string_at_point_in_dir(grid, target, location, dir);
				if (result)
				{
					log << "\nFound '" << target << "' at loc=(" << location << ") & dir=(" << dir << ')';
				}
				return result;
			});
		AdventCheck(result <= static_cast<decltype(result)>(std::numeric_limits<int>::max()));
		return static_cast<int>(result);
	}

	int count_string(const Grid& grid, std::string_view target)
	{
		const auto elems = utils::coords_iterators::elem_range{ grid.bottom_left(), grid.top_right() + utils::coords{1,1} };
		return std::transform_reduce(begin(elems), end(elems), 0, std::plus<int>{}, [&grid, target](utils::coords loc) {return count_string_at_point(grid, target, loc); });
	}

	int64_t solve_p1(std::istream& input)
	{
		const Grid grid = read_grid(input);
		return count_string(grid, "XMAS");
	}
}

namespace
{

	bool has_cross_at_point(const Grid& grid, std::string_view target, utils::coords location)
	{
		AdventCheck(target.size() == 3u);
		AdventCheck(grid.is_on_grid(location));
		const char front = target[0];
		const char focus = target[1];
		const char back = target[2];
		if (grid[location] != focus) return 0;

		const std::array<utils::coords, 4> direction_steps =
		{
			utils::coords{ 1, 1},
			utils::coords{ 1,-1},
			utils::coords{-1, 1},
			utils::coords{-1,-1}
		};

		const auto lines_found = stdr::count_if(direction_steps, [&grid, target, location](utils::coords dir)
			{
				return has_string_at_point_in_dir(grid, target, location - dir, dir);
			});
		const bool result = lines_found >= 2;
		if (result)
		{
			log << "\nFound crosses at (" << location << "). Num=" << result
				<< "\n    " << grid[location + utils::coords{ -1,1 }] << grid[location + utils::coords{ 0,1 }] << grid[location + utils::coords{ 1,1 }]
				<< "\n    " << grid[location + utils::coords{ -1,0 }] << grid[location + utils::coords{ 0,0 }] << grid[location + utils::coords{ 1,0 }]
				<< "\n    " << grid[location + utils::coords{ -1,-1 }] << grid[location + utils::coords{ 0,-1 }] << grid[location + utils::coords{ 1,-1 }];
		}
		return result;
	}

	int64_t count_crosses(const Grid& grid, std::string_view target)
	{
		const auto elems = utils::coords_iterators::elem_range{ grid.bottom_left() + utils::coords{1,1}, grid.top_right() };
		return std::count_if(begin(elems), end(elems), [&grid, target](utils::coords loc) {return has_cross_at_point(grid, target, loc); });
	}

	int64_t solve_p2(std::istream& input)
	{
		const Grid grid = read_grid(input);
		return count_crosses(grid, "MAS");
	}
}

ResultType day_four_p1_a(std::istream& input)
{
	return solve_p1(input);
}

ResultType day_four_p2_a(std::istream& input)
{
	return solve_p2(input);
}

ResultType advent_four_p1()
{
	auto input = advent::open_puzzle_input(4);
	return solve_p1(input);
}

ResultType advent_four_p2()
{
	auto input = advent::open_puzzle_input(4);
	return solve_p2(input);
}

#undef DAY4DBG
#undef ENABLE_DAY4DBG
