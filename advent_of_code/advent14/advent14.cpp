#include "advent14.h"
#include "advent/advent_utils.h"

#ifdef FORCE_DAY14DBG
#define DAY14DBG 1
#else
#define ENABLE_DAY14DBG 1
#ifdef NDEBUG
#define DAY14DBG 0
#else
#define DAY14DBG ENABLE_DAY14DBG
#endif
#endif

#if DAY14DBG
	#include <iostream>
#endif

namespace
{
#if DAY14DBG
	std::ostream& log = std::cout;
#else
	struct {	template <typename T> auto& operator<<(const T&) const noexcept { return *this; } } log;
#endif
}

#include "coords.h"
#include "parse_utils.h"
#include "enums.h"
#include "istream_line_iterator.h"
#include "range_contains.h"
#include "modular_int.h"

namespace
{
	enum class Quadrant : uint8_t
	{
		top_left,
		top_right,
		bottom_left,
		bottom_right,
		none
	};

	std::ostream& operator<<(std::ostream& os, Quadrant q)
	{
		using enum Quadrant;
		switch (q)
		{
		case none:
			os << "none";
			break;
		case top_left:
			os << "top_left";
			break;
		case top_right:
			os << "top_right";
			break;
		case bottom_left:
			os << "bottom_left";
			break;
		case bottom_right:
			os << "bottom_right";
			break;
		default:
			AdventUnreachable();
			break;
		}
		return os;
	}

	using CoordType = int16_t;
	using Coords = utils::basic_coords<CoordType>;

	enum class SplitResult : uint8_t
	{
		lower,
		middle,
		higher
	};

	void check_grid_location(Coords loc, Coords grid_size)
	{
		AdventCheck(utils::range_contains_inc(loc.x, 0, grid_size.x));
		AdventCheck(utils::range_contains_inc(loc.y, 0, grid_size.y));
	}

	SplitResult get_side(CoordType val, CoordType max)
	{
		AdventCheck(0 <= max);
		AdventCheck(0 <= val);
		AdventCheck(val <= max);
		const CoordType mid_value = max / 2;
		using enum SplitResult;
		if (val < mid_value)
		{
			return lower;
		}
		else if (val == mid_value)
		{
			return middle;
		}
		else if (val > mid_value)
		{
			return higher;
		}
		AdventUnreachable();
		return middle;
	}

	Quadrant get_quadrant(Coords location, Coords grid_size)
	{
		using enum Quadrant;
		using enum SplitResult;

		check_grid_location(location, grid_size);

		const SplitResult x_split = get_side(location.x, grid_size.x);
		const SplitResult y_split = get_side(location.y, grid_size.y);

		if (x_split == middle || y_split == middle)
		{
			return none;
		}

		if (x_split == lower && y_split == lower)
		{
			return top_left;
		}

		if (x_split == lower && y_split == higher)
		{
			return bottom_left;
		}

		if (x_split == higher && y_split == lower)
		{
			return top_right;
		}

		if (x_split == higher && y_split == higher)
		{
			return bottom_right;
		}

		AdventUnreachable();
		return none;
	}

	struct Robot
	{
		Coords location, velocity;
	};

	Coords parse_prefixed_vector(char prefix, std::string_view vector)
	{
		vector = utils::remove_specific_prefix(vector, prefix);
		vector = utils::remove_specific_prefix(vector, '=');

		return Coords::from_chars(vector);
	}

	Robot parse_robot(std::string_view line, Coords grid_size)
	{
		const auto [loc, vel] = utils::split_string_at_first(line, ' ');
		
		Robot result;
		result.location = parse_prefixed_vector('p', loc);
		result.velocity = parse_prefixed_vector('v', vel);

		check_grid_location(result.location, grid_size);
		return result;
	}

	Coords advance_robot(Robot robot, Coords grid_size, uint64_t steps)
	{
		using Modular = utils::modular<int64_t>;
		Modular x{ robot.location.x, grid_size.x };
		Modular y{ robot.location.y, grid_size.y };

		const utils::coords64 big_velocity = robot.velocity;
		const utils::coords64 offset = big_velocity * steps;
		x += offset.x;
		y += offset.y;
		const Coords result{ static_cast<CoordType>(x.get_value()), static_cast<CoordType>(y.get_value()) };
		log << "\nMoved robot " << steps << " steps from " << robot.location << " with v=" << robot.velocity << " to location " << result;
		return result;
	}

