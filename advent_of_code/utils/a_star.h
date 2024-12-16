#pragma once

#include <vector>
#include <algorithm>
#include <iterator>
#include <concepts>
#include <type_traits>
#include <ranges>

#include "advent/advent_assert.h"
#include "swap_remove.h"

#define ADVENT_A_STAR_USE_UTILS_SORTED_VECTOR 1
#if ADVENT_A_STAR_USE_UTILS_SORTED_VECTOR
#include "sorted_vector.h"
#else
#include <set>
#endif

#ifdef NDEBUG
#define ADVENT_A_STAR_DEBUG_SPAM 0
#else
#define ADVENT_A_STAR_DEBUG_SPAM 1
#endif

namespace utils
{
	namespace a_star_internal
	{
#if ADVENT_A_STAR_USE_UTILS_SORTED_VECTOR
		template <typename ValueType, typename Sorter>
		using NodeSet = utils::sorted_vector<ValueType, Sorter>;
		template <typename ValueType, typename Sorter>
		void reserve(NodeSet<ValueType, Sorter>& ns, std::size_t num)
		{
			ns.reserve(num);
		}
		template <typename ValueType, typename Sorter>
		ValueType get_last(NodeSet<ValueType, Sorter>& ns)
		{
			ValueType result = std::move(ns.back());
			ns.pop_back();
			return result;
		}
		template <typename ValueType, typename Sorter>
		void push_back(NodeSet<ValueType, Sorter>& ns, ValueType new_val)
		{
			ns.push_back(std::move(new_val));
		}
#else
		template <typename ValueType,typename Sorter>
		using NodeSet = std::set<ValueType, Sorter>;
		template <typename ValueType, typename Sorter>
		void reserve(NodeSet<ValueType, Sorter>& ns, std::size_t num)
		{
			AdventNoOp();
		}
		template <typename ValueType, typename Sorter>
		ValueType get_last(NodeSet<ValueType, Sorter>& ns)
		{
			auto last_it = cend(ns);
			std::advance(last_it, -1);
			auto node = ns.extract(last_it);
			return node.value();
		}
		template <typename ValueType, typename Sorter>
		void push_back(NodeSet<ValueType, Sorter>& ns, ValueType new_val)
		{
			ns.insert(end(ns), std::move(new_val));
		}
#endif
	}

