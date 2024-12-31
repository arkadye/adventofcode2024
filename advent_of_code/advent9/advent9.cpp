#include "advent9.h"
#include "advent/advent_utils.h"

#ifdef FORCE_DAY9DBG
#define DAY9DBG 1
#else
#define ENABLE_DAY9DBG 0
#ifdef NDEBUG
#define DAY9DBG 0
#else
#define DAY9DBG ENABLE_DAY9DBG
#endif
#endif

#if DAY9DBG
	#include <iostream>
#endif

namespace
{
#if DAY9DBG
	std::ostream& log = std::cout;
#else
	struct {	template <typename T> auto& operator<<(const T&) const noexcept { return *this; } } log;
#endif
}

#include "int_range.h"
#include "to_value.h"

#include <vector>
#include <span>

namespace
{
	using BlockStorage = std::vector<int8_t>;
	using BlockView = std::span<int8_t>;

	BlockStorage get_input(std::istream& input)
	{
		using It = std::istream_iterator<char>;
		BlockStorage result;
		std::transform(It{ input }, It{}, std::back_inserter(result), [](char c) {return utils::to_value<int8_t>(std::string_view{ &c,1 }); });
		return result;
	}

	uint64_t get_partial_checksum(std::size_t file_id, std::size_t block_len, std::size_t starting_idx)
	{
		/*
		
		We want to multiply the file_id by the total area created by graphing the indices.
		
		For block_len = 4 and starting_idx = 3 This looks like this:

		   *
		  **
		 *** 
		**** 
		****
		**** 
		
		This can be decomposed into a triangle and a square:

		TRIANGLE:
		   *
		  **
		 ***
		
		SQUARE:
		****
		****
		****
		
		The triangle has area (block_len * (block_len - 1)) / 2. (block_len == 0 not a worry as it will get zeroed.
		The square has area block_len * starting_idx.
		*/

		// It *might* be faster to check for zeroes in appropriate places and skip the calculation.
		// But all the calculations are computationally trivial and the zero cases are exceedingly rare.
		const uint64_t square_area = block_len * starting_idx;
		const uint64_t triangle_area = (block_len * (block_len - 1)) / 2;
		const uint64_t total_area = square_area + triangle_area;
		const uint64_t result = file_id * total_area;
//		log << "\nGetting partial checksum for region: ID=" << file_id << " start = " << starting_idx << " len = " << block_len << " RESULT=" << result;
		return result;
	}
	uint64_t solve_p1(BlockView input)
	{
		AdventCheckMsg(input.size() % 2 == 1, "Input has empty space at end.");
		for (auto i : utils::int_range{ input.size() })
		{
			AdventCheckMsg(!((i % 2 == 0) && (input[i] == '0')), "Input has a zero length file.");
		}
		uint64_t checksum = 0u;
		std::size_t start_file_id = 0u;
		std::size_t end_file_id = input.size() / 2 + 1; // We decrement right at the start.
		std::size_t starting_idx = 0u;
		int end_file_len_remaining = 0;
		while (!input.empty())
		{
			// FILE AT START
			{
				const int file_len = input.front();
				const uint64_t partial_checksum = get_partial_checksum(start_file_id, file_len, starting_idx);
				checksum += partial_checksum;
				starting_idx += file_len;
				++start_file_id;
				input = input.subspan(1);
			}

			// Check whether we're finished
			if (input.empty())
			{
				const uint64_t partial_checksum = get_partial_checksum(end_file_id, end_file_len_remaining, starting_idx);
				checksum += partial_checksum;
				continue;
			}

			// GET FILE AT END AND HOLE AT START
			int hole_len = input.front();
			input = input.subspan(1);
			while (hole_len != 0 && !input.empty())
			{
				AdventCheck(hole_len > 0);
				AdventCheck(end_file_len_remaining >= 0);
				if (end_file_len_remaining == 0)
				{
					end_file_len_remaining = input.back();
					AdventCheck(end_file_len_remaining > 0);
					--end_file_id;

					// Remove both the file AND the free space at the end.
					input = input.subspan(0, input.size() - 2);
				}

				const int block_to_fill = std::min(hole_len, end_file_len_remaining);
				const uint64_t partial_checksum = get_partial_checksum(end_file_id, block_to_fill, starting_idx);
				checksum += partial_checksum;
				hole_len -= block_to_fill;
				end_file_len_remaining -= block_to_fill;
				starting_idx += block_to_fill;
			}
			
		}
		return checksum;
	}

	uint64_t solve_p1(std::istream& input)
	{
		BlockStorage blocks = get_input(input); // Span needs non-const :-(
		const BlockView view(blocks);
		return solve_p1(view);
	}
}

namespace
{
	struct Block
	{
#if DAY9DBG
		std::size_t idx = 0u;
#endif
		std::size_t file_id = 0u;
		std::size_t file_len = 0;
		std::size_t space_len = 0;
	};

	std::ostream& operator<<(std::ostream& oss, const Block& b)
	{
		oss << '[' << b.file_id << ',' << static_cast<int>(b.file_len) << ',' << static_cast<int>(b.space_len) << ']';
		return oss;
	}

