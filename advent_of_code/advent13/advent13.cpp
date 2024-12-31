#include "advent13.h"
#include "advent/advent_utils.h"

#ifdef FORCE_DAY13DBG
#define DAY13DBG 1
#else
#define ENABLE_DAY13DBG 0
#ifdef NDEBUG
#define DAY13DBG 0
#else
#define DAY13DBG ENABLE_DAY13DBG
#endif
#endif

#if DAY13DBG
	#include <iostream>
#endif

namespace
{
#if DAY13DBG
	std::ostream& log = std::cout;
#else
	struct {	template <typename T> auto& operator<<(const T&) const noexcept { return *this; } } log;
#endif
}

#include "coords.h"
#include "istream_block_iterator.h"
#include "parse_utils.h"
#include "to_value.h"
#include "int_range.h"
#include "range_contains.h"
#include "comparisons.h"

namespace
{
	using ValType = int64_t;
	constexpr ValType BUTTON_A_COST = 3;
	constexpr ValType BUTTON_B_COST = 1;
	constexpr ValType NO_SOLUTION = std::numeric_limits<ValType>::max();
	using Coords = utils::basic_coords<ValType>;
	struct ProblemStatement
	{
		Coords prize_location;
		Coords button_a_move;
		Coords button_b_move;
		ValType max_presses{ 0 };
	};

	ValType parse_val_with_id(std::string_view input, char id, std::string_view splitter)
	{
		input = utils::remove_specific_prefix(input, id);
		input = utils::remove_specific_prefix(input, splitter);
		return utils::to_value<ValType>(input);
	}

	Coords parse_line(std::string_view input, std::string_view prefix, std::string_view id_to_num_splitter)
	{
		input = utils::remove_specific_prefix(input, prefix);
		const auto [x_frag, y_frag] = utils::split_string_at_first(input, ' ');
		
		Coords result;
		result.x = parse_val_with_id(utils::remove_specific_suffix(x_frag,','), 'X', id_to_num_splitter);
		result.y = parse_val_with_id(y_frag, 'Y', id_to_num_splitter);
		return result;
	}

	ProblemStatement parse_input_block(std::string_view block, ValType max_presses, const Coords& prize_offset)
	{
		const auto [button_a, button_b, prize] = utils::get_string_elements(block, '\n', 0, 1, 2);

		ProblemStatement ps;
		ps.max_presses = max_presses;
		ps.button_a_move = parse_line(button_a, "Button A: ", "");
		ps.button_b_move = parse_line(button_b, "Button B: ", "");
		ps.prize_location = parse_line(prize, "Prize: ", "=") + prize_offset;

		return ps;
	}

	bool is_solution(const ProblemStatement& ps, ValType A, ValType B)
	{
		return A * ps.button_a_move + B * ps.button_b_move == ps.prize_location;
	}

	ValType score_solution(const ProblemStatement& ps, ValType A, ValType B)
	{
		AdventCheck(utils::range_contains_inc(A, 0, ps.max_presses));
		AdventCheck(utils::range_contains_inc(B, 0, ps.max_presses));
		AdventCheck(is_solution(ps,A,B));
		return BUTTON_A_COST * A + BUTTON_B_COST * B;
	}

	ValType score_problem(const ProblemStatement& ps)
	{
		// For some B, Px = A * Ax + B * Bx AND Py = A * Ay + B * By.
		// So we can say for: B = (Px - A*Ax) / Bx or B = Px/Bx - A*Ax/Bx

		// We can then substitue this in to the OTHER equation for Y and get one equation in terms of A:
		// Py = A*Ay + By*(Px/Bx - A*Ax/Bx)

		// Expand brackets
		// Py = A*Ay + By*Px/Bx - A*Ax*By/Bx

		// Multiply by Bx to avoid integer divisions
		// Bx*Py = A*Ay*Bx + By*Px - A*Ax*By

		// Collect the A terms
		// Bx*Py = A*(Ay*Bx - Ax*By) + By*px

		// A = Bx*Py - By*px

		// And so A = (Bx*Py - By*px) / (Ay*Bx - Ax*By)

		// Set out some intermediate values:
		// N = (Bx*Py - By*px)
		// M = (Ay*Bx - Ax*By)
		// So A = N / M for values where N%M==0

		// This can be substituted back into B = (Px - A*Ax) / Bx to get a solution candidate.

		const ValType Ax = ps.button_a_move.x;
		const ValType Ay = ps.button_a_move.y;
		const ValType Bx = ps.button_b_move.x;
		const ValType By = ps.button_b_move.y;
		const ValType Px = ps.prize_location.x;
		const ValType Py = ps.prize_location.y;

		const ValType N = (Bx * Py - By * Px);
		const ValType M = (Ay * Bx - Ax * By);

		if (M == 0) return NO_SOLUTION;
		if (N % M != 0) return NO_SOLUTION;
		const ValType A = N / M;

		const ValType P = Px - A * Ax;
		if (Bx == 0) return NO_SOLUTION;
		if (P % Bx != 0) return NO_SOLUTION;
		const ValType B = P / Bx;
		AdventCheck(is_solution(ps, A, B));
		return score_solution(ps, A, B);
	}

	ValType score_block(std::string_view block, ValType max_presses, const Coords& prize_offset)
	{
		const ProblemStatement ps = parse_input_block(block,max_presses, prize_offset);
		const auto result = score_problem(ps);
		return result != NO_SOLUTION ? result : 0;
	}

	ValType solve_generic(std::istream& input, ValType max_presses, const Coords& prize_offset)
	{
		using IBI = utils::istream_block_iterator;
		const auto tf = [max_presses, prize_offset](std::string_view b)
			{
				return score_block(b, max_presses, prize_offset);
			};
		const auto result = std::transform_reduce(IBI{ input }, IBI{}, ValType{ 0 }, std::plus<ValType>{}, tf);
		return result;
	}

	int64_t solve_p1(std::istream& input)
	{
		return solve_generic(input, 100, Coords{ 0,0 });
	}
}

namespace
{
	int64_t solve_p2(std::istream& input)
	{
		constexpr ValType offset = 10'000'000'000'000;
		return solve_generic(input, std::numeric_limits<ValType>::max(), Coords{offset,offset});
	}
}

ResultType day_thirteen_p1_a(std::istream& input)
{
	return solve_p1(input);
}

ResultType advent_thirteen_p1()
{
	auto input = advent::open_puzzle_input(13);
	return solve_p1(input);
}

ResultType advent_thirteen_p2()
{
	auto input = advent::open_puzzle_input(13);
	return solve_p2(input);
}

#undef DAY13DBG
#undef ENABLE_DAY13DBG