	Quadrant get_robot_final_quadrant(Robot robot, Coords grid_size, uint64_t steps)
	{
		const Coords loc = advance_robot(robot, grid_size, steps);
		check_grid_location(loc, grid_size);
		const Quadrant result = get_quadrant(loc, grid_size);
		log << ": quad=" << result;
		return result;
	}

	Quadrant get_line_quadrant(std::string_view line, Coords grid_size, uint64_t steps)
	{
		const Robot robot = parse_robot(line, grid_size);
		return get_robot_final_quadrant(robot, grid_size, steps);
	}

	using QuadrantCount = std::array<int, utils::to_idx(Quadrant::none)>;

	QuadrantCount count_quadrants(std::istream& input, Coords grid_size, uint64_t steps)
	{
		QuadrantCount result{};
		for (std::string_view line : utils::istream_line_range{ input })
		{
			const Quadrant quad = get_line_quadrant(line, grid_size, steps);
			switch (quad)
			{
			case Quadrant::none:
				break;
			default:
				++result[utils::to_idx(quad)];
				break;
			}
		}
		return result;
	}

	int64_t solve_p1(std::istream& input, int grid_x, int grid_y)
	{
		const QuadrantCount quadrant_count = count_quadrants(input, Coords{ static_cast<CoordType>(grid_x),static_cast<CoordType>(grid_y) }, 100);
		const auto result = stdr::fold_left_first(quadrant_count, std::multiplies<int>{});
		AdventCheck(result.has_value());
		return *result;
	}
}

namespace
{
	void print_grid(std::ostream& output, std::vector<Robot> robots, Coords grid_size)
	{
		bool first = true;
		stdr::sort(robots, [](const Robot& l, const Robot& r) { return l.location < r.location; });

		for (CoordType y : utils::int_range{ grid_size.y })
		{
			if (!first)
			{
				output << '\n';
			}
			first = false;

			for (CoordType x : utils::int_range{ grid_size.x })
			{
				const Coords loc{ x,y };
				const bool has_robot = stdr::binary_search(robots, loc, {}, [](Robot r) {return r.location; });
				output << (has_robot ? '#' : ' ');
			}
		}
	}

	int64_t solve_p2(std::istream& input)
	{
		return 0;
		using ILI = utils::istream_line_iterator;

		const Coords grid_size{ 101,103 };
		std::vector<Robot> robots;
		std::transform(ILI{ input }, ILI{}, std::back_inserter(robots), [grid_size](std::string_view line) {return parse_robot(line, grid_size); });

		int counter = 0;
		while (counter < (grid_size.x * grid_size.y))
		{
			std::ostringstream grid;
			print_grid(grid, robots, grid_size);
			log << "\nSeconds=" << counter << ":\n" << grid.str() << '\n';

			for (Robot& r : robots)
			{
				r.location = advance_robot(r, grid_size, 1);
			}
			++counter;
		}
		return counter;
	}
}

ResultType day_fourteen_internal::p1_a(int num_steps)
{
	constexpr Coords grid_size{ 11,7 };
	const std::string_view input{ "p=2,4 v=2,-3" };
	const Robot robot = parse_robot(input, grid_size);
	const Coords location = advance_robot(robot, grid_size, num_steps);
	return location.to_string();
}

ResultType day_fourteen_p1_b()
{
	std::istringstream input{
		"p=0,4 v=3,-3\n"
		"p=6,3 v=-1,-3\n"
		"p=10,3 v=-1,2\n"
		"p=2,0 v=2,-1\n"
		"p=0,0 v=1,3\n"
		"p=3,0 v=-2,-2\n"
		"p=7,6 v=-1,-3\n"
		"p=3,0 v=-1,-2\n"
		"p=9,3 v=2,3\n"
		"p=7,3 v=-1,2\n"
		"p=2,4 v=2,-3\n"
		"p=9,5 v=-3,-3" };

	return solve_p1(input, 11, 7);

}

ResultType advent_fourteen_p1()
{
	auto input = advent::open_puzzle_input(14);
	return solve_p1(input,101,103);
}

ResultType advent_fourteen_p2()
{
	auto input = advent::open_puzzle_input(14);
	return solve_p2(input);
}

#undef DAY14DBG
#undef ENABLE_DAY14DBG
