#include "advent15.h"
#include "advent/advent_utils.h"

#ifdef FORCE_DAY15DBG
#define DAY15DBG 1
#else
#define ENABLE_DAY15DBG 0
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
		static constexpr uint8_t IS_SMALL_BOX = 1 << 1;
		static constexpr uint8_t IS_MOVEABLE = 1 << 2;
		static constexpr uint8_t IS_ROBOT = 1 << 3;
		static constexpr uint8_t IS_LEFT_BOX = 1 << 4;
		static constexpr uint8_t IS_RIGHT_BOX = 1 << 5;
		static constexpr uint8_t IS_GPS_BOX = IS_LEFT_BOX | IS_SMALL_BOX;
		static constexpr uint8_t IS_ANY_BOX = IS_LEFT_BOX | IS_SMALL_BOX | IS_RIGHT_BOX;
		uint8_t flags;
		explicit Tile(uint8_t initial_flags) : flags{ initial_flags } {}
	public:
		static Tile make_empty() { return Tile{ IS_MOVEABLE }; }
		static Tile make_wall() { return Tile{ IS_WALL }; }
		static Tile make_small_box() { return Tile{ IS_SMALL_BOX | IS_MOVEABLE }; }
		static Tile make_left_box() { return Tile{ IS_LEFT_BOX | IS_MOVEABLE }; }
		static Tile make_right_box() { return Tile{ IS_RIGHT_BOX | IS_MOVEABLE }; }
		static Tile make_robot() { return Tile{ IS_ROBOT }; }

		bool is_moveable() const { return flags & IS_MOVEABLE; }
		bool is_locked() const { return !is_moveable(); }
		void lock() { flags = flags & ~IS_MOVEABLE; }
		bool is_empty() const { return ((IS_ANY_BOX | IS_WALL) & flags) == 0u; }
		bool is_wall() const { return flags & IS_WALL; }
		bool is_small_box() const { return flags & IS_SMALL_BOX; }
		bool is_robot() const { return flags & IS_ROBOT; }
		bool is_left_box() const { return flags & IS_LEFT_BOX; }
		bool is_right_box() const { return flags & IS_RIGHT_BOX; }
		bool is_any_box() const { return flags & IS_ANY_BOX; }
		bool is_gps_box() const { return flags & IS_GPS_BOX; }
	};

	using Grid = utils::grid<Tile>; 

	Tile char_to_tile(char c)
	{
		switch (c)
		{
		case '#':
			return Tile::make_wall();
		case 'O':
			return Tile::make_small_box();
		case '[':
			return Tile::make_left_box();
		case ']':
			return Tile::make_right_box();
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
		if (t.is_small_box()) return (t.is_moveable() ? 'o' : 'O');
		if (t.is_left_box()) return (t.is_moveable() ? '[' : '{');
		if (t.is_right_box()) return (t.is_moveable() ? ']' : '}');
		if (t.is_wall()) return '#';
		if (t.is_robot()) return '@';
		if (t.is_empty()) return '.';

		return 'X';
	}

	template <AdventDay day>
	Grid parse_grid(std::istream& in);

	template <>
	Grid parse_grid<AdventDay::one>(std::istream& in)
	{
		return utils::grid_helpers::build(in, char_to_tile);
	}

	template <>
	Grid parse_grid<AdventDay::two>(std::istream& in)
	{
		utils::istream_block_iterator block{ in };
		std::string input_block{ *block };
		std::ostringstream output_stream;
		for (char c : input_block)
		{
			switch (c)
			{
			case '#':
				output_stream << "##";
				break;
			case 'O':
				output_stream << "[]";
				break;
			case '.':
				output_stream << "..";
				break;
			case '@':
				output_stream << "@.";
				break;
			case '\n':
				output_stream << '\n';
				break;
			default:
				AdventUnreachable();
				break;
			}
		}

		return utils::grid_helpers::build(output_stream.str(), char_to_tile);
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
		AdventCheck(grid[loc].is_any_box());
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
		if (!grid[loc].is_any_box()) return false;
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

	template <AdventDay day>
	State parse_state(std::istream& input)
	{
		State result;
		result.grid = parse_grid<day>(input);
		

		// Lock all boxes in corners
		{
			utils::small_vector<Location, 1> candidate_boxes;
			do
			{
				candidate_boxes = result.grid.get_all_coordinates_by_predicate([](Tile t) {return t.is_any_box() && t.is_moveable(); });
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

	bool can_push(const Grid& grid, const Location& from_location, Dir direction)
	{
		const Location target = from_location + Location::dir(direction);
		const Tile t = grid[target];
		if (t.is_locked()) return false;
		if (t.is_empty()) return true;
		AdventCheck(t.is_any_box());

		if (utils::is_horizontal(direction) || t.is_small_box())
		{
			return can_push(grid, target, direction);
		}

		AdventCheck(utils::is_vertical(direction));
		const bool direct = can_push(grid, target, direction);
		if (!direct) return false;

		const Location offset_target = [&target, t]()
			{;
				if (t.is_left_box())
				{
					return target + Location::dir(utils::direction::right);
				}
				else if (t.is_right_box())
				{
					return target + Location::dir(utils::direction::left);
				}
				AdventUnreachable();
				return target;
			}();

		return can_push(grid, offset_target, direction);
	}

	void execute_push(Grid& grid, const Location& from_location, Dir direction)
	{
		AdventCheck(can_push(grid, from_location, direction));
		const Location target = from_location + Location::dir(direction);
		if (!grid[target].is_empty())
		{
			if (utils::is_horizontal(direction) || grid[target].is_small_box())
			{
				execute_push(grid, target, direction);
			}
			else
			{
				const Location offset_target = [&target, t = grid[target]]()
					{;
						if (t.is_left_box())
						{
							return target + Location::dir(utils::direction::right);
						}
						else if (t.is_right_box())
						{
							return target + Location::dir(utils::direction::left);
						}
						AdventUnreachable();
						return target;
					}();

				execute_push(grid, target, direction);
				execute_push(grid, offset_target, direction);
			}
		}

		AdventCheck(grid[target].is_empty());

		grid[target] = grid[from_location];
		grid[from_location] = Tile::make_empty();

		try_lock_box(grid, target);
		for (const Location& n : target.neighbours())
		{
			try_lock_box(grid, n);
		}


		return;
	}

	void play_move(State& state, MoveList::const_iterator move_it)
	{
		const Dir move = *move_it;
		if (can_push(state.grid, state.robot, move))
		{
			execute_push(state.grid, state.robot, move);
			state.robot += Location::dir(move);
		}
	}

	// Stops when all boxes are locked
	State play_moves(State input)
	{
		log << "\nMaking " << input.moves.size() << " moves.";
		for (MoveList::const_iterator it = begin(input.moves); it != end(input.moves); ++it)
		{
			play_move(input, it);
		}
		return input;
	}

	int64_t get_gps_score(const Grid& grid, const Location& loc)
	{
		AdventCheck(grid[loc].is_gps_box());

		const int max_y = grid.get_max_point().y - 1;
		if (grid[loc].is_gps_box())
		{
			const int y = max_y - loc.y;
			const int x = loc.x;	
			return 100 * y + x;
		}

		AdventCheck(grid[loc].is_left_box());
		const int l_x = loc.x;
		const int max_x = grid.get_max_point().x - 1;
		const int r_x = max_x - (loc.x+1);
		const int x = std::min(l_x, r_x);
		
		const int t_y = loc.y;
		const int b_y = max_y - loc.y;
		const int y = b_y;// std::min(t_y, b_y);
		const int result = 100 * y + x;
		log << "\nBox at " << loc << " has score of " << result;
		return result;
	}

	int64_t get_gps_score(const Grid& grid)
	{
		const auto boxes = grid.get_all_coordinates_by_predicate([](Tile t) {return t.is_gps_box(); });
		auto score_view = stdv::transform(boxes, [&grid](const Location& l) {return get_gps_score(grid, l); });
		const int64_t result = stdr::fold_left(score_view, int64_t{ 0 }, std::plus<int64_t>{});
		return result;
	}

	template <AdventDay day>
	int64_t solve_generic(std::istream& input)
	{
		State state = parse_state<day>(input);
		state = play_moves(std::move(state));
#if DAY15DBG
		log << "\nFinal grid:";
		print_state(state);
#endif
		return get_gps_score(state.grid);
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

ResultType day_fifteen_p1_a(std::istream& input)
{
	return solve_p1(input);
}

ResultType day_fifteen_p2_a(std::istream& input)
{
	return solve_p2(input);
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
