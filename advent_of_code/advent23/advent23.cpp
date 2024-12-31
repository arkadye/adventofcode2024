#include "advent23.h"
#include "advent/advent_utils.h"

#ifdef FORCE_DAY23DBG
#define DAY23DBG 1
#else
#define ENABLE_DAY23DBG 1
#ifdef NDEBUG
#define DAY23DBG 0
#else
#define DAY23DBG ENABLE_DAY23DBG
#endif
#endif

#if DAY23DBG
	#include <iostream>
#endif

namespace
{
#if DAY23DBG
	std::ostream& log = std::cout;
#else
	struct {	template <typename T> auto& operator<<(const T&) const noexcept { return *this; } } log;
#endif
}

#include "parse_utils.h"
#include "sorted_vector.h"
#include "int_range.h"
#include "swap_remove.h"
#include "istream_line_iterator.h"

#ifndef NDEBUG
#include "utils/tests/utils_tests.h"
#endif

#include <vector>

namespace
{
#ifdef NDEBUG
	using Computer = uint16_t;
	Computer parse_computer(std::string_view computer_name)
	{
		AdventCheck(computer_name.size() == 2u);
		return (static_cast<uint16_t>(computer_name[0]) << 8) + static_cast<uint16_t>(computer_name[1]);
	}
	bool computer_starts_with(Computer computer, char c)
	{
		return (computer >> 8) == static_cast<uint16_t>(c);
	}
	void check_computer(Computer) {}
	std::string to_string(Computer computer)
	{
		std::string result;
		result.push_back(static_cast<char>(computer >> 8));
		result.push_back(static_cast<char>(computer % 0x100));
		return result;
	}
#else
	using Computer = std::string;
	bool computer_starts_with(Computer computer, char c)
	{
		return computer.front() == c;
	}
	Computer parse_computer(std::string_view computer_name)
	{
		AdventCheck(computer_name.size() == 2u);
		return Computer{ computer_name };
	}
	void check_computer(Computer c)
	{
		AdventCheck(c.size() == 2u);
	}
	std::string to_string(Computer computer)
	{
		return computer;
	}
#endif

	using Link = std::pair<Computer, Computer>;

	Link parse_link(std::string_view line)
	{
		auto [left_computer_str, right_computer_str] = utils::split_string_at_first(line, '-');
		const Computer left = parse_computer(left_computer_str);
		const Computer right = parse_computer(right_computer_str);
		return std::minmax(left, right);
	}

	using Network = utils::sorted_vector<Computer, std::less<Computer>, 3>;

	struct NetworkStateBase
	{
		utils::sorted_vector<Link> links;
		bool has_link(Computer first, Computer second) const
		{
			if (first == second) return false;
			if (first > second)
			{
				return has_link(second, first);
			}

			return links.contains(Link{ first, second });
		}

		bool has_mutual_link(Link link, Computer computer) const
		{
			return has_link(link.first, computer) && has_link(link.second, computer);
		}
	};

	struct NetworkStateP1 : NetworkStateBase
	{
		utils::sorted_vector<Computer> computers;
		utils::sorted_vector<Network> networks;
	};

	NetworkStateP1 add_pair_p1(Link link, NetworkStateP1 network_state)
	{
		// Do thruples first
		for (Computer c : network_state.computers)
		{
			if (network_state.has_mutual_link(link,c))
			{
				Network tw{ link.first, link.second, c };
				network_state.networks.push_back(std::move(tw));
			}
		}

		// Add the link
		network_state.links.insert_keep_sorted(link);

		// Add the individuals
		network_state.computers.insert_keep_sorted(link.first);
		network_state.computers.insert_keep_sorted(link.second);
		network_state.computers.unique();

		return network_state;
	}

	NetworkStateP1 parse_input_p1(std::istream& input)
	{
		NetworkStateP1 result;
		for (std::string_view line : utils::istream_line_range{input})
		{
			const Link link = parse_link(line);
			result = add_pair_p1(link, std::move(result));
		}
		return result;
	}

	bool thruple_filter(const Network& tw)
	{
		constexpr char COMPUTER_STARTS_WITH = 't';
		auto computer_matches = [COMPUTER_STARTS_WITH](Computer c)
			{
				check_computer(c);
				return computer_starts_with(c, COMPUTER_STARTS_WITH);
			};

		return stdr::any_of(tw, computer_matches);
	}

