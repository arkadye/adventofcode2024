#include "advent16.h"
#include "advent/advent_utils.h"

#ifdef FORCE_DAY16DBG
#define DAY16DBG 1
#else
#define ENABLE_DAY16DBG 1
#ifdef NDEBUG
#define DAY16DBG 0
#else
#define DAY16DBG ENABLE_DAY16DBG
#endif
#endif

#if DAY16DBG
	#include <iostream>
#endif

namespace
{
#if DAY16DBG
	std::ostream& log = std::cout;
#else
	struct {	template <typename T> auto& operator<<(const T&) const noexcept { return *this; } } log;
#endif
}

#define FORCE_SMALL_VECTOR_DEBUG_INFO

#include "a_star.h"
#include "grid.h"
#include "small_vector.h"

namespace
{
	enum class Tile : char
	{
		wall = '#',
		space = '.',
		start = 'S',
		end = 'E'
	};

	Tile char_to_tile(char c)
	{
		AdventCheck(std::string_view{ "#.SE" }.contains(c));
		return static_cast<Tile>(c);
	}

	using Location = utils::coords;
	using Direction = utils::direction;
	using Grid = utils::grid<Tile>;

	struct State
	{
		Location location;
		Direction direction = Direction::right;
		State step_forward() const
		{
			State s = *this;
			s.location += Location::dir(direction);
			return s;
		}
		State turn(utils::turn_dir dir) const
		{
			State s = *this;
			s.direction = utils::rotate(direction, dir);
			return s;
		}
		auto operator<=>(const State& other) const noexcept = default;
	};

	std::ostream& operator<<(std::ostream& os, const State& s)
	{
		os << '[' << s.location << '-' << s.direction << ']';
		return os;
	}

	struct ParseResult
	{
		Grid grid;
		State state;
		Location target;
	};

	ParseResult parse_input(std::istream& input)
	{
		Grid grid = utils::grid_helpers::build(input, char_to_tile);
		State state;
		const std::optional<Location> start_loc = grid.get_coordinates(Tile::start);
		AdventCheck(start_loc.has_value());
		state.location = *start_loc;
		const std::optional<Location> end_loc = grid.get_coordinates(Tile::end);
		AdventCheck(end_loc.has_value());
		return { grid, state, *end_loc };
	}

	int64_t get_best_result(const Grid& grid, const State& start, const Location& target)
	{
		constexpr int64_t step_cost = 1;
		constexpr int64_t turn_cost = 1000;
		auto is_end_point = [target](const State& s) { return s.location == target; };
		auto get_next_nodes = [&grid](const State& state)
			{
				AdventCheck(grid[state.location] != Tile::wall);
				utils::small_vector<State, 3> result;
				const State forward_step = state.step_forward();
				const State left = state.turn(utils::turn_dir::anticlockwise);
				const State right = state.turn(utils::turn_dir::clockwise);
				auto add_if_valid = [&state,&grid, &result](const State& s, const Location& ref_loc)
					{
						if (grid[ref_loc] != Tile::wall)
						{
							result.push_back(s);
						}
						else
						{
							std::cout << "\nRejected adding " << s << ": inside or turning at a wall";
						};
					};
				add_if_valid(forward_step, forward_step.location);
				add_if_valid(left, left.step_forward().location);
				add_if_valid(right, right.step_forward().location);
				return result;
			};

		auto get_cost_between_nodes = [step_cost,turn_cost](const State& from, const State& to) -> int64_t
			{
				if (from.location != to.location)
				{
					AdventCheck(from.direction == to.direction);
					AdventCheck(from.location + Location::dir(from.direction) == to.location);
					return step_cost;
				}
				AdventCheck(from.location == to.location);
				AdventCheck(from.direction != to.direction);
				AdventCheck(utils::is_vertical(from.direction) != utils::is_vertical(to.direction));
				return turn_cost;
			};

		auto get_heuristic_cost = [&target](const State& state) -> int64_t
			{
				const utils::coords offset = target - state.location;
				int64_t cost = step_cost * offset.manhatten_distance();
				if (cost == 0) return cost;

				utils::small_vector<Direction, 2> good_directions;
				if (offset.x > 0)
				{
					good_directions.push_back(Direction::right);
				}
				else if (offset.x < 0)
				{
					good_directions.push_back(Direction::left);
				}
				if (offset.y > 0)
				{
					good_directions.push_back(Direction::up);
				}
				else if (offset.y < 0)
				{
					good_directions.push_back(Direction::down);
				}

				switch (good_directions.size())
				{
				case 1u:
				{
					const Direction good = good_directions[0];
					if (good == state.direction) break;
					cost += turn_cost;
					if (utils::is_vertical(good) == utils::is_vertical(state.direction)) cost += turn_cost;
					break;
				}
				case 2u:
					cost += (turn_cost * ((stdr::find(good_directions, state.direction) != end(good_directions)) ? 1 : 2));
					break;
				default:
					AdventUnreachable();
					break;
				}
				return cost;
			};

		auto states_equal = [](const State& l, const State& r)
			{
				return l == r;
			};

		auto log_progress = [&grid](const State& state, int64_t cost, int64_t plus_heuristic, std::size_t processed, std::size_t to_check, const auto& get_path)
			{
#if DAY16DBG
				log << "\nState: " << state << "Path cost : " << cost << " (H = " << plus_heuristic << ").Processed = " << processed << " Unprocessed = " << to_check;
				return;
				const auto path = get_path();
				log << "\nDirection=" << state.direction;
				const Location grid_max = grid.get_max_point();
				for (int y : stdv::reverse(utils::int_range{ grid_max.y }))
				{
					log << '\n';
					for (int x : utils::int_range{ grid_max.x })
					{
						const Location loc{ x,y };
						if (loc == state.location)
						{
							log << '@';
							continue;
						}
						const auto state_it = stdr::find_if(path, [loc](const State& s) {return s.location == loc; });
						if (state_it != end(path))
						{
							AdventCheck(grid[loc] != Tile::wall);
							const char d = [dir = state_it->direction]()
								{
									switch (dir)
									{
									case Direction::down:
										return 'v';
									case Direction::right:
										return '>';
									case Direction::up:
										return '^';
									case Direction::left:
										return '<';
									default:
										AdventUnreachable();
									}
									return 'X';
								}();
							log << d;
							continue;
						}
						log << static_cast<char>(grid[loc]);
					}
				}
#endif
			};

		const auto [path,cost] = utils::a_star(start, is_end_point, get_next_nodes, get_cost_between_nodes, get_heuristic_cost, states_equal, log_progress);
		return cost;
	}

	int64_t solve_p1(std::istream& input)
	{
		const auto [grid, initial_state, target] = parse_input(input);
		const auto result = get_best_result(grid, initial_state, target);
		return result;
	}
}

namespace
{
	int64_t solve_p2(std::istream& input)
	{
		return 0;
	}
}

ResultType day_sixteen_p1_a(std::istream& input)
{
	return solve_p1(input);
}

ResultType advent_sixteen_p1()
{
	auto input = advent::open_puzzle_input(16);
	return solve_p1(input);
}

ResultType advent_sixteen_p2()
{
	auto input = advent::open_puzzle_input(16);
	return solve_p2(input);
}

#undef DAY16DBG
#undef ENABLE_DAY16DBG