	// NodeType: An arbitrary node. No particular requirements. User provided functors are used to interact.
	// IsEndPointFunc: A function bool f(Node) that returns true if the argument is an end-point.
	// GetNextNodesFunc: Return any iterable type containing NodeTypes that can be reached from a NodeType argument.
	// GetCostBetweenNodesFunc: Functor with the signature: CostType f(NodeType,NodeType).
	// GetHeuristicForNode: Functor with CostType f(NodeType) to get the heuristic.
	// AreNodesEqual: A function bool f(NodeType,NodeType) that returns true if both nodes are equal
	// LogProgressFunc: A function void f(const NodeType& n, CostType cost, CostType cost_with_heuristic, std::size_t processed, std::size_t to_check, const NodeGetterFunc& get_nodes)
	//		that is called every iteration. get_nodes() will build and return a std::vector<NodeType> of all nodes in the path so far.
	template <
		typename NodeType,
		typename IsEndPointFunc,
		typename GetNextNodesFunc,
		typename GetCostBetweenNodesFunc,
		typename GetHeuristicForNode,
		typename AreNodesEqual,
		typename LogProgressFunc>
		auto a_star(
			const NodeType& start_point,
			const IsEndPointFunc& is_end_point,
			const GetNextNodesFunc& get_next_nodes,
			const GetCostBetweenNodesFunc& get_cost_between_nodes,
			const GetHeuristicForNode& get_heuristic,
			const AreNodesEqual& are_nodes_equal,
			const LogProgressFunc& log_progress,
			std::size_t estimated_number_of_nodes = 1)
	{
		using namespace a_star_internal;
		using ID = std::size_t;
		using CostType = decltype(get_cost_between_nodes(start_point, start_point));
		struct AStarNode
		{
			NodeType node;
			CostType cost;
			CostType with_heuristic;
			ID id;
			ID previous_id;
		};

		struct SortById
		{
			bool operator()(const AStarNode& l, const AStarNode& r) const noexcept { return l.id < r.id; }
		};

		struct SortByHeuristic
		{
			bool operator()(const AStarNode& l, const AStarNode& r) const noexcept { return l.with_heuristic > r.with_heuristic; }
		};

		ID latest_id = 0;
		NodeSet<AStarNode,SortById> checked_nodes;

		auto get_checked_node_by_id = [&checked_nodes](ID id) -> const AStarNode&
		{
			AStarNode dummy;
			dummy.id = id;
			const auto find_result = checked_nodes.find(dummy);
			AdventCheck(find_result != end(checked_nodes));
			return *find_result;
		};

		NodeSet<AStarNode,SortByHeuristic> nodes_to_search;
		reserve(nodes_to_search,estimated_number_of_nodes);
		reserve(checked_nodes,estimated_number_of_nodes);

		nodes_to_search.insert(AStarNode{ start_point,0,0,++latest_id,0 });

		while (!nodes_to_search.empty())
		{
#if ADVENT_A_STAR_DEBUG_SPAM
			std::cout << "\nChecked nodes: ";
			for (const AStarNode& node : checked_nodes)
			{
				std::cout << "\n    { " << node.id << ": " << node.cost << '[' << node.with_heuristic << "] " << node.node << " }";
			}
			std::cout << "\nUnchecked nodes: ";
			for (const AStarNode& node : nodes_to_search)
			{
				std::cout << "\n    { " << node.id << ": " << node.cost << '[' << node.with_heuristic << "] " << node.node << " }";
			}
#endif
			AStarNode current_node = get_last(nodes_to_search);

			auto get_path = [&current_node,&get_checked_node_by_id]() -> std::vector<NodeType>
				{
					AStarNode node = current_node;
					std::vector<NodeType> result;
					while (true)
					{
						result.push_back(std::move(node.node));
						if (node.previous_id == 0)
						{
							std::reverse(begin(result), end(result));
							return result;
						}
						node = get_checked_node_by_id(node.previous_id);
					}
				};

			log_progress(current_node.node, current_node.cost, current_node.with_heuristic, checked_nodes.size(), nodes_to_search.size(), get_path);

			// Handle end-point
			if (is_end_point(current_node.node))
			{
				std::vector<NodeType> result = get_path();
				const auto final_cost = current_node.cost;
				return std::make_pair(result, final_cost);
			}

			// Get next nodes
			auto next_nodes = get_next_nodes(current_node.node);
			for (auto& n : next_nodes)
			{
				const auto prev_node_check = std::any_of(begin(checked_nodes), end(checked_nodes),
					[&n,&are_nodes_equal](const AStarNode& asn)
				{
					return are_nodes_equal(n, asn.node);
				});
				if (prev_node_check)
				{
					continue;
				}
				AStarNode node;
				node.cost = current_node.cost + get_cost_between_nodes(current_node.node, n);
				node.with_heuristic = node.cost + get_heuristic(n);
				node.node = std::move(n);
				node.previous_id = current_node.id;
				node.id = ++latest_id;
#if ADVENT_A_STAR_DEBUG_SPAM
				std::cout << "\nAdding node [ID:" << node.id << ";C:" << node.cost << ";H:" << node.with_heuristic << "] " << node.node << " to check.";
#endif
				nodes_to_search.push_back(std::move(node));
				//auto new_it = nodes_to_search.insert(nodes_to_search.upper_bound(node), std::move(node));
				
			}

			// Update checked nodes
			push_back(checked_nodes,std::move(current_node));
		}

		// If we run out of nodes, there's no path.
		return std::make_pair(std::vector<NodeType>{}, CostType{});
	}

	// NodeType: An arbitrary node. No particular requirements. User provided functors are used to interact.
	// IsEndPointFunc: A function bool f(Node) that returns true if the argument is an end-point.
	// GetNextNodesFunc: Return any iterable type containing NodeTypes that can be reached from a NodeType argument.
	// GetCostBetweenNodesFunc: Functor with the signature: CostType f(NodeType,NodeType).
	// GetHeuristicForNode: Functor with CostType f(NodeType) to get the heuristic.
	// AreNodesEqual: A function bool f(NodeType,NodeType) that returns true if both nodes are equal
	template <
		typename NodeType,
		typename IsEndPointFunc,
		typename GetNextNodesFunc,
		typename GetCostBetweenNodesFunc,
		typename GetHeuristicForNode,
		typename AreNodesEqual,
		typename LogProgressFunc>
	auto a_star(
		const NodeType& start_point,
		const IsEndPointFunc& is_end_point,
		const GetNextNodesFunc& get_next_nodes,
		const GetCostBetweenNodesFunc& get_cost_between_nodes,
		const GetHeuristicForNode& get_heuristic,
		const AreNodesEqual& are_nodes_equal,
		std::size_t estimated_number_of_nodes = 1)
	{
		using CostType = decltype(get_cost_between_nodes(start_point, start_point));
		auto dummy_logger = [](const NodeType& n, CostType cost, CostType cost_with_heuristic, std::size_t processed, std::size_t to_check, const auto& get_nodes)
			{

			};
		return a_star(start_point, is_end_point, get_next_nodes, get_cost_between_nodes, get_heuristic, are_nodes_equal, estimated_number_of_nodes, dummy_logger);
	}
}