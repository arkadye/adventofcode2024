#pragma once

#include <sstream>
#include <string_view>

#include "advent/consteval_string.h"

namespace advent
{
	template <std::size_t NUM_NEWLINES, std::size_t FIRST_LEN>
	[[nodiscard]] inline constexpr auto combine_inputs(const consteval_string<FIRST_LEN> first)
	{
		return first;
	}

	template <std::size_t NUM_NEWLINES, std::size_t FIRST_LEN, typename...Strings>
	[[nodiscard]] inline constexpr auto combine_inputs(consteval_string<FIRST_LEN> first, Strings...remaining)
	{
		return first + consteval_string<NUM_NEWLINES+1>('\n') + combine_inputs<NUM_NEWLINES>(remaining...);
	}
}

/*

Define inline definitions of test inputs here for use in the testcases.

For example:

test_data TEST_ONE_A = "test1_input";

Use can also combine strings together with combine_inputs, for example, which will also add newlinse automatically using the template parameter to decide how many:

test_data TEST_ONE_B = "test2";
static const auto TEST_ONE = advent::combine_inputs<2>(TEST_ONE_A, TEST_ONE_B);
will set TEXT_ONE to "test1_input\n\ntest2"

*/

template <std::size_t S>
using test_data = advent::consteval_string<S>;

test_data DAY_ONE_A{
R"(3   4
4   3
2   5
1   3
3   9
3   3)" };

test_data DAY_TWO_A = "7 6 4 2 1";
test_data DAY_TWO_B = "1 2 7 8 9";
test_data DAY_TWO_C = "9 7 6 2 1";
test_data DAY_TWO_D = "1 3 2 4 5";
test_data DAY_TWO_E = "8 6 4 4 1";
test_data DAY_TWO_F = "1 3 6 7 9";
test_data DAY_TWO_G = advent::combine_inputs<1>(DAY_TWO_A, DAY_TWO_B, DAY_TWO_C, DAY_TWO_D, DAY_TWO_E, DAY_TWO_F);

test_data DAY_THREE_A = "xmul(2,4)%&mul[3,7]!@^do_not_mul(5,5)+mul(32,64]then(mul(11,8)mul(8,5))";
test_data DAY_THREE_B = "xmul(2,4)&mul[3,7]!^don't()_mul(5,5)+mul(32,64](mul(11,8)undo()?mul(8,5))";

test_data DAY_FOUR_A =
R"(..X...
.SAMX.
.A..A.
XMAS.S
.X....)";

test_data DAY_FOUR_B =
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

test_data DAY_FOUR_C =
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

test_data DAY_FOUR_D =
R"(M.S
.A.
M.S)";

test_data DAY_FOUR_E =
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

test_data DAY_FIVE_A =
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

test_data DAY_SIX_A =
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

test_data DAY_SEVEN_A = "190: 10 19";
test_data DAY_SEVEN_B = "3267 : 81 40 27";
test_data DAY_SEVEN_C = "83 : 17 5";
test_data DAY_SEVEN_D = "156 : 15 6";
test_data DAY_SEVEN_E = "7290 : 6 8 6 15";
test_data DAY_SEVEN_F = "161011 : 16 10 13";
test_data DAY_SEVEN_G = "192 : 17 8 14";
test_data DAY_SEVEN_H = "21037 : 9 7 18 13";
test_data DAY_SEVEN_I = "292 : 11 6 16 20";

test_data DAY_SEVEN_J =  advent::combine_inputs<1>(DAY_SEVEN_A, DAY_SEVEN_B, DAY_SEVEN_C, DAY_SEVEN_D, DAY_SEVEN_E, DAY_SEVEN_F, DAY_SEVEN_G, DAY_SEVEN_H, DAY_SEVEN_I);

test_data DAY_EIGHT_A =
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

test_data DAY_TEN_A =
R"(0123
1234
8765
9876)";

test_data DAY_TEN_B =
R"(...0...
...1...
...2...
6543456
7.....7
8.....8
9.....9)";

test_data DAY_TEN_C =
R"(..90..9
...1.98
...2..7
6543456
765.987
876....
987....)";

test_data DAY_TEN_D =
R"(10..9..
2...8..
3...7..
4567654
...8..3
...9..2
.....01)";

test_data DAY_TEN_E =
R"(89010123
78121874
87430965
96549874
45678903
32019012
01329801
10456732)";

test_data DAY_TEN_F =
R"(.....0.
..4321.
..5..2.
..6543.
..7..4.
..8765.
..9....)";

test_data DAY_TEN_G =
R"(..90..9
...1.98
...2..7
6543456
765.987
876....
987....)";

