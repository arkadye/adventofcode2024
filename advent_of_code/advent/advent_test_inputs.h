#pragma once

#include <sstream>
#include <string_view>

namespace advent
{
	namespace combine_inputs_internal
	{
		inline std::string impl(std::string_view joiner, std::ostringstream& collector)
		{
			return collector.str();
		}

		template <typename...StringViews>
		inline std::string impl(std::string_view joiner, std::ostringstream& collector, std::string_view next_string, StringViews...remaining)
		{
			collector << joiner << next_string;
			return impl(joiner, collector,remaining...);
		}
	}

	template <std::size_t NUM_NEWLINES, typename...StringViews>
	inline std::string combine_inputs(std::string_view first_string, StringViews...remaining)
	{
		std::ostringstream collector;
		std::string joiner(NUM_NEWLINES, '\n');
		collector << first_string;
		return combine_inputs_internal::impl(joiner, collector, remaining...);
	}
}

/*

Define inline definitions of test inputs here for use in the testcases.

For example:

constexpr const char* TEST_ONE_A = "test1_input";

Use can also combine strings together with combine_inputs, for example, which will also add newlinse automatically using the template parameter to decide how many:

constexpr const char* TEST_ONE_B = "test2";
static const auto TEST_ONE = advent::combine_inputs<2>(TEST_ONE_A, TEST_ONE_B);
will set TEXT_ONE to "test1_input\n\ntest2"

*/

constexpr const char* DAY_ONE_A = 
R"(3   4
4   3
2   5
1   3
3   9
3   3)";

constexpr const char* DAY_TWO_A = "7 6 4 2 1";
constexpr const char* DAY_TWO_B = "1 2 7 8 9";
constexpr const char* DAY_TWO_C = "9 7 6 2 1";
constexpr const char* DAY_TWO_D = "1 3 2 4 5";
constexpr const char* DAY_TWO_E = "8 6 4 4 1";
constexpr const char* DAY_TWO_F = "1 3 6 7 9";
static const auto DAY_TWO_G = advent::combine_inputs<1>(DAY_TWO_A, DAY_TWO_B, DAY_TWO_C, DAY_TWO_D, DAY_TWO_E, DAY_TWO_F);

constexpr const char* DAY_THREE_A = "xmul(2,4)%&mul[3,7]!@^do_not_mul(5,5)+mul(32,64]then(mul(11,8)mul(8,5))";
constexpr const char* DAY_THREE_B = "xmul(2,4)&mul[3,7]!^don't()_mul(5,5)+mul(32,64](mul(11,8)undo()?mul(8,5))";

constexpr const char* DAY_FOUR_A =
R"(..X...
.SAMX.
.A..A.
XMAS.S
.X....)";

constexpr const char* DAY_FOUR_B =
R"(MMMSXXMASM
MSAMXMSMSA
AMXSXMAAMM
MSAMASMSMX
XMASAMXAMM
XXAMMXXAMA
SMSMSASXSS
SAXAMASAAA
MAMMMXMMMM
MXMXAXMASX)";

constexpr const char* DAY_FOUR_C =
R"(MMMSXXMASM
MSAMXMSMSA
AMXSXMAAMM
MSAMASMSMX
XMASAMXAMM
XXAMMXXAMA
SMSMSASXSS
SAXAMASAAA
MAMMMXMMMM
MXMXAXMASX)";

constexpr const char* DAY_FOUR_D =
R"(M.S
.A.
M.S)";

constexpr const char* DAY_FOUR_E =
R"(.M.S......
..A..MSMS.
.M.S.MAA..
..A.ASMSM.
.M.S.M....
..........
S.S.S.S.S.
.A.A.A.A..
M.M.M.M.M.
..........)";

constexpr const char* DAY_FIVE_A =
R"(47|53
97|13
97|61
97|47
75|29
61|13
75|53
29|13
97|29
53|29
61|53
97|53
61|29
47|13
75|47
97|75
47|61
75|61
47|29
75|13
53|13

