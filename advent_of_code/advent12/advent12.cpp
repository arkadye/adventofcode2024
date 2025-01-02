#include "advent12.h"
#include "advent/advent_utils.h"

#ifdef FORCE_DAY12DBG
#define DAY12DBG 1
#else
#define ENABLE_DAY12DBG 0
#ifdef NDEBUG
#define DAY12DBG 0
#else
#define DAY12DBG ENABLE_DAY12DBG
#endif
#endif

#if DAY12DBG
	#include <iostream>
#endif

namespace
{
#if DAY12DBG
	std::ostream& log = std::cout;
#else
	struct {	template <typename T> auto& operator<<(const T&) const noexcept { return *this; } } log;
#endif
}

#include "grid.h"
#include "enums.h"

#include <vector>

namespace
{
	using Dir = utils::direction;

	class Square
	{
		using FlagType = uint8_t;
		char region_id = 0;

		FlagType flags{ 0 };

		static constexpr FlagType get_dir_flag(Dir d) { return FlagType{ 0b1 } << utils::to_idx(d); }
		static constexpr FlagType get_checked_flag() { return FlagType{ 0b1000'0000 }; }
	public:
		explicit Square(char in) : region_id{ in } { AdventCheck(std::isalpha(in) || in == '.'); }
		void mark_checked() { flags = flags | get_checked_flag(); }
		bool is_checked() const { return (flags & get_checked_flag()) != 0; }
		void set_fence(Dir d) { flags = flags | get_dir_flag(d); }
		bool has_fence(Dir d) const { return (flags & get_dir_flag(d)) != 0; }
		char get_id() const { return region_id; }
	};

	using Grid = utils::grid<Square>;

	Grid parse_grid(std::istream& input)
	{
		return utils::grid_helpers::build(input, [](char c) {return Square{ c }; });
	}

	template <AdventDay day>
	uint64_t get_overall_score(Grid grid)
	{
		auto get_square = [&grid](utils::coords loc)
			{
				if (!grid.is_on_grid(loc))
				{
					Square result{ '.' };
					result.mark_checked();
					return result;
				}
				return grid[loc];
			};

		uint64_t result = 0;
		std::vector<utils::coords> regions_to_check{ utils::coords{0,0} };
		while (!regions_to_check.empty())
		{
			const utils::coords region_loc = regions_to_check.back();
			AdventCheck(grid.is_on_grid(region_loc));
			regions_to_check.pop_back();

			if (get_square(region_loc).is_checked()) continue;

			const char region_id = get_square(region_loc).get_id();

			std::size_t region_area = 0u;
			std::size_t region_fences = 0u;

			std::vector<utils::coords> squares_to_check{ region_loc };
			while (!squares_to_check.empty())
			{
				const utils::coords loc = squares_to_check.back();
				squares_to_check.pop_back();
				AdventCheck(grid.is_on_grid(loc));
				Square& square = grid[loc];

				AdventCheck(square.get_id() == region_id);
				if (square.is_checked()) continue;

				++region_area;
				constexpr std::array<Dir, 4> directions{ Dir::up, Dir::right, Dir::down, Dir::left };
				for (Dir dir : directions)
				{
					const utils::coords neighbour_loc = loc + utils::coords::dir(dir);
					const Square neighbour = get_square(neighbour_loc);
					if (neighbour.get_id() != square.get_id())
					{
						square.set_fence(dir);
						if constexpr (day == AdventDay::one)
						{
							++region_fences;
						}

						if (!neighbour.is_checked())
						{
							regions_to_check.push_back(neighbour_loc);
						}
					}
					else if (!neighbour.is_checked())
					{
						squares_to_check.push_back(neighbour_loc);
					}
				}

				if constexpr (day == AdventDay::two)
				{
					for (std::size_t dir_idx : utils::int_range{ directions.size() })
					{
						std::size_t next_idx = (dir_idx + 1) % directions.size();
						const Dir dir = directions[dir_idx];
						const Dir next = directions[next_idx];
						const bool fence1 = square.has_fence(dir);
						const bool fence2 = square.has_fence(next);
						if (fence1 && fence2)
						{
							++region_fences;
						}
						if (!fence1 && !fence2)
						{
							const utils::coords diag_loc = loc + utils::coords::dir(dir) + utils::coords::dir(next);
							const Square diag = get_square(diag_loc);
							if (diag.get_id() != square.get_id())
							{
								// Concave corner
								++region_fences;
							}
						}
					}
				}

				square.mark_checked();
			}

			log << "\nRegion '" << region_id << "' Starting at " << region_loc << " - A=" << region_area << " ; F=" << region_fences;
			result += (region_area * region_fences);
		}
		return result;
	}

	template <AdventDay day>
	uint64_t solve_generic(std::istream& input)
	{
		Grid g = parse_grid(input);
		return get_overall_score<day>(std::move(g));
	}
}



ResultType day_twelve_p1_a(std::istream& input)
{
	return solve_generic<AdventDay::one>(input);
}

ResultType day_twelve_p2_a(std::istream& input)
{
	return solve_generic<AdventDay::two>(input);
}

ResultType advent_twelve_p1()
{
	auto input = advent::open_puzzle_input(12);
	return solve_generic<AdventDay::one>(input);
}

ResultType advent_twelve_p2()
{
	auto input = advent::open_puzzle_input(12);
	return solve_generic<AdventDay::two>(input);
}

#undef DAY12DBG
#undef ENABLE_DAY12DBG