test_data DAY_TEN_H =
R"(012345
123456
234567
345678
4.6789
56789.)";

test_data DAY_ELEVEN_A = "0 1 10 99 999";
test_data DAY_ELEVEN_A_1 = "1 2024 1 0 9 9 2021976";

test_data DAY_ELEVEN_B = "125 17";

test_data DAY_TWELVE_A =
R"(AAAA
BBCD
BBCC
EEEC)";

test_data DAY_TWELVE_B =
R"(OOOOO
OXOXO
OOOOO
OXOXO
OOOOO)";

test_data DAY_TWELVE_C =
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

test_data DAY_TWELVE_D =
R"(EEEEE
EXXXX
EEEEE
EXXXX
EEEEE)";

test_data DAY_TWELVE_E =
R"(AAAAAA
AAABBA
AAABBA
ABBAAA
ABBAAA
AAAAAA)";

test_data DAY_THIRTEEN_A = R"(Button A: X+94, Y+34
Button B: X+22, Y+67
Prize: X=8400, Y=5400)";

test_data DAY_THIRTEEN_B = R"(Button A: X+26, Y+66
Button B: X+67, Y+21
Prize: X=12748, Y=12176)";

test_data DAY_THIRTEEN_C = R"(Button A: X+17, Y+86
Button B: X+84, Y+37
Prize: X=7870, Y=6450)";

test_data DAY_THIRTEEN_D = R"(Button A: X+69, Y+23
Button B: X+27, Y+71
Prize: X=18641, Y=10279)";

test_data DAY_THIRTEEN_E = advent::combine_inputs<2>(DAY_THIRTEEN_A, DAY_THIRTEEN_B, DAY_THIRTEEN_C, DAY_THIRTEEN_D);

test_data DAY_FIFTEEN_A =
R"(########
#..O.O.#
##@.O..#
#...O..#
#.#.O..#
#...O..#
#......#
########

<^^>>>vv<v>>v<<)";

test_data DAY_FIFTEEN_B =
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

test_data DAY_FIFTEEN_C =
R"(#######
#...#.#
#.....#
#..OO@#
#..O..#
#.....#
#######

<vv<<^^<<^^)";

test_data DAY_SIXTEEN_A =
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

test_data DAY_SIXTEEN_B =
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

test_data DAY_SEVENTEEN_A =
R"(Register A: 0
Register B: 0
Register C: 9

Program: 2,6)";

test_data DAY_SEVENTEEN_B =
R"(Register A: 10
Register B: 0
Register C: 0

Program: 5,0,5,1,5,4)";

test_data DAY_SEVENTEEN_C =
R"(Register A: 2024
Register B: 0
Register C: 0

Program: 0,1,5,4,3,0)";

test_data DAY_SEVENTEEN_D =
R"(Register A: 0
Register B: 29
Register C: 0

Program: 1,7)";

test_data DAY_SEVENTEEN_E =
R"(Register A: 0
Register B: 2024
Register C: 43690

Program: 4,0)";

test_data DAY_SEVENTEEN_F =
R"(Register A: 729
Register B: 0
Register C: 0

Program: 0,1,5,4,3,0)";

test_data DAY_EIGHTEEN_A =
R"(5,4
4,2
4,5
3,0
2,1
6,3
2,4
1,5
0,6
3,3
2,6
5,1
1,2
5,5
2,5
6,5
1,4
0,4
6,4
1,1
6,1
1,0
0,5
1,6
2,0)";

test_data DAY_NINETEEN_HEADER = "r, wr, b, g, bwu, rb, gb, br";

template <typename...Lines>
inline constexpr auto day_19_make_testcase(auto header, Lines...lines)
{
	const test_data body = advent::combine_inputs<1>(lines...);
	return advent::combine_inputs<2>(header, body);
}

test_data DAY_NINETEEN_LINE_A = "brwrr";
test_data DAY_NINETEEN_LINE_B = "bggr";
test_data DAY_NINETEEN_LINE_C = "gbbr";
test_data DAY_NINETEEN_LINE_D = "rrbgbr";
test_data DAY_NINETEEN_LINE_E = "ubwu";
test_data DAY_NINETEEN_LINE_F = "bwurrg";
test_data DAY_NINETEEN_LINE_G = "brgr";
test_data DAY_NINETEEN_LINE_H = "bbrgwb";