75,47,61,53,29
97,61,53,29,13
75,29,13
75,97,47,61,53
61,13,29
97,13,75,29,47)";

constexpr const char* DAY_SIX_A =
R"(....#.....
.........#
..........
..#.......
.......#..
..........
.#..^.....
........#.
#.........
......#...)";

constexpr const char* DAY_SEVEN_A = "190: 10 19";
constexpr const char* DAY_SEVEN_B = "3267 : 81 40 27";
constexpr const char* DAY_SEVEN_C = "83 : 17 5";
constexpr const char* DAY_SEVEN_D = "156 : 15 6";
constexpr const char* DAY_SEVEN_E = "7290 : 6 8 6 15";
constexpr const char* DAY_SEVEN_F = "161011 : 16 10 13";
constexpr const char* DAY_SEVEN_G = "192 : 17 8 14";
constexpr const char* DAY_SEVEN_H = "21037 : 9 7 18 13";
constexpr const char* DAY_SEVEN_I = "292 : 11 6 16 20";

static const auto DAY_SEVEN_J = advent::combine_inputs<1>(DAY_SEVEN_A, DAY_SEVEN_B, DAY_SEVEN_C, DAY_SEVEN_D, DAY_SEVEN_E, DAY_SEVEN_F, DAY_SEVEN_G, DAY_SEVEN_H, DAY_SEVEN_I);

constexpr const char* DAY_EIGHT_A =
R"(............
........0...
.....0......
.......0....
....0.......
......A.....
............
............
........A...
.........A..
............
............)";

constexpr const char* DAY_TEN_A =
R"(0123
1234
8765
9876)";

constexpr const char* DAY_TEN_B =
R"(...0...
...1...
...2...
6543456
7.....7
8.....8
9.....9)";

constexpr const char* DAY_TEN_C =
R"(..90..9
...1.98
...2..7
6543456
765.987
876....
987....)";

constexpr const char* DAY_TEN_D =
R"(10..9..
2...8..
3...7..
4567654
...8..3
...9..2
.....01)";

constexpr const char* DAY_TEN_E =
R"(89010123
78121874
87430965
96549874
45678903
32019012
01329801
10456732)";

constexpr const char* DAY_TEN_F =
R"(.....0.
..4321.
..5..2.
..6543.
..7..4.
..8765.
..9....)";

constexpr const char* DAY_TEN_G =
R"(..90..9
...1.98
...2..7
6543456
765.987
876....
987....)";

constexpr const char* DAY_TEN_H =
R"(012345
123456
234567
345678
4.6789
56789.)";

constexpr const char* DAY_ELEVEN_A = "0 1 10 99 999";
#define DAY_ELEVEN_A_1 "1 2024 1 0 9 9 2021976"

constexpr const char* DAY_ELEVEN_B = "125 17";

constexpr const char* DAY_TWELVE_A =
R"(AAAA
BBCD
BBCC
EEEC)";

constexpr const char* DAY_TWELVE_B =
R"(OOOOO
OXOXO
OOOOO
OXOXO
OOOOO)";

constexpr const char* DAY_TWELVE_C =
R"(RRRRIICCFF
RRRRIICCCF
VVRRRCCFFF
VVRCCCJFFF
VVVVCJJCFE
VVIVCCJJEE
VVIIICJJEE
MIIIIIJJEE
MIIISIJEEE
MMMISSJEEE)";

constexpr const char* DAY_TWELVE_D =
R"(EEEEE
EXXXX
EEEEE
EXXXX
EEEEE)";

constexpr const char* DAY_TWELVE_E =
R"(AAAAAA
AAABBA
AAABBA
ABBAAA
ABBAAA
AAAAAA)";

constexpr const char* DAY_THIRTEEN_A = R"(Button A: X+94, Y+34
Button B: X+22, Y+67
Prize: X=8400, Y=5400)";

