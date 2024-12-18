#include "advent18.h"
#include "advent/advent_utils.h"

#ifdef FORCE_DAY18DBG
#define DAY18DBG 1
#else
#define ENABLE_DAY18DBG 1
#ifdef NDEBUG
#define DAY18DBG 0
#else
#define DAY18DBG ENABLE_DAY18DBG
#endif
#endif

#if DAY18DBG
	#include <iostream>
#endif

#include "coords.h"
#include "istream_line_iterator.h"
#include "range_contains.h"
#include "sorted_vector.h"
#include "binary_find.h"
#include "parse_utils.h"
#include "coords_iterators.h"

#include <ranges>

namespace
{
#if DAY18DBG
	std::ostream& log = std::cout;
#else
	struct {	template <typename T> auto& operator<<(const T&) const noexcept { return *this; } } log;
#endif
}

namespace
{
	using Coords = utils::coords;
	using CoordList = utils::small_vector<Coords, 1>;

	bool validate_coords(const Coords& loc, const Coords& limit)
	{
		return utils::range_contains_inc(loc.x, 0, limit.x) && utils::range_contains_inc(loc.y, 0, limit.y);
	}


	CoordList get_corrupted_locations(std::istream& input, int num_bytes, const Coords& limit)
	{
		CoordList coords;
		coords.reserve(num_bytes);
		utils::istream_line_range input_range{ input };
		stdr::transform(input_range | stdv::take(num_bytes), std::back_inserter(coords), Coords::from_chars);

		stdr::for_each(coords, [&limit](const Coords& c) {
				AdventCheck(validate_coords(c, limit));
			});

		return coords;
	}

	int pathfind_out(const utils::sorted_vector<Coords>& corrupted, const Coords& finish)
	{
		constexpr Coords start{ 0,0 };
		struct Node
		{
			Coords location;
			std::size_t id = 0u;
			std::size_t prev_id = 0u;
			int steps = 0;
			int heuristic = 0;
		};

		struct HeuristicSort
		{
			bool operator()(const Node& l, const Node& r) const { return l.heuristic > r.heuristic; } // HIGHER heuristic scores at front.
		};

		struct IDSort
		{
			bool operator()(const Node& l, const Node& r) const { return l.id < r.id; }
		};

		CoordList result;

		std::size_t last_id = 0u;

		auto make_node_base = [&finish, &last_id](const Coords& loc, std::size_t last_node_id, int new_steps)
			{
				return Node{
					loc,
					++last_id,
					last_node_id,
					new_steps,
					new_steps + loc.manhatten_distance(finish)
				};
			};

		auto make_node = [&make_node_base](const Coords& loc, const Node& prev)
			{
				return make_node_base(loc, prev.id, prev.steps + 1);
			};

		std::vector<Node> visited_locations;
		utils::sorted_vector<Node, HeuristicSort> nodes_to_check{ make_node_base(start,0u,0u) };

		while (!nodes_to_check.empty())
		{
			const Node node = nodes_to_check.back();
			nodes_to_check.pop_back();

			if (!validate_coords(node.location, finish)) continue;
			if (corrupted.contains(node.location)) continue;

			if (stdr::find_if(visited_locations | stdv::reverse, [&loc = node.location](const Node& n) { return n.location == loc; }) != rend(visited_locations)) continue;

			if (node.location == finish)
			{
#if DAY18DBG
				Node node_to_add = node;
				stdr::sort(visited_locations, IDSort{});
				result.reserve(node.steps);
				while (true)
				{
					if (node_to_add.prev_id == 0u)
					{
						stdr::reverse(result);
						AdventCheck(result.size() == node.steps);
						log << "\nPath length " << node.steps << " found:";
						for (const Coords& c : utils::coords_iterators::get_range(start, finish))
						{
							if (c.x == 0) log << '\n';
							if ((c == start) || (stdr::find(result,c) != end(result)))
							{
								log << 'O';
							}
							else
							{
								log << (corrupted.contains(c) ? '#' : '.');
							}
						}
						break;
					}
					result.push_back(node_to_add.location);
					const auto next_node_it = utils::ranges::binary_find_by_predicate(visited_locations, [id = node_to_add.prev_id](const Node& n) {return n.id <=> id; });
					AdventCheck(next_node_it != end(visited_locations));
					node_to_add = *next_node_it;
				}
#endif
				return node.steps;
			}

			stdr::transform(node.location.neighbours(), std::back_inserter(nodes_to_check), [&node, &make_node](const Coords& loc) {return make_node(loc, node); });

			visited_locations.push_back(node);
		}
#if DAY18DBG
		log << "\nNo path found:";
		for (const Coords& c : utils::coords_iterators::get_range(start,finish))
		{
			if (c.x == 0) log << '\n';
			log << (corrupted.contains(c) ? '#' : '.');
		}
#endif
		return -1;
	}

	int solve_p1(std::istream& input, const Coords& target_location, int num_bytes)
	{
		utils::small_vector<Coords,1> corrupted_bytes = get_corrupted_locations(input, num_bytes, target_location);
		const auto path_size = pathfind_out(utils::sorted_vector<Coords>(std::move(corrupted_bytes)), target_location);
		return path_size;
	}
}

namespace
{
	CoordList::const_iterator solve_p2(CoordList::const_iterator range_start, CoordList::const_iterator range_end, CoordList::const_iterator search_start, CoordList::const_iterator search_end, const Coords& target_location)
	{
		const CoordList::const_iterator midpoint = std::midpoint(search_start, search_end);
		if (search_start == midpoint)
		{
			AdventCheck(search_end != range_end);
			return search_start;
		}

		const int path_size = pathfind_out({ range_start, midpoint }, target_location);
		const bool found_path = (path_size >= 0);

		const CoordList::const_iterator next_start = found_path ? midpoint : search_start;
		const CoordList::const_iterator next_end = found_path ? search_end : midpoint;
		return solve_p2(range_start, range_end, next_start, next_end, target_location);
	}

	std::string solve_p2(std::istream& input, const Coords& target_location)
	{
		utils::small_vector<Coords, 1> corrupted_bytes = get_corrupted_locations(input, std::numeric_limits<int>::max(), target_location);
		const CoordList::const_iterator result_it = solve_p2(begin(corrupted_bytes), end(corrupted_bytes), begin(corrupted_bytes), end(corrupted_bytes), target_location);
		const Coords& result = *result_it;
		std::string output = result.to_string();
		AdventCheck(output.back() == ')');
		output.pop_back();
		AdventCheck(output.front() == '(');
		output.erase(0,1);
		return output;
	}
}

ResultType day_eighteen_p1(std::istream& input)
{
	return solve_p1(input, Coords{ 6,6 }, 12);
}
ResultType day_eighteen_p2(std::istream& input)
{
	return solve_p2(input, Coords{ 6,6 });
}

ResultType advent_eighteen_p1()
{
	auto input = advent::open_puzzle_input(18);
	return solve_p1(input, Coords{ 70,70 }, 1024);
}

ResultType advent_eighteen_p2()
{
	auto input = advent::open_puzzle_input(18);
	return solve_p2(input, Coords{ 70,70 });
}

#undef DAY18DBG
#undef ENABLE_DAY18DBG
