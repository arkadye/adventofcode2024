#include "advent24.h"
#include "advent/advent_utils.h"

#ifdef FORCE_DAY24DBG
#define DAY24DBG 1
#else
#define ENABLE_DAY24DBG 1
#ifdef NDEBUG
#define DAY24DBG 0
#else
#define DAY24DBG ENABLE_DAY24DBG
#endif
#endif

#if DAY24DBG
	#include <iostream>
#endif

namespace
{
#if DAY24DBG
	std::ostream& log = std::cout;
#else
	struct {	template <typename T> auto& operator<<(const T&) const noexcept { return *this; } } log;
#endif
}

#include "sorted_vector.h"
#include "istream_line_iterator.h"
#include "parse_utils.h"
#include "swap_remove.h"
#include "to_value.h"

namespace
{
	using Wire = std::string;
	using WireSet = utils::sorted_vector<Wire>;

	Wire parse_wire(std::string_view wire)
	{
		AdventCheck(wire.size() == 3u);
		return Wire{ wire };
	}

	enum class Op : char
	{
		AND,
		OR,
		XOR
	};

	Op parse_op(std::string_view in)
	{
		using enum Op;
		if (in == "AND") return AND;
		if (in == "OR") return OR;
		if (in == "XOR") return XOR;
		AdventUnreachable();
		return static_cast<Op>(std::numeric_limits<char>::max());
	}

	struct Operation
	{
		Wire first, second, target;
		Op op = Op::XOR;
	};

	Operation parse_operation(std::string_view line)
	{
		auto split = [&line]()
			{
				auto [result,remaining] = utils::split_string_at_first(line, ' ');
				line = remaining;
				return result;
			};
		Operation result;
		result.first = parse_wire(split());
		result.op = parse_op(split());
		result.second = parse_wire(split());
		line = utils::remove_specific_prefix(line, "-> ");
		result.target = parse_wire(line);
		return result;
	}

	class State
	{
		WireSet ones, zeroes;
		std::vector<Operation> operations;
		WireSet z_wires;

		static Wire make_wire(char prefix, int index)
		{
			Wire result;
			result.push_back(prefix);
			if (index < 10)
			{
				result.push_back('0');
			}
			result += std::to_string(index);
			return result;
		}

		bool get_operation_result(Wire left, Op op, Wire right) const
		{
			const bool lval = get_wire(left);
			const bool rval = get_wire(right);

			switch (op)
			{
			case Op::AND:
				return lval && rval;
			case Op::OR:
				return lval || rval;
			case Op::XOR:
				return lval != rval;
			default:
				AdventUnreachable();
				break;
			}
			return false;
		}

		void override_input(char input, uint64_t value)
		{
			utils::swap_remove_if(ones, [input](Wire w) {return w.front() == input; });
			utils::swap_remove_if(zeroes, [input](Wire w) {return w.front() == input; });
			for (int i = 0; i < 64; ++i)
			{
				const uint64_t mask = uint64_t{ 1u } << i;
				const bool is_one = mask & value;
				const Wire wire = make_wire(input, i);
				(is_one ? ones : zeroes).push_back(wire);
			}
		}

		WireSet get_wires_associated_with_wire(Wire wire) const
		{
			WireSet result{ wire };

			auto recurse = [this,&result](Wire w)
				{
					WireSet recursed = get_wires_associated_with_wire(w);
					stdr::copy(recursed, std::back_inserter(result));
				};

			for (const Operation& op : operations)
			{
				if (op.target == wire)
				{
					recurse(op.first);
					recurse(op.second);
				}
			}

			return result;
		}

	public:
		void set_wire(Wire w, bool val)
		{
			AdventCheck(!ones.contains(w));
			AdventCheck(!zeroes.contains(w));
			WireSet& add_to = val ? ones : zeroes;
			add_to.push_back(w);
			if (w.front() == 'z')
			{
				z_wires.push_back(w);
			}
		}

		void add_operation(const Operation& operation)
		{
			operations.push_back(operation);
		}

		bool get_wire(Wire w) const
		{
			const bool result = ones.contains(w);
			AdventCheck(zeroes.contains(w) != result);
			return result;
		}

		const WireSet& get_z_wires() const
		{
			return z_wires;
		}

		bool has_wire(Wire w) const
		{
			return ones.contains(w) || zeroes.contains(w);
		}

		void resolve_operations()
		{
			while (!operations.empty())
			{
				auto operation_it = stdr::find_if(operations, [this](const Operation& o)
					{
						return has_wire(o.first) && has_wire(o.second);
					});
				AdventCheck(operation_it != end(operations));
				const bool result = get_operation_result(operation_it->first, operation_it->op, operation_it->second);
				set_wire(operation_it->target, result);
				utils::swap_remove(operations, operation_it);
			}
		}

		void override_x(uint64_t new_val)
		{
			override_input('x', new_val);
		}

		void override_y(uint64_t new_val)
		{
			override_input('y', new_val);
		}