test_data DAY_NINETEEN_A = day_19_make_testcase(DAY_NINETEEN_HEADER, DAY_NINETEEN_LINE_A);
test_data DAY_NINETEEN_B = day_19_make_testcase(DAY_NINETEEN_HEADER, DAY_NINETEEN_LINE_B);
test_data DAY_NINETEEN_C = day_19_make_testcase(DAY_NINETEEN_HEADER, DAY_NINETEEN_LINE_C);
test_data DAY_NINETEEN_D = day_19_make_testcase(DAY_NINETEEN_HEADER, DAY_NINETEEN_LINE_D);
test_data DAY_NINETEEN_E = day_19_make_testcase(DAY_NINETEEN_HEADER, DAY_NINETEEN_LINE_E);
test_data DAY_NINETEEN_F = day_19_make_testcase(DAY_NINETEEN_HEADER, DAY_NINETEEN_LINE_F);
test_data DAY_NINETEEN_G = day_19_make_testcase(DAY_NINETEEN_HEADER, DAY_NINETEEN_LINE_G);
test_data DAY_NINETEEN_H = day_19_make_testcase(DAY_NINETEEN_HEADER, DAY_NINETEEN_LINE_H);
test_data DAY_NINETEEN_I = day_19_make_testcase(DAY_NINETEEN_HEADER,
	DAY_NINETEEN_LINE_A,
	DAY_NINETEEN_LINE_B,
	DAY_NINETEEN_LINE_C,
	DAY_NINETEEN_LINE_D,
	DAY_NINETEEN_LINE_E,
	DAY_NINETEEN_LINE_F,
	DAY_NINETEEN_LINE_G,
	DAY_NINETEEN_LINE_H);

test_data DAY_TWENTY_A =
R"(###############
#...#...#.....#
#.#.#.#.#.###.#
#S#...#.#.#...#
#######.#.#.###
#######.#.#...#
#######.#.###.#
###..E#...#...#
###.#######.###
#...###...#...#
#.#####.#.###.#
#.#...#.#.#...#
#.#.#.#.#.#.###
#...#...#...###
###############)";

test_data DAY_TWENTYTWO_A = "123";
test_data DAY_TWENTYTWO_B = "1";
test_data DAY_TWENTYTWO_C = "10";
test_data DAY_TWENTYTWO_D = "100";
test_data DAY_TWENTYTWO_E = "2024";
test_data DAY_TWENTYTWO_F = advent::combine_inputs<1>(DAY_TWENTYTWO_B, DAY_TWENTYTWO_C, DAY_TWENTYTWO_D, DAY_TWENTYTWO_E);

test_data DAY_TWENTYTWO_G = "1\n2\n3\n2024";

test_data DAY_TWENTYTHREE_A =
R"(kh-tc
qp-kh
de-cg
ka-co
yn-aq
qp-ub
cg-tb
vc-aq
tb-ka
wh-tc
yn-cg
kh-ub
ta-co
de-co
tc-td
tb-wq
wh-td
ta-ka
td-qp
aq-cg
wq-ub
ub-vc
de-ta
wq-aq
wq-vc
wh-yn
ka-de
kh-ta
co-tc
wh-qp
tb-vc
td-yn)";

test_data DAY_TWENTYFOUR_A =
R"(x00: 1
x01: 1
x02: 1
y00: 0
y01: 1
y02: 0

x00 AND y00 -> z00
x01 XOR y01 -> z01
x02 OR y02 -> z02)";

test_data DAY_TWENTYFOUR_B =
R"(x00: 1
x01: 0
x02: 1
x03: 1
x04: 0
y00: 1
y01: 1
y02: 1
y03: 1
y04: 1

ntg XOR fgs -> mjb
y02 OR x01 -> tnw
kwq OR kpj -> z05
x00 OR x03 -> fst
tgd XOR rvg -> z01
vdt OR tnw -> bfw
bfw AND frj -> z10
ffh OR nrd -> bqk
y00 AND y03 -> djm
y03 OR y00 -> psh
bqk OR frj -> z08
tnw OR fst -> frj
gnj AND tgd -> z11
bfw XOR mjb -> z00
x03 OR x00 -> vdt
gnj AND wpb -> z02
x04 AND y00 -> kjc
djm OR pbm -> qhw
nrd AND vdt -> hwm
kjc AND fst -> rvg
y04 OR y02 -> fgs
y01 AND x02 -> pbm
ntg OR kjc -> kwq
psh XOR fgs -> tgd
qhw XOR tgd -> z09
pbm OR djm -> kpj
x03 XOR y03 -> ffh
x00 XOR y04 -> ntg
bfw OR bqk -> z06
nrd XOR fgs -> wpb
frj XOR qhw -> z04
bqk OR frj -> z07
y03 OR x01 -> nrd
hwm AND bqk -> z03
tgd XOR rvg -> z12
tnw OR pbm -> gnj)";