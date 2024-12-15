#include "advent15.h"
#include "advent/advent_utils.h"

#ifdef FORCE_DAY15DBG
#define DAY15DBG 1
#else
#define ENABLE_DAY15DBG 1
#ifdef NDEBUG
#define DAY15DBG 0
#else
#define DAY15DBG ENABLE_DAY15DBG
#endif
#endif

#if DAY15DBG
	#include <iostream>
#endif

namespace
{
#if DAY15DBG
	std::ostream& log = std::cout;
#else
	struct {	template <typename T> auto& operator<<(const T&) const noexcept { return *this; } } log;
#endif
}

#include "grid.h"
#include "transform_if.h"

namespace
{
	using Location = utils::coords;
	class Tile
	{
		static constexpr uint8_t IS_WALL = 1 << 0;
		static constexpr uint8_t IS_BOX = 1 << 1;
		static constexpr uint8_t IS_MOVEABLE = 1 << 2;
		static constexpr uint8_t IS_ROBOT = 1 << 3;
		uint8_t flags;
		explicit Tile(uint8_t initial_flags) : flags{ initial_flags } {}
	public:
		static Tile make_empty() { return Tile{ IS_MOVEABLE }; }
		static Tile make_wall() { return Tile{ IS_WALL }; }
		static Tile make_box() { return Tile{ IS_BOX | IS_MOVEABLE }; }
		static Tile make_robot() { return Tile{ IS_ROBOT }; }

		bool is_moveable() const { return flags & IS_MOVEABLE; }
		bool is_locked() const { return !is_moveable(); }
		void lock() { flags = flags & ~IS_MOVEABLE; }
		bool is_empty() const { return ((IS_BOX | IS_WALL) & flags) == 0u; }
		bool is_wall() const { return flags & IS_WALL; }
		bool is_box() const { return flags & IS_BOX; }
		bool is_robot() const { return flags & IS_ROBOT; }
	};

	using Grid = utils::grid<Tile>; 

	Tile char_to_tile(char c)
	{
		switch (c)
		{
		case '#':
			return Tile::make_wall();
		case 'O':
			return Tile::make_box();
		case '@':
			return Tile::make_robot();
		case '.':
			return Tile::make_empty();
		default:
			AdventUnreachable();
			break;
		}
		return Tile::make_empty();
	}

	char tile_to_char(Tile t)
	{
		if (t.is_box()) return (t.is_moveable() ? 'o' : 'O');
		if (t.is_wall()) return '#';
		if (t.is_robot()) return '@';
		if (t.is_empty()) return ' ';
		return 'X';
	}

	Grid parse_grid(std::istream& in)
	{
		return utils::grid_helpers::build(in, char_to_tile);
	}

	void print_grid(const Grid& g)
	{
#if DAY15DBG
		log << '\n';
		g.stream_grid(log, tile_to_char);
#endif
	}

	using Dir = utils::direction;

	bool is_dir_char(char c)
	{
		constexpr std::string_view valid{ "^v<>" };
		return valid.contains(c);
	}

	Dir parse_dir(char c)
	{
		using enum Dir;
		switch (c)
		{
		case '^':
			return up;
		case 'v':
			return down;
		case '>':
			return right;
		case '<':
			return left;
		default:
			AdventUnreachable();
			break;
		}
		return Dir::up;
	}

	using MoveList = std::vector<Dir>;

	MoveList parse_moves(std::istream& in)
	{
		utils::istream_block_iterator block{ in };
		std::string_view moves{ *block };

		MoveList result;
		result.reserve(moves.size());

		auto filtered_moves = stdv::filter(moves, is_dir_char);
		stdr::transform(filtered_moves, std::back_inserter(result), parse_dir);

		return result;
	}

	struct State
	{
		Grid grid;
		MoveList moves;
		Location robot;
	};

	void print_state(State& s)
	{
#if DAY15DBG
		s.grid[s.robot] = Tile::make_robot();
		print_grid(s.grid);
		s.grid[s.robot] = Tile::make_empty();
#endif
	}

	bool try_lock_box(Grid& grid, const Location& loc);

	void lock_box(Grid& grid, const Location& loc)
	{
		AdventCheck(grid.is_on_grid(loc));
		AdventCheck(grid[loc].is_box());
		AdventCheck(grid[loc].is_moveable());

		grid[loc].lock();

		for (const Location& neighbour : loc.neighbours())
		{
			try_lock_box(grid, neighbour);
		}
	}