		uint64_t read_z() const
		{
			uint64_t result = 0u;
			for (Wire w : z_wires)
			{
				if (!get_wire(w)) continue;
				std::string_view wire = w;
				wire = utils::remove_specific_prefix(wire, 'z');
				const int mask_location = utils::to_value<int>(wire);
				result = result | (uint64_t{ 1u } << mask_location);
			}
			return result;
		}

		std::size_t get_num_bits() const
		{
			return z_wires.size();
		}

		WireSet get_wires_associated_with_output(int output_pin) const
		{
			const Wire wire = make_wire('z', output_pin);
			WireSet result = get_wires_associated_with_wire(wire);
			result.unique();
			return result;
		}

		void dump_state(bool full) const
		{
			log << "\nZ output = " << read_z();
			if (!full) return;
			log << "\nAll wire states:";
			WireSet unified;
			unified.reserve(ones.size() + zeroes.size());
			stdr::copy(ones, std::back_inserter(unified));
			stdr::copy(zeroes, std::back_inserter(unified));

			for (Wire w : unified)
			{
				log << "\n  " << w << ": " << get_wire(w);
			}
		}
	};

	std::pair<Wire, bool> parse_wire_inital_value(std::string_view line)
	{
		auto [wire_str, val_str] = utils::split_string_at_first(line, ": ");
		const Wire wire = parse_wire(wire_str);
		
		AdventCheck(val_str == "1" || val_str == "0");

		const bool val = val_str == "1";
		return { wire, val };
	}

	State parse_state(std::istream& input)
	{
		bool getting_ops = false;

		State result;
		for (std::string_view line : utils::istream_line_range{ input })
		{
			if (line.empty())
			{
				getting_ops = true;
				continue;
			}

			if (getting_ops)
			{
				const Operation operation = parse_operation(line);
				result.add_operation(operation);
			}
			else
			{
				const auto [wire, val] = parse_wire_inital_value(line);
				result.set_wire(wire, val);
			}
		}
		return result;
	}

	uint64_t solve_p1(std::istream& input)
	{
		State state = parse_state(input);
		state.resolve_operations();
		state.dump_state(true);
		return state.read_z();
	}
}

namespace
{
	// Basic strategy:
	// For any two numbers, x + y, the sum of x + y can never affect more than one extra bit.
	// Example: 0b11 + 0b11 = 0b110. 
	//
	// For any pair x__ and y__ we have to consider eight cases:
	//		(c = carry ; r = 2-bit result - next carry then this z__)
	
	// +---+---+---+----+
	// | x | y | c |  r |
	// +---+---+---+----+
	// | 0 | 0 | 0 | 00 |
	// | 0 | 0 | 1 | 01 |
	// | 0 | 1 | 0 | 01 |
	// | 0 | 1 | 1 | 10 |
	// | 1 | 0 | 0 | 01 |
	// | 1 | 0 | 1 | 10 |
	// | 1 | 1 | 0 | 10 |
	// | 1 | 1 | 1 | 11 |
	// +---+---+---+----+

	// If that truth table holds true then all wires involved with these bits are correct.
	// What will make things tricky is that the carry bit interferes with the next bit...

	bool test_input_at(const State& state, int place)
	{
		if (place == 0)
		{
			for (unsigned int i = 0b00; i <= 0b11; ++i)
			{
				State copy = state;
				const unsigned int x = i & 0b1;
				const unsigned int y = i >> 1;
				const unsigned int expected = (x + y) & 0b1;
				copy.override_x(i & 1u);
				copy.override_y(i >> 1);
				copy.resolve_operations();
				const uint64_t result = copy.read_z() & 0b1;
				if (result != expected) return false;
			}
			return true;
		}
		else
		{
			const int offset = place - 1;
			for (uint64_t i = 0b0000; i <= 0b1111; ++i)
			{
				const uint64_t x = (i & 0b11) << offset;
				const uint64_t y = (i >> 2) << offset;
				const uint64_t expected = ((x + y) & 0b10) << offset;
				State copy = state;
				copy.override_x(x << offset);
				copy.override_y(y << offset);
				copy.resolve_operations();
				const uint64_t result = copy.read_z();
				const bool outcome = (result & (uint64_t{ 0b10 } << offset)) == expected;
				log << "\n    Tried: " << x << " + " << y << " = " << expected << " | Got " << result << ". " << (outcome ? "passed" : "failed");
				if (!outcome) return false;
			}
			return true;
		}
		AdventUnreachable();
		return false;
	}

	std::string solve_p2(std::istream& input)
	{
		const State state = parse_state(input);
		WireSet zero_bit_wires = state.get_wires_associated_with_output(40);
		const bool successful = test_input_at(state, 1);
		return "";
	}
}

ResultType day_twentyfour_p1(std::istream& input)
{
	return solve_p1(input);
}

ResultType advent_twentyfour_p1()
{
	auto input = advent::open_puzzle_input(24);
	return solve_p1(input);
}

ResultType advent_twentyfour_p2()
{
	auto input = advent::open_puzzle_input(24);
	return solve_p2(input);
}

#undef DAY24DBG
#undef ENABLE_DAY24DBG
