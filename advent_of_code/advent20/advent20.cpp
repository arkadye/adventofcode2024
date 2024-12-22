#include "advent20.h"
#include "advent/advent_utils.h"

#ifdef FORCE_DAY20DBG
#define DAY20DBG 1
#else
#define ENABLE_DAY20DBG 1
#ifdef NDEBUG
#define DAY20DBG 0
#else
#define DAY20DBG ENABLE_DAY20DBG
#endif
#endif

#if DAY20DBG
	#include <iostream>
#endif

namespace
{
#if DAY20DBG
	std::ostream& log = std::cout;
#else
	struct {	template <typename T> auto& operator<<(const T&) const noexcept { return *this; } } log;
#endif
}

#include "grid.h"

namespace
{
	enum class TileType : char
	{
		floor = '.',
		wall = '#',
		start = 'S',
		end = 'E'
	};

	struct Tile
	{
		int steps_to_end;
		TileType type;
		explicit Tile(TileType t) : type{ t }, steps_to_end{ std::numeric_limits<int>::max() } {}
	};

	Tile to_tile(char c)
	{
		return Tile{ static_cast<TileType>(c) };
	}

	using Grid = utils::grid<Tile>;

	enum class DumpGridMode : char
	{
		normal,
		show_costs
	};

	char tile_to_char(Tile t, DumpGridMode mode)
	{
		switch (mode)
		{
		case DumpGridMode::show_costs:
			if (t.type == TileType::floor)
			{
				return (t.steps_to_end % 10) + '0';
			}
			[[fallthrough]];
		case DumpGridMode::normal:
			return static_cast<char>(t.type);
		default:
			AdventUnreachable();
			break;
		}
		return 'X';
	}

	void dump_grid(const Grid& grid, DumpGridMode mode)
	{
#if DAY20DBG
		grid.stream_grid(log, [mode](Tile t) {return tile_to_char(t, mode); });
#endif
	}

	Grid parse_grid(std::istream& input)
	{
		Grid result = utils::grid_helpers::build(input, to_tile);

		log << "\nGot grid:";
		dump_grid(result, DumpGridMode::normal);

#if DAY20DBG
		// Do the checks
		for (utils::coords loc : utils::coords_iterators::elem_range{ result.get_max_point() })
		{
			const Tile tile = result[loc];
			if (tile.type == TileType::wall) continue;
			const auto non_wall_neighbours = stdr::count_if(loc.neighbours(), [](Tile t) { return t.type != TileType::wall; }, [&result](utils::coords n) {return result[n]; });
			if (tile.type == TileType::floor)
			{
				AdventCheck(non_wall_neighbours == 2);
			}
			else
			{
				AdventCheck(non_wall_neighbours == 1);
			}
		}
#endif


		utils::small_vector<utils::coords, 1> tiles_to_check = result.get_all_coordinates_by_predicate([](Tile t) {return t.type == TileType::end; });
		utils::small_vector<utils::coords, 1> next_set_of_tiles;
		
		int current_steps = 0u;

		while (!tiles_to_check.empty())
		{
			AdventCheck(tiles_to_check.size() == 1u);
			for (utils::coords loc : tiles_to_check)
			{
				Tile& t = result[loc];
				AdventCheck(t.type != TileType::wall);
				AdventCheck(t.steps_to_end > current_steps);
				t.steps_to_end = current_steps;
				stdr::copy_if(loc.neighbours(), std::back_inserter(next_set_of_tiles), [current_steps](Tile t)
					{
						return t.type != TileType::wall && t.steps_to_end > current_steps;
					},
					[&result](utils::coords c) { return result[c]; });
			}
			tiles_to_check.swap(next_set_of_tiles);
			next_set_of_tiles.clear();
			++current_steps;
		}

		log << "\nCosted grid:";
		dump_grid(result, DumpGridMode::show_costs);
		return result;
	}

	int score_cheat(const Grid& grid, utils::coords location, utils::direction direction)
	{
		constexpr int CHEAT_FAILED = std::numeric_limits<int>::min();
		const Tile start_tile = grid[location];
		if (start_tile.type == TileType::wall) return CHEAT_FAILED;

		const utils::coords target_loc = location + 2 * utils::coords::dir(direction);
		if (!grid.is_on_grid(target_loc)) return CHEAT_FAILED; // Fail if cheat takes you out of bounds.
		const Tile target_tile = grid[target_loc];
		if (target_tile.type == TileType::wall) return CHEAT_FAILED;

		const int non_cheated_score = start_tile.steps_to_end - 2;
		return non_cheated_score - target_tile.steps_to_end;
	}

	int64_t count_cheats_at_location(const Grid& grid, utils::coords location, int threshold)
	{
		using enum utils::direction;
		constexpr std::array<utils::direction, 4> directions{ up, right, down, left };
		return stdr::count_if(directions, [threshold](int score) {return score >= threshold; }, [&grid, location](utils::direction dir) {return score_cheat(grid, location, dir); });
	}

	int64_t count_cheats(const Grid& grid, int threshold)
	{
		return stdr::fold_left(
			utils::coords_iterators::elem_range{ grid.get_max_point() }
			| stdv::transform([&grid, threshold](utils::coords loc) {return count_cheats_at_location(grid, loc, threshold); }),
				0,
				std::plus<int64_t>{});
	}

	int64_t solve_p1(std::istream& input, int cheat_threshold)
	{
		const Grid grid = parse_grid(input);
		return count_cheats(grid, cheat_threshold);
	}
}

namespace
{
	int64_t solve_p2(std::istream& input)
	{
		return 0;
	}
}

ResultType advent_20::p1(std::istream& input, int threshold)
{
	return solve_p1(input, threshold);
}

ResultType advent_twenty_p1()
{
	auto input = advent::open_puzzle_input(20);
	return solve_p1(input, 100);
}

ResultType advent_twenty_p2()
{
	auto input = advent::open_puzzle_input(20);
	return solve_p2(input);
}

#undef DAY20DBG
#undef ENABLE_DAY20DBG
