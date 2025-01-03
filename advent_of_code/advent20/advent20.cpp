#include "advent20.h"
#include "advent/advent_utils.h"

#ifdef FORCE_DAY20DBG
#define DAY20DBG 1
#else
#define ENABLE_DAY20DBG 0
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

#include <map>

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

	int score_cheat(const Grid& grid, const utils::coords& start_location, const utils::coords& end_location, int max_cheat)
	{
		constexpr int CHEAT_FAILED = std::numeric_limits<int>::min();

		const int cheat_time = start_location.manhatten_distance(end_location);
		if (cheat_time > max_cheat) return CHEAT_FAILED;
		
		const Tile start_tile = grid[start_location];
		if (start_tile.type == TileType::wall) return CHEAT_FAILED;

		if (!grid.is_on_grid(end_location)) return CHEAT_FAILED; // Fail if cheat takes you out of bounds.
		const Tile target_tile = grid[end_location];
		if (target_tile.type == TileType::wall) return CHEAT_FAILED;

		const int non_cheated_score = start_tile.steps_to_end - cheat_time;
		return non_cheated_score - target_tile.steps_to_end;
	}

	int64_t count_cheats_at_location(const Grid& grid, const utils::coords& location, int threshold, int max_cheat)
	{
		const utils::coords cheat_offsets{ max_cheat };
		auto target_candidates = utils::coords_iterators::get_range(location - cheat_offsets, location + cheat_offsets);
		return stdr::count_if(target_candidates, [threshold](int score) {return score >= threshold; }, [&grid, location, max_cheat](utils::coords target) {return score_cheat(grid, location, target, max_cheat); });
	}

	int64_t count_cheats(const Grid& grid, int threshold, int max_cheat)
	{
		return stdr::fold_left(
			utils::coords_iterators::elem_range{ grid.get_max_point() }
			| stdv::transform([&grid, threshold, max_cheat](utils::coords loc) {return count_cheats_at_location(grid, loc, threshold, max_cheat); }),
				0,
				std::plus<int64_t>{});
	}

	int64_t solve_generic(std::istream& input, int cheat_threshold, int max_cheat)
	{
		const Grid grid = parse_grid(input);
		return count_cheats(grid, cheat_threshold, max_cheat);
	}

	int64_t solve_p1(std::istream& input, int cheat_threshold)
	{
		return solve_generic(input, cheat_threshold, 2);
	}

	int64_t solve_p2(std::istream& input, int cheat_threshold)
	{
		return solve_generic(input, cheat_threshold, 20);
	}
}

ResultType advent_20::p1(std::istream& input, int threshold)
{
	return solve_p1(input, threshold);
}

ResultType advent_20::p2(std::istream& input, int threshold)
{
	return solve_p2(input, threshold);
}

ResultType advent_twenty_p1()
{
	auto input = advent::open_puzzle_input(20);
	return solve_p1(input, 100);
}

ResultType advent_twenty_p2()
{
	auto input = advent::open_puzzle_input(20);
	return solve_p2(input,100);
}

#undef DAY20DBG
#undef ENABLE_DAY20DBG
