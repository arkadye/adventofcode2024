#include "advent17.h"
#include "advent/advent_utils.h"

#ifdef FORCE_DAY17DBG
#define DAY17DBG 1
#else
#define ENABLE_DAY17DBG 1
#ifdef NDEBUG
#define DAY17DBG 0
#else
#define DAY17DBG ENABLE_DAY17DBG
#endif
#endif

#if DAY17DBG
	#include <iostream>
#endif

namespace
{
#if DAY17DBG
	std::ostream& log = std::cout;
#else
	struct {	template <typename T> auto& operator<<(const T&) const noexcept { return *this; } } log;
#endif
}

#include "enums.h"
#include "small_vector.h"
#include "string_line_iterator.h"
#include "parse_utils.h"
#include "to_value.h"

namespace
{
	using RegType = uint64_t;
	using ComboArg = uint8_t;

	enum class Opcode : uint8_t
	{
		adv = 0,
		bxl = 1,
		bst = 2,
		jnz = 3,
		bxc = 4,
		out = 5,
		bdv = 6,
		cdv = 7
	};

	using Program = utils::small_vector<uint8_t, 16>;
	using ProgramOutput = Program;

	enum Reg : uint8_t
	{
		A = 0u,
		B = 1u,
		C = 2u
	};

	struct Computer
	{
		std::array<RegType, 3> registers;
		std::size_t ip{ 0u };

		ProgramOutput output;

		RegType get_combo_arg(ComboArg val)
		{
			AdventCheck(val < 7u);
			if (val < 4u)
			{
				return val;
			}
			return registers[val - 4];
		}

		static RegType get_literal_arg(uint8_t val)
		{
			AdventCheck(val <= 8);
			return val;
		}

		template <typename Self>
		auto& reg(this Self&& self, Reg r) { return self.registers[utils::to_idx(r)]; }

		void div(Reg target, ComboArg divider) { const RegType arg = get_combo_arg(divider); AdventCheck(arg < 64); reg(target) = reg(A) >> arg; ip += 2u; }

		void adv(ComboArg a) { div(Reg::A, a); }
		void bxl(ComboArg a) { RegType& B = reg(Reg::B); B = B ^ get_literal_arg(a); ip += 2u; }
		void bst(ComboArg a) { reg(Reg::B) = get_combo_arg(a) % 8; ip += 2; }
		void jnz(ComboArg a) { if (reg(Reg::A) == 0) { ip += 2u; } else { ip = get_literal_arg(a); } }
		void bxc(ComboArg a) { RegType& B = reg(Reg::B); B = B ^ reg(Reg::C); ip += 2; }
		void out(ComboArg a) { output.push_back(get_combo_arg(a) % 8); ip += 2; }
		void bdv(ComboArg a) { div(Reg::B, a); }
		void cdv(ComboArg a) { div(Reg::C, a); }

		void execute(Opcode op, ComboArg arg)
		{
#define MAKE_CASE(code) case Opcode::code: code(arg); break
			switch (op)
			{
				MAKE_CASE(adv);
				MAKE_CASE(bxl);
				MAKE_CASE(bst);
				MAKE_CASE(jnz);
				MAKE_CASE(bxc);
				MAKE_CASE(out);
				MAKE_CASE(bdv);
				MAKE_CASE(cdv);
			default:
				AdventUnreachable();
				break;
			}
#undef MAKE_CASE
		}

		void execute(uint8_t op, ComboArg arg)
		{
			AdventCheck(op < 8);
			return execute(static_cast<Opcode>(op), arg);
		}
	};

	RegType parse_register(std::istream& input, char name)
	{
		std::string dummy;
		std::getline(input, dummy);
		std::string_view line{ dummy };

		line = utils::remove_specific_prefix(line, "Register ");
		line = utils::remove_specific_prefix(line, name);
		line = utils::remove_specific_prefix(line, ": ");
		return utils::to_value<RegType>(line);
	}

	std::pair<Computer, Program> parse_input(std::istream& input)
	{
		std::pair<Computer, Program> result;
		result.first.reg(Reg::A) = parse_register(input, 'A');
		result.first.reg(Reg::B) = parse_register(input, 'B');
		result.first.reg(Reg::C) = parse_register(input, 'C');

		std::string dummy;
		std::getline(input, dummy);

		AdventCheck(dummy.empty());
		std::getline(input, dummy);

		std::string_view line = utils::remove_specific_prefix(dummy, "Program: ");

		stdr::transform(utils::string_line_range{ line,',' }, std::back_inserter(result.second), utils::to_value<uint8_t>);

		return result;
	}

	Computer execute_program(Computer computer, const Program& program)
	{
		while (computer.ip < program.size() - 1)
		{
			const auto raw_opcode = program[computer.ip];
			const auto arg = program[computer.ip + 1];
			computer.execute(raw_opcode, arg);
		}
		return computer;
	}

	Computer execute_program(std::istream& input)
	{
		const std::pair<Computer, Program> start = parse_input(input);
		const Computer result = execute_program(start.first, start.second);
		return result;
	}

	std::string solve_p1(std::istream& input)
	{
		const Computer result = execute_program(input);

		std::ostringstream output_stream;
		stdr::copy(result.output, std::ostream_iterator<unsigned int>{output_stream, ","});
		std::string output = output_stream.str();
		if (!output.empty())
		{
			AdventCheck(output.back() == ',');
			output.pop_back();
		}
		return output;
	}

	int64_t get_register_after(std::istream& input, Reg r)
	{
		const Computer result = execute_program(input);
		return result.reg(r);
	}
}

namespace
{
	int64_t solve_p2(std::istream& input)
	{
		return 0;
	}
}

ResultType day_seventeen_p1(std::istream& input)
{
	return solve_p1(input);
}

ResultType day_seventeen_p1_a(std::istream& input)
{
	return get_register_after(input, Reg::A);
}

ResultType day_seventeen_p1_b(std::istream& input)
{
	return get_register_after(input, Reg::B);
}

ResultType day_seventeen_p1_c(std::istream& input)
{
	return get_register_after(input, Reg::C);
}

ResultType advent_seventeen_p1()
{
	auto input = advent::open_puzzle_input(17);
	return solve_p1(input);
}

ResultType advent_seventeen_p2()
{
	auto input = advent::open_puzzle_input(17);
	return solve_p2(input);
}

#undef DAY17DBG
#undef ENABLE_DAY17DBG
