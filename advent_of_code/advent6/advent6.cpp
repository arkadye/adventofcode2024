#include "advent6.h"
#include "advent/advent_utils.h"

#ifdef FORCE_DAY6DBG
#define DAY6DBG 1
#else
#define ENABLE_DAY6DBG 1
#ifdef NDEBUG
#define DAY6DBG 0
#else
#define DAY6DBG ENABLE_DAY6DBG
#endif
#endif

#if DAY6DBG
	#include <iostream>
#endif

namespace
{
#if DAY6DBG
	std::ostream& log = std::cout;
#else
	struct {	template <typename T> auto& operator<<(const T&) const noexcept { return *this; } } log;
#endif
}

#include "sorted_vector.h"
#include "Coords.h"
#include "line.h"

#include <map>
#include <vector>

namespace
{
	using CoordType = uint8_t; // Range is 0-130.
	using Coords = utils::basic_coords<CoordType>;
	using Dir = utils::direction;
	using Path = std::vector<utils::line<Coords>>;

	// This is a line. We store it in a map. If the key is the column index this will be row index of all obstacles in that line.
	using MapSegment = utils::sorted_vector<CoordType>;
	using Map = std::map<CoordType, MapSegment>;

	// [0,0] is TOP-LEFT
	class GameState
	{
		Map row_major;
		Map column_major;
		Coords guard_location;
		Dir guard_direction;
		Coords limit;

		static bool has_obstacle(const Map& map, CoordType major, CoordType minor)
		{
			auto map_it = map.find(major);
			if (map_it == end(map)) return false;
			return map_it->second.contains(minor);
		}

		static void add_obstacle(Map& map, CoordType major, CoordType minor)
		{
			AdventCheck(!has_obstacle(map, major, minor));
			auto map_it = map.find(major);
			if (map_it != end(map))
			{
				map_it->second.push_back(minor);
			}
			else
			{
				map.insert(std::pair{ major,MapSegment{minor} });
			}
		}
	public:
		void add_obstacle(Coords c)
		{
			AdventCheck(c != guard_location);
			add_obstacle(row_major, c.y, c.x);
			add_obstacle(column_major, c.x, c.y);
		}

		bool has_obstacle(Coords c) const
		{
			const bool result = has_obstacle(row_major, c.x, c.y);
			AdventCheck(result == has_obstacle(column_major, c.y, c.x));
			return result;
		}

		void add_guard(Coords c, Dir dir)
		{
			AdventCheck(!has_obstacle(c));
			guard_location = c;
			guard_direction = dir;
		}

		Path create_guard_path() const
		{
			Coords gl = guard_location;
			Dir gd = guard_direction;
			Path result;
			while (true)
			{
				Coords move_start = gl;
				const bool guard_moving_horizontally = utils::is_horizontal(gd);
				const Map& map = guard_moving_horizontally ? row_major : column_major;
				const bool guard_ascending = utils::is_ascending(gd);

				const CoordType& stable_coord = guard_moving_horizontally ? gl.y : gl.x;
				CoordType& moving_coord = guard_moving_horizontally ? gl.x : gl.y;

				auto minor_it = map.find(stable_coord);
				bool blocked = false;
				if (minor_it != end(map))
				{
					const MapSegment& obstacles = minor_it->second;
					if (guard_ascending)
					{
						const auto blocker_it = obstacles.upper_bound(moving_coord);
						if (blocker_it != end(obstacles))
						{
							moving_coord = (*blocker_it) - 1;
							gd = utils::rotate(gd, utils::turn_dir::clockwise);
							blocked = true;
						}
					}
					else
					{
						const auto blocker_it = stdr::upper_bound(stdv::reverse(obstacles), moving_coord, std::greater<CoordType>{});
						if (blocker_it != rend(obstacles))
						{
							moving_coord = (*blocker_it) + 1;
							gd = utils::rotate(gd, utils::turn_dir::clockwise);
							blocked = true;
						}
					}
				}
				if (!blocked)
				{
					moving_coord = guard_ascending ? (guard_moving_horizontally ? limit.x : limit.y) - 1 : 0;
				}
				result.emplace_back(move_start, gl);
				if (!blocked)
				{
					return result;
				}
			}
		}

		void set_limit(Coords c)
		{
			limit = c;
		}

		void validate() const
		{
#if DAY6DBG
			auto compare = [](const Map& ref, const Map& target, CoordType major_limit, CoordType minor_limit)
				{
					for (const auto& [major, minors] : ref)
					{
						AdventCheck(major < major_limit);
						for (CoordType minor : minors)
						{
							AdventCheck(minor < minor_limit);
							AdventCheck(has_obstacle(target, minor, major));
						}
					}
				};
			AdventCheck(!has_obstacle(row_major, guard_location.x, guard_location.y));
			AdventCheck(!has_obstacle(column_major, guard_location.y, guard_location.x));
			compare(column_major, row_major, limit.x, limit.y);
			compare(row_major, column_major, limit.y, limit.x);
#endif
		}
	};

	GameState parse_game_state(std::istream& input)
	{
		std::vector<Coords> obstacles;
		Coords limit;
		Coords guard_start;
		auto parse = [&obstacles, &limit, &guard_start, &input]()
			{
				Coords current_location{ 0,0 };
				bool guard_found = false;
				while (true)
				{
					const char c = input.get();
					switch (c)
					{
					case EOF:
						AdventCheck(guard_found);
						limit = current_location;
						return;
					case '\n':
						current_location.x = 0;
						++current_location.y;
						continue;
					case '#':
						obstacles.push_back(current_location);
						break;
					case '^':
						AdventCheck(!guard_found);
						guard_start = current_location;
						guard_found = true;
						break;
					case '.':
						break;
					default:
						AdventUnreachable();
						break;
					}
					++current_location.x;
				}
				AdventUnreachable();
				return;
			};

		parse();

		auto convert_loc = [y = limit.y](Coords c)
			{
				c.y = y - c.y;
				return c;
			};

		GameState result;
		for (Coords& obstacle : obstacles)
		{
			obstacle = convert_loc(obstacle);
			result.add_obstacle(obstacle);
		}

		result.add_guard(convert_loc(guard_start), Dir::up);
		result.set_limit(limit + Coords{ 1,1 });
		result.validate();
		return result;
	}

	std::size_t get_path_coverage(const Path& path)
	{
		std::size_t result = 0u;
		for (auto fwd_check = begin(path); fwd_check != end(path); ++fwd_check)
		{
			result += fwd_check->length();
			for (auto bck_check = begin(path); bck_check != fwd_check; ++bck_check)
			{
				if (auto crossover = fwd_check->get_crossing(*bck_check))
				{
					result -= crossover->length();
				}
			}
		}
		return result;
	}

	int64_t solve_p1(std::istream& input)
	{
		const GameState state = parse_game_state(input);
		const Path path = state.create_guard_path();
		const std::size_t result = get_path_coverage(path);
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

ResultType day_six_p1_a(std::istream& input)
{
	return solve_p1(input);
}

ResultType advent_six_p1()
{
	auto input = advent::open_puzzle_input(6);
	return solve_p1(input);
}

ResultType advent_six_p2()
{
	auto input = advent::open_puzzle_input(6);
	return solve_p2(input);
}

#undef DAY6DBG
#undef ENABLE_DAY6DBG