	bool try_lock_box(Grid& grid, const Location& loc)
	{
		if (!grid.is_on_grid(loc)) return false;
		if (!grid[loc].is_box()) return false;
		if (grid[loc].is_locked()) return false;

		const auto neighbours = loc.neighbours();

		for (auto didx : utils::int_range{ neighbours.size() })
		{
			const auto next_didx = (didx + 1) % neighbours.size();
			const Location first_neighbour = neighbours[didx];
			const Location next_neighbour = neighbours[next_didx];

			auto is_locked = [&grid](const Location& l)
				{
					return grid.is_on_grid(l) && grid[l].is_locked();
				};

			if (is_locked(first_neighbour) && is_locked(next_neighbour))
			{
				lock_box(grid, loc);
				return true;
			}
		}
		return false;
	}

	State parse_state(std::istream& input)
	{
		State result;
		result.grid = parse_grid(input);
		

		// Lock all boxes in corners
		{
			utils::small_vector<Location, 1> candidate_boxes;
			do
			{
				candidate_boxes = result.grid.get_all_coordinates_by_predicate([](Tile t) {return t.is_box() && t.is_moveable(); });
			} while (stdr::any_of(candidate_boxes, [&g = result.grid](const Location& l) {return try_lock_box(g, l); }));
		}

		print_grid(result.grid);
		
		std::optional<Location> start = result.grid.get_coordinates_by_predicate([](Tile t) {return t.is_robot(); });
		AdventCheck(start.has_value());
		result.robot = start.value();
		result.grid[start.value()] = Tile::make_empty();

		result.moves = parse_moves(input);
		return result;
	}

	void play_move(State& state, MoveList::const_iterator move_it)
	{
		const Dir move = *move_it;
		const Location target = state.robot + Location::dir(move);
		
		Grid& grid = state.grid;
		AdventCheck(grid.is_on_grid(target));

		if (grid[target].is_locked())
		{
			// Can't move there.
			return;
		}

		if (grid[target].is_empty())
		{
			state.robot = target;
			return;
		}

		AdventCheck(grid[target].is_box() && grid[target].is_moveable());

		// Ray cast along the push to find an end-point: either an empty square, or something immoveable
		Location filled_loc = target;
		while (true)
		{
			filled_loc += Location::dir(move);
			AdventCheck(grid.is_on_grid(filled_loc));

			if (grid[filled_loc].is_locked())
			{
				// Can't push in this direction.
				return;
			}

			if (grid[filled_loc].is_empty())
			{
				// Move a box here.
				break;
			}
		}

		grid[filled_loc] = Tile::make_box();
		grid[target] = Tile::make_empty();
		state.robot = target;

		// Try locking the moved box and neighbours
		for (const Location& l : filled_loc.neighbours())
		{
			try_lock_box(grid, l);
		}
		try_lock_box(grid, filled_loc);
	}

	// Stops when all boxes are locked
	State play_moves(State input)
	{
		log << "\nMaking " << input.moves.size() << " moves.";
		for (MoveList::const_iterator it = begin(input.moves); it != end(input.moves); ++it)
		{
			play_move(input, it);
			//log << "\nMove " << *it;
			//print_state(input);
		}
		return input;
	}

	int64_t get_gps_score(const Grid& grid, const Location& loc)
	{
		AdventCheck(grid.is_on_grid(loc));
		AdventCheck(grid[loc].is_box());

		const int max_y = grid.get_max_point().y - 1;
		const int x = loc.x;
		const int y = max_y - loc.y;
		return 100 * y + x;
	}

	int64_t get_gps_score(const Grid& grid)
	{
		const auto boxes = grid.get_all_coordinates_by_predicate([](Tile t) {return t.is_box(); });
		auto score_view = stdv::transform(boxes, [&grid](const Location& l) {return get_gps_score(grid, l); });
		const int64_t result = stdr::fold_left(score_view, int64_t{ 0 }, std::plus<int64_t>{});
		return result;
	}

	int64_t solve_p1(std::istream& input)
	{
		State state = parse_state(input);
		state = play_moves(std::move(state));
#if DAY15DBG
		log << "\nFinal grid:";
		print_state(state);
#endif
		return get_gps_score(state.grid);
	}
}

namespace
{
	int64_t solve_p2(std::istream& input)
	{
		return 0;
	}
}

ResultType day_fifteen_p1_a(std::istream& input)
{
	return solve_p1(input);
}

ResultType advent_fifteen_p1()
{
	auto input = advent::open_puzzle_input(15);
	return solve_p1(input);
}

ResultType advent_fifteen_p2()
{
	auto input = advent::open_puzzle_input(15);
	return solve_p2(input);
}

#undef DAY15DBG
#undef ENABLE_DAY15DBG