	void try_moving_file(std::vector<Block>& blocks, std::size_t id_to_move)
	{
		//AdventCheck(blocks.back().space_len == 0);
		const auto file_block_it = stdr::find_if(blocks, [id_to_move](const Block& b) {return b.file_id == id_to_move; });
		AdventCheck(file_block_it != end(blocks));

		const std::size_t file_len = file_block_it->file_len;
		const auto move_target = std::find_if(begin(blocks), file_block_it, [file_len](const Block& b) {return b.space_len >= file_len; });

		// File will be placed immediately AFTER move_target. So move_target doesn't move.
		if (move_target == file_block_it)
		{
			log << "\nNo target found for " << *file_block_it;
			return; // No suitable target was found.
		}

#if DAY9DBG
		// Update the debug index of the block moving
		file_block_it->idx = move_target->idx + move_target->file_len;
#endif

		// If we're moving just one space left, no re-ordering happens. Just shift around some space.
		const auto preceding_block = file_block_it - 1;
		if (move_target == preceding_block)
		{
			log << "\nMoving " << *file_block_it << " one left.\n    From:" << *move_target << ',' << *file_block_it;
			file_block_it->space_len += move_target->space_len;
			move_target->space_len = 0;
			log << "\n    To  :" << *move_target << ',' << *file_block_it;
			return;
		}

		// Set the new padding length on the preceding file to account for the full size of the current block.
		preceding_block->space_len += (file_block_it->file_len + file_block_it->space_len);

		// Adjust space on current block to account for its new position
		file_block_it->space_len = move_target->space_len - file_block_it->file_len;

		// Target block ends up with no space after it.
		move_target->space_len = 0;

		// Now shift the blocks around!
		log << "\nMoving " << *file_block_it << " multiple spaces.\n    From:" << *move_target << ',' << *(move_target + 1) << " ... " << *(file_block_it - 1) << ',' << *file_block_it;
		std::rotate(move_target + 1, file_block_it, file_block_it + 1);
		log << "\n    To  :" << *move_target << ',' << *(move_target + 1) << " ... " << *(file_block_it - 1) << ',' << *file_block_it;

		// Last thing: set the space at the end of blocks to be zero
		//blocks.back().space_len = 0;
	}

	uint64_t solve_p2(const BlockStorage& input)
	{
		std::vector<Block> blocks;
		blocks.reserve(input.size());
		for (auto idx : utils::int_range<std::size_t>(0u, input.size(), 2u))
		{
			Block block;
			block.file_id = idx / 2;
			block.file_len = input[idx];
			if (idx < input.size() - 1)
			{
				block.space_len = input[idx + 1];
			}
			blocks.push_back(block);
		}
#if DAY9DBG
		{
			std::size_t start_idx = 0u;
			for (Block& block : blocks)
			{
				block.idx = start_idx;
				start_idx += (block.file_len + block.space_len);
			}
		}
#endif

		const std::size_t last_file_id = blocks.back().file_id;
		
		for (auto id : stdv::reverse(utils::int_range{ last_file_id + 1 }))
		{
			try_moving_file(blocks, id);
#if DAY9DBG
			// Verify the indices
			std::size_t idx = 0u;
			for (const Block& block : blocks)
			{
				AdventCheck(block.idx == idx);
				idx += (block.file_len + block.space_len);
			}
#endif
		}

		// Build the checksum
		uint64_t checksum = 0u;
		std::size_t start_idx = 0u;
		for (const Block& block : blocks)
		{
			const uint64_t partial_checksum = get_partial_checksum(block.file_id, block.file_len, start_idx);
			checksum += partial_checksum;
			start_idx += (block.file_len + block.space_len);
		}
		return checksum;
	}

	uint64_t solve_p2(std::istream& input)
	{
		return solve_p2(get_input(input));
	}
}

namespace
{
	std::istringstream testcase_a()
	{
		return std::istringstream{ "12345" };
	}

	std::istringstream testcase_b()
	{
		return std::istringstream{ "2333133121414131402" };
	}
}

ResultType day_nine_p1_a()
{
	auto input = testcase_a();
	return solve_p1(input);
}


ResultType day_nine_p1_b()
{
	auto input = testcase_b();
	return solve_p1(input);
}

ResultType day_nine_p2_a()
{
	auto input = testcase_a();
	return solve_p2(input);
}


ResultType day_nine_p2_b()
{
	auto input = testcase_b();
	return solve_p2(input);
}

ResultType advent_nine_p1()
{
	auto input = advent::open_puzzle_input(9);
	return solve_p1(input);
}

ResultType advent_nine_p2()
{
	auto input = advent::open_puzzle_input(9);
	return solve_p2(input);
}

#undef DAY9DBG
#undef ENABLE_DAY9DBG

/*

0 1 2 3  4  5  6  7  8
0 2 2 1  1  1  2  2  2
0 2 4 3  4  5 12 14 16
0 2 6 9 13 18 30 44 60

0 1 2 3 4  5  6  7  8  9 10 11 12  13  14
0 . . 1 1  1  .  .  .  .  2  2  2   2   2
0 0 0 3 4  5  0  0  0  0 20 22 24  26  28
0 0 0 3 7 12 12 12 12 12 32 54 78 104 132

*/