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

//#define FORCE_SMALL_VECTOR_DEBUG_INFO

#include "grid.h"
#include "small_vector.h"
#include "sorted_vector.h"
#include "swap_remove.h"

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

	using Path = utils::small_vector<Location, 1>;

	struct PathFindingResults
	{
		utils::small_vector<Path,1> paths;
		int64_t cost = std::numeric_limits<int64_t>::max();
	};

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

	struct SearchNode
	{
		State state;
		int64_t cost = 0;
		std::size_t node_id = 0u;
		std::size_t previous_id = 0u;
	};

	std::ostream& operator<<(std::ostream& os, const SearchNode& sn)
	{
		os << "{ [" << sn.node_id << "] " << sn.state << " (" << sn.cost << ") }";
		return os;
	}

	struct NodeId
	{
		std::size_t operator()(const SearchNode& n) const noexcept { return n.node_id; }
	};

	struct NodeCost
	{
		int64_t operator()(const SearchNode& n) const noexcept { return n.cost; }
	};

	struct NodeState
	{
		State operator()(const SearchNode& n) const noexcept { return n.state; }
	};

	template <std::invocable<const SearchNode&> NodeAdaptor>
	struct NodeSorter
	{
		bool operator()(const SearchNode& l, const SearchNode& r) const noexcept
		{
			return NodeAdaptor{}(l) < NodeAdaptor{}(r);
		}
	};

	template <std::invocable<const SearchNode&,const SearchNode&> Sorter>
	struct Inverter
	{
		bool operator()(const SearchNode& l, const SearchNode& r) const noexcept
		{
			return Sorter{}(r, l);
		}
	};

	template <AdventDay day>
	PathFindingResults find_paths(const Grid& grid, const State& start, const Location& target)
	{
		// Finding ALL the paths is not helped by A*'s heuristic, so just use a simply Djkstra search.
		constexpr int64_t step_cost = 1;
		constexpr int64_t turn_cost = 1000;
		constexpr bool find_all_paths = (day == AdventDay::two);

		PathFindingResults result;
		utils::small_vector<std::size_t, 1> path_endpoints_by_id;

		std::size_t last_id = 0u;

		auto node_from_state = [&last_id](std::size_t previous_node_id, const State& state, int64_t cost)
			{
				SearchNode result;
				result.cost = cost;
				result.previous_id = previous_node_id;
				result.node_id = ++last_id;
				result.state = state;
				return result;
			};

		auto node_from_previous_node = [&node_from_state,step_cost, turn_cost](const SearchNode& previous, const State& new_state)
			{
				int64_t cost = 0;
				AdventCheck(previous.state != new_state);
				if (previous.state.direction == new_state.direction)
				{
					// We must have moved
					cost = step_cost;
					AdventCheck(previous.state.location.manhatten_distance(new_state.location) == 1);
				}
				else
				{
					// We must have turned
					AdventCheck(previous.state.location == new_state.location);
					AdventCheck(utils::is_vertical(previous.state.direction) != utils::is_vertical(new_state.direction));
					cost = turn_cost;
				}

				return node_from_state(previous.node_id, new_state, previous.cost + cost);
			};

		utils::sorted_vector<SearchNode, Inverter<NodeSorter<NodeCost>>, 1> nodes_to_search{ node_from_state(0u, start, 0) };
		utils::sorted_vector<SearchNode, NodeSorter<NodeState>, 1> searched_nodes;

		while (!nodes_to_search.empty())
		{
			log << "\nChecked " << searched_nodes.size() << " nodes:";
			for (const SearchNode& n : searched_nodes)
			{
				log << "\n    " << n;
			}
			log << "\nUnchecked " << nodes_to_search.size() << " remaining:";
			for (const SearchNode& n : nodes_to_search)
			{
				log << "\n    " << n;
			}

			const SearchNode node_to_check = nodes_to_search.back();
			nodes_to_search.pop_back();

			{
#if DAY16DBG
				log << "\nChecking node " << node_to_check;
				utils::small_vector<State, 1> states_in_current_path;
				states_in_current_path.reserve(searched_nodes.size());
				std::size_t id = node_to_check.previous_id;
				while (id != 0u)
				{
					auto path_it = searched_nodes.find_if_no_sort([id](const SearchNode& sn) {return sn.node_id == id; });
					AdventCheck(path_it != end(searched_nodes));
					const State& path_state = path_it->state;
					auto previously_used_it = stdr::find_if(states_in_current_path, [&loc = path_state.location](const State& s)
						{
							return s.location == loc;
						});
					if (previously_used_it == end(states_in_current_path))
					{
						states_in_current_path.push_back(path_state);
					}
					id = path_it->previous_id;
				}

				for (utils::coords it : utils::coords_iterators::elem_range{ grid.get_max_point() })
				{
					const utils::coords loc{ it.x, grid.get_max_point().y - it.y - 1 };
					if (loc.x == 0) log << '\n';
					if (loc == node_to_check.state.location)
					{
						log << '@';
						continue;
					}
					const auto path_find_result = stdr::find_if(states_in_current_path, [&loc](const State& s) {return s.location == loc; });
					if (path_find_result != end(states_in_current_path))
					{
						switch (path_find_result->direction)
						{
						case Direction::up:
							log << '^'; break;
						case Direction::right:
							log << '>'; break;
						case Direction::down:
							log << 'v'; break;
						case Direction::left:
							log << '<'; break;
						default:
							AdventUnreachable(); break;
						}
						utils::swap_remove(states_in_current_path, path_find_result);
						continue;
					}
					log << static_cast<char>(grid[loc]);
				}
				log << '\n';
#endif
			}

			// Check if we've exhausted the search area
			if (node_to_check.cost > result.cost) break;

			// Check if we're at the end.
			if (node_to_check.state.location == target)
			{
				AdventCheck(path_endpoints_by_id.empty() || (node_to_check.cost == result.cost));
				
				result.cost = node_to_check.cost;
				
				if constexpr (find_all_paths)
				{
					path_endpoints_by_id.push_back(node_to_check.node_id);
					searched_nodes.push_back(node_to_check);
				}
				if constexpr (!find_all_paths)
				{
					break;
				}
			}

			// Check if we've already seen a better way to get here.
			{
				auto [first, last] = searched_nodes.equal_range(node_to_check);
				if (std::any_of(first, last, [this_cost = node_to_check.cost, find_all_paths](const SearchNode& s)
					{
						return (find_all_paths ? (s.cost < this_cost) : (s.cost <= this_cost));
					}))
				{
					continue;
				}
			}

			// Add new nodes to search
			auto try_adding_node = [&grid,&nodes_to_search,&node_from_previous_node,&node_to_check](const State& state, const Location & reference_location)
			{
				AdventCheck(grid.is_on_grid(reference_location));
				if (grid[reference_location] != Tile::wall)
				{
					const SearchNode next_node = node_from_previous_node(node_to_check, state);
					log << "\nAdding node " << next_node << " to check";
					nodes_to_search.insert_keep_sorted(next_node);
				}
			};

			const State forward_step = node_to_check.state.step_forward();
			const State left_turn = node_to_check.state.turn(utils::turn_dir::anticlockwise);
			const State right_turn = node_to_check.state.turn(utils::turn_dir::clockwise);

			try_adding_node(forward_step, forward_step.location);
			try_adding_node(left_turn, left_turn.step_forward().location);
			try_adding_node(right_turn, right_turn.step_forward().location);

			// Store that we've searched here
			searched_nodes.insert_keep_sorted(node_to_check);
		}

		if constexpr (find_all_paths)
		{
			utils::sorted_vector<SearchNode, NodeSorter<NodeId>, 1> nodes(std::move(searched_nodes));
			auto get_node_by_id = [&nodes](std::size_t id) -> const SearchNode&
				{
					auto result = nodes.binary_find_if(id,[](const SearchNode& sn, std::size_t id) {return sn.node_id <=> id; });
					AdventCheck(result != end(nodes));
					return *result;
				};
			for (std::size_t id : path_endpoints_by_id)
			{
				Path path;
				while (id != 0u)
				{
					const SearchNode& node = get_node_by_id(id);
					const Location& location = node.state.location;
					if (path.empty() || path.back() != location)
					{
						path.push_back(location);
					}
					id = node.previous_id;
				}
				result.paths.push_back(std::move(path));
			}
		}

		return result;
	}

	int64_t solve_p1(std::istream& input)
	{
		const auto [grid, initial_state, target] = parse_input(input);
		const auto [dummy, result] = find_paths<AdventDay::one>(grid, initial_state, target);
		return result;
	}
}