	std::size_t solve_p1(std::istream& input)
	{
		const NetworkStateP1 networks = parse_input_p1(input);
		return stdr::count_if(networks.networks, thruple_filter);
	}
}

namespace
{
	using NetworkMap = utils::flat_map<Computer, Network>;

	NetworkMap add_pair_p2(Link link, NetworkMap network_state)
	{
		auto add_link = [&network_state](Computer first, Computer second)
		{
				auto existing_map = network_state.find_by_key(first);
				if (existing_map == end(network_state))
				{
					network_state.insert({ first,Network{second} });
					return;
				}

				AdventCheck(!existing_map->second.contains(second));
				existing_map->second.insert(second);
		};

		add_link(link.first, link.second);

		return network_state;
	}

	NetworkMap parse_p2(std::istream& input)
	{
		NetworkMap result;
		for (std::string_view line : utils::istream_line_range{ input })
		{
			const Link link = parse_link(line);
			result = add_pair_p2(link, std::move(result));
		}
		return result;
	}

	Network get_biggest_subnetwork(const NetworkMap& map, const Network& network, std::size_t search_limit)
	{
		if (network.size() < search_limit) return Network{};
		utils::flat_map<Computer, uint16_t> num_breaks;
		uint16_t total_breaks = 0u;
		stdr::transform(network, std::back_inserter(num_breaks), [](Computer c) {return std::pair{ c,uint16_t{0} }; });

		for (auto first = begin(network); first != end(network); ++first)
		{
			const Network dummy;
			const Computer c_f = *first;
			const Network& connections = map.contains_key(c_f) ? map.at(c_f) : dummy;
			for (auto second = first + 1; second != end(network); ++second)
			{
				const Computer c_s = *second;
				if (!connections.contains(c_s))
				{
					++num_breaks[c_f];
					++num_breaks[c_s];
					++total_breaks;
				}
			}
		}

		if (total_breaks == 0u) return network;

		if (network.size() == search_limit) return Network{};

		auto breaks_underlying = std::move(num_breaks.get_underlying());
		stdr::sort(breaks_underlying, std::greater<uint16_t>{}, & std::pair<Computer, uint16_t>::second);

		Network best;

		for (auto [remove_computer, min_removals] : breaks_underlying)
		{
			Network candidate = network;
			candidate.erase(remove_computer);
			Network result = get_biggest_subnetwork(map, candidate, search_limit);
			search_limit = std::max(search_limit, result.size());
			if (result.size() > best.size())
			{
				best = std::move(result);
			}
		}

		return best;
	}

	Network get_best_network(const NetworkMap& network_map)
	{
		auto make_whole_network = [](std::pair<Computer, Network> datum)
			{
				datum.second.push_back(datum.first);
				return datum.second;
			};

		std::vector<Network> networks;

		{
			networks.reserve(network_map.size());
			stdr::transform(network_map, std::back_inserter(networks), make_whole_network);

			stdr::sort(networks);
			auto new_finish = std::unique(begin(networks), end(networks));
			networks.erase(new_finish, end(networks));
		}

		stdr::sort(networks, std::greater<std::size_t>{}, & Network::size);

		Network result;

		for (const Network& network : networks)
		{
			Network candidate = get_biggest_subnetwork(network_map, network, std::max<std::size_t>(2u, result.size()));
			if (candidate.size() > result.size())
			{
				result = std::move(candidate);
			}
		}

		return result;
	}

	std::string solve_p2(std::istream& input)
	{
		const NetworkMap network_map = parse_p2(input);
		const Network result = get_best_network(network_map);

		std::ostringstream output_builder;
		bool first_elem = true;
		for (Computer c : result)
		{
			if (first_elem)
			{
				first_elem = false;
			}
			else
			{
				output_builder << ',';
			}
			output_builder << to_string(c);
		}
		return output_builder.str();
	}
}

ResultType day_twentythree_p1(std::istream& input)
{
	return solve_p1(input);
}

ResultType day_twentythree_p2(std::istream& input)
{
	return solve_p2(input);
}

ResultType advent_twentythree_p1()
{
	auto input = advent::open_puzzle_input(23);
	return solve_p1(input);
}

ResultType advent_twentythree_p2()
{
	auto input = advent::open_puzzle_input(23);
	return solve_p2(input);
}

#undef DAY23DBG
#undef ENABLE_DAY23DBG