constexpr const char* DAY_THIRTEEN_B = R"(Button A: X+26, Y+66
Button B: X+67, Y+21
Prize: X=12748, Y=12176)";

constexpr const char* DAY_THIRTEEN_C = R"(Button A: X+17, Y+86
Button B: X+84, Y+37
Prize: X=7870, Y=6450)";

constexpr const char* DAY_THIRTEEN_D = R"(Button A: X+69, Y+23
Button B: X+27, Y+71
Prize: X=18641, Y=10279)";

static const auto DAY_THIRTEEN_E = advent::combine_inputs<2>(DAY_THIRTEEN_A, DAY_THIRTEEN_B, DAY_THIRTEEN_C, DAY_THIRTEEN_D);

constexpr const char* DAY_FIFTEEN_A =
R"(########
#..O.O.#
##@.O..#
#...O..#
#.#.O..#
#...O..#
#......#
########

<^^>>>vv<v>>v<<)";

constexpr const char* DAY_FIFTEEN_B =
R"(##########
#..O..O.O#
#......O.#
#.OO..O.O#
#..O@..O.#
#O#..O...#
#O..O..O.#
#.OO.O.OO#
#....O...#
##########

<vv>^<v^>v>^vv^v>v<>v^v<v<^vv<<<^><<><>>v<vvv<>^v^>^<<<><<v<<<v^vv^v>^
vvv<<^>^v^^><<>>><>^<<><^vv^^<>vvv<>><^^v>^>vv<>v<<<<v<^v>^<^^>>>^<v<v
><>vv>v^v^<>><>>>><^^>vv>v<^^^>>v^v^<^^>v^^>v^<^v>v<>>v^v^<v>v^^<^^vv<
<<v<^>>^^^^>>>v^<>vvv^><v<<<>^^^vv^<vvv>^>v<^^^^v<>^>vvvv><>>v^<<^^^^^
^><^><>>><>^^<<^^v>>><^<v>^<vv>>v>>>^v><>^v><<<<v>>v<v<v>vvv>^<><<>^><
^>><>^v<><^vvv<^^<><v<<<<<><^v<<<><<<^^<v<^^^><^>>^<v^><<<^>>^v<v^v<v^
>^>>^v>vv>^<<^v<>><<><<v<<v><>v<^vv<<<>^^v^>^^>>><<^v>>v^v><^^>>^<>vv^
<><^^>^^^<><vvvvv^v<v<<>^v<v>v<<^><<><<><<<^^<<<^<<>><<><^^^>^^<>^>v<>
^^>vv<^v^v<vv>^<><v<^v>^^^>>>^^vvv^>vvv<>>>^<^>>>>>^<<^v>^vvv<>^<><<v>
v^^>>><<^^<>>^v^<v^vv<>v^<<>^<^v^v><^<<<><<^<v><v<>vv>>v><v^<vv<>v^<<^)";

constexpr const char* DAY_SIXTEEN_A =
R"(###############
#.......#....E#
#.#.###.#.###.#
#.....#.#...#.#
#.###.#####.#.#
#.#.#.......#.#
#.#.#####.###.#
#...........#.#
###.#.#####.#.#
#...#.....#.#.#
#.#.#.###.#.#.#
#.....#...#.#.#
#.###.#.#.#.#.#
#S..#.....#...#
###############)";

constexpr const char* DAY_SIXTEEN_B =
R"(#################
#...#...#...#..E#
#.#.#.#.#.#.#.#.#
#.#.#.#...#...#.#
#.#.#.#.###.#.#.#
#...#.#.#.....#.#
#.#.#.#.#.#####.#
#.#...#.#.#.....#
#.#.#####.#.###.#
#.#.#.......#...#
#.#.###.#####.###
#.#.#...#.....#.#
#.#.#.#####.###.#
#.#.#.........#.#
#.#.#.#########.#
#S#.............#
#################)";