namespace
{
	std::size_t solve_p2(std::istream& input)
	{
		const auto [grid, initial_state, target] = parse_input(input);
		const auto [paths, dummy] = find_paths<AdventDay::two>(grid, initial_state, target);
		utils::sorted_vector<Location> path_locations;
		{
			const std::size_t total_path_size = stdr::fold_left(paths 
				| stdv::transform([](const Path& p) {return p.size(); }),
				std::size_t{ 0u },
				std::plus<std::size_t>{});
			path_locations.reserve(total_path_size);
		}
		
		stdr::copy(paths | stdv::join, std::back_inserter(path_locations));
		path_locations.unique();

		log << "\nCombined paths:";
		for (utils::coords it : utils::coords_iterators::elem_range{ grid.get_max_point() })
		{
			const utils::coords loc{ it.x, grid.get_max_point().y - it.y - 1 };
			if (loc.x == 0) log << '\n';
			auto path_it = path_locations.find(loc);
			if (path_it == end(path_locations))
			{
				log << 'O';
				continue;
			}
			log << static_cast<char>(grid[loc]);
		}

		return path_locations.size();
	}
}

ResultType day_sixteen_p1_a(std::istream& input)
{
	return solve_p1(input);
}

ResultType day_sixteen_p2_a(std::istream& input)
{
	return solve_p2(input);
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
