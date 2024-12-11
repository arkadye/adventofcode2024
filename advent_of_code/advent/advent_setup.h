#pragma once

#include "advent_testcase_setup.h"
#include "advent_test_inputs.h"
#include "advent_headers.h"
#include "advent_solutions.h"

static const verification_test tests[] =
{
	TESTCASE_WITH_ARG(day_one_p1_a, DAY_ONE_A, 11),
	TESTCASE_WITH_ARG(day_one_p2_a, DAY_ONE_A, 31),
	DAY(one,DAY_01_1_SOLUTION,DAY_01_2_SOLUTION),
	TESTCASE_WITH_ARG(day_two_p1_a,DAY_TWO_A, 1),
	TESTCASE_WITH_ARG(day_two_p1_a,DAY_TWO_B, 0),
	TESTCASE_WITH_ARG(day_two_p1_a,DAY_TWO_C, 0),
	TESTCASE_WITH_ARG(day_two_p1_a,DAY_TWO_D, 0),
	TESTCASE_WITH_ARG(day_two_p1_a,DAY_TWO_E, 0),
	TESTCASE_WITH_ARG(day_two_p1_a,DAY_TWO_F, 1),
	TESTCASE_WITH_ARG(day_two_p1_a,DAY_TWO_G, 2),
	TESTCASE_WITH_ARG(day_two_p2_a,DAY_TWO_A, 1),
	TESTCASE_WITH_ARG(day_two_p2_a,DAY_TWO_B, 0),
	TESTCASE_WITH_ARG(day_two_p2_a,DAY_TWO_C, 0),
	TESTCASE_WITH_ARG(day_two_p2_a,DAY_TWO_D, 1),
	TESTCASE_WITH_ARG(day_two_p2_a,DAY_TWO_E, 1),
	TESTCASE_WITH_ARG(day_two_p2_a,DAY_TWO_F, 1),
	TESTCASE_WITH_ARG(day_two_p2_a,DAY_TWO_G, 4),
	DAY(two,DAY_02_1_SOLUTION,DAY_02_2_SOLUTION),
	TESTCASE_WITH_ARG(day_three_p1_a, "mul(44,46)", 2024),
	TESTCASE_WITH_ARG(day_three_p1_a, "mul(123,4)", 492),
	TESTCASE_WITH_ARG(day_three_p1_a, "mul(4*", 0),
	TESTCASE_WITH_ARG(day_three_p1_a, "mul(6,9!", 0),
	TESTCASE_WITH_ARG(day_three_p1_a, "?(12,34)", 0),
	TESTCASE_WITH_ARG(day_three_p1_a, "mul ( 2 , 4 )", 0),
	TESTCASE_WITH_ARG(day_three_p1_a, DAY_THREE_A, 161),
	TESTCASE_WITH_ARG(day_three_p2_a, DAY_THREE_B, 48),
	DAY(three,DAY_03_1_SOLUTION,DAY_03_2_SOLUTION),
	TESTCASE_WITH_ARG(day_four_p1_a, DAY_FOUR_A, 4),
	TESTCASE_WITH_ARG(day_four_p1_a, DAY_FOUR_B, 18),
	TESTCASE_WITH_ARG(day_four_p1_a, DAY_FOUR_C, 18),
	TESTCASE_WITH_ARG(day_four_p2_a, DAY_FOUR_D, 1),
	TESTCASE_WITH_ARG(day_four_p2_a, DAY_FOUR_E, 9),
	DAY(four,DAY_04_1_SOLUTION,DAY_04_2_SOLUTION),
	TESTCASE_WITH_ARG(day_five_p1_a, DAY_FIVE_A, 143),
	TESTCASE_WITH_ARG(day_five_p2_a, DAY_FIVE_A, 123),
	DAY(five,DAY_05_1_SOLUTION,DAY_05_2_SOLUTION),
	TESTCASE_WITH_ARG(day_six_p1_a,DAY_SIX_A, 41),
	TESTCASE_WITH_ARG(day_six_p2_a,DAY_SIX_A, 6),
	DAY(six,DAY_06_1_SOLUTION,DAY_06_2_SOLUTION),
	TESTCASE_WITH_ARG(day_seven_p1_a, DAY_SEVEN_A,190),
	TESTCASE_WITH_ARG(day_seven_p1_a, DAY_SEVEN_B,3267),
	TESTCASE_WITH_ARG(day_seven_p1_a, DAY_SEVEN_C,0),
	TESTCASE_WITH_ARG(day_seven_p1_a, DAY_SEVEN_D,0),
	TESTCASE_WITH_ARG(day_seven_p1_a, DAY_SEVEN_E,0),
	TESTCASE_WITH_ARG(day_seven_p1_a, DAY_SEVEN_F,0),
	TESTCASE_WITH_ARG(day_seven_p1_a, DAY_SEVEN_G,0),
	TESTCASE_WITH_ARG(day_seven_p1_a, DAY_SEVEN_H,0),
	TESTCASE_WITH_ARG(day_seven_p1_a, DAY_SEVEN_I,292),
	TESTCASE_WITH_ARG(day_seven_p1_a, DAY_SEVEN_J,3749),	
	TESTCASE_WITH_ARG(day_seven_p2_a, DAY_SEVEN_A,190),
	TESTCASE_WITH_ARG(day_seven_p2_a, DAY_SEVEN_B,3267),
	TESTCASE_WITH_ARG(day_seven_p2_a, DAY_SEVEN_C,0),
	TESTCASE_WITH_ARG(day_seven_p2_a, DAY_SEVEN_D,156),
	TESTCASE_WITH_ARG(day_seven_p2_a, DAY_SEVEN_E,7290),
	TESTCASE_WITH_ARG(day_seven_p2_a, DAY_SEVEN_F,0),
	TESTCASE_WITH_ARG(day_seven_p2_a, DAY_SEVEN_G,192),
	TESTCASE_WITH_ARG(day_seven_p2_a, DAY_SEVEN_H,0),
	TESTCASE_WITH_ARG(day_seven_p2_a, DAY_SEVEN_I,292),
	TESTCASE_WITH_ARG(day_seven_p2_a, DAY_SEVEN_J,11387),
	DAY(seven,DAY_07_1_SOLUTION,DAY_07_2_SOLUTION),
	TESTCASE_WITH_ARG(day_eight_p1_a, DAY_EIGHT_A, 14),
	TESTCASE_WITH_ARG(day_eight_p2_a, DAY_EIGHT_A, 34),
	DAY(eight,DAY_08_1_SOLUTION,DAY_08_2_SOLUTION),
	TESTCASE(day_nine_p1_a, 60),
	TESTCASE(day_nine_p1_b, 1928),
	TESTCASE(day_nine_p2_a, 132),
	TESTCASE(day_nine_p2_b, 2858),
	DAY(nine,DAY_09_1_SOLUTION,DAY_09_2_SOLUTION),
	TESTCASE_WITH_ARG(day_ten_p1_a, DAY_TEN_A, 1),
	TESTCASE_WITH_ARG(day_ten_p1_a, DAY_TEN_B, 2),
	TESTCASE_WITH_ARG(day_ten_p1_a, DAY_TEN_C, 4),
	TESTCASE_WITH_ARG(day_ten_p1_a, DAY_TEN_D, 3),
	TESTCASE_WITH_ARG(day_ten_p1_a, DAY_TEN_E, 36),
	TESTCASE_WITH_ARG(day_ten_p2_a, DAY_TEN_F, 3),
	TESTCASE_WITH_ARG(day_ten_p2_a, DAY_TEN_G, 13),
	TESTCASE_WITH_ARG(day_ten_p2_a, DAY_TEN_H, 227),
	TESTCASE_WITH_ARG(day_ten_p2_a, DAY_TEN_E, 81),
	DAY(ten, DAY_10_1_SOLUTION, DAY_10_2_SOLUTION),
	TESTCASE_WITH_ARG(day_eleven_p1_a<1>, DAY_ELEVEN_A, 7),
	TESTCASE_WITH_ARG(day_eleven_p1_a<1>, DAY_ELEVEN_B, 3),
	TESTCASE_WITH_ARG(day_eleven_p1_a<2>, DAY_ELEVEN_B, 4),
	TESTCASE_WITH_ARG(day_eleven_p1_a<3>, DAY_ELEVEN_B, 5),
	TESTCASE_WITH_ARG(day_eleven_p1_a<4>, DAY_ELEVEN_B, 9),
	TESTCASE_WITH_ARG(day_eleven_p1_a<5>, DAY_ELEVEN_B, 13),
	TESTCASE_WITH_ARG(day_eleven_p1_a<6>, DAY_ELEVEN_B, 22),
	TESTCASE_WITH_ARG(day_eleven_p1_a<25>, DAY_ELEVEN_B, 55312),
	DAY(eleven, DAY_11_1_SOLUTION, DAY_11_2_SOLUTION),
	DAY(twelve, DAY_12_1_SOLUTION, DAY_12_2_SOLUTION),
	DAY(thirteen, DAY_13_1_SOLUTION, DAY_13_2_SOLUTION),
	DAY(fourteen, DAY_14_1_SOLUTION, DAY_14_2_SOLUTION),
	DAY(fifteen, DAY_15_1_SOLUTION, DAY_15_2_SOLUTION),
	DAY(sixteen, DAY_16_1_SOLUTION, DAY_16_2_SOLUTION),
	DAY(seventeen, DAY_17_1_SOLUTION, DAY_17_2_SOLUTION),
	DAY(eighteen, DAY_18_1_SOLUTION, DAY_18_2_SOLUTION),
	DAY(nineteen, DAY_19_1_SOLUTION, DAY_19_2_SOLUTION),
	DAY(twenty, DAY_20_1_SOLUTION, DAY_20_2_SOLUTION),
	DAY(twentyone, DAY_21_1_SOLUTION, DAY_21_2_SOLUTION),
	DAY(twentytwo, DAY_22_1_SOLUTION, DAY_22_2_SOLUTION),
	DAY(twentythree, DAY_23_1_SOLUTION, DAY_23_2_SOLUTION),
	DAY(twentyfour, DAY_24_1_SOLUTION, DAY_24_2_SOLUTION),
	DAY(twentyfive, DAY_25_1_SOLUTION,"MERRY CHRISTMAS!")
};

#undef ARG
#undef TESTCASE
#undef FUNC_NAME
#undef TEST_DECL
#undef DAY
#undef DUMMY
#undef DUMMY_DAY
