#pragma once

#ifdef NDEBUG
#ifdef UTILS_TESTING
#undef UTILS_TESTING
#endif
#define UTILS_TESTING 0
#endif

#ifndef UTILS_TESTING
#define UTILS_TESTING 1
#endif

#if UTILS_TESTING
#include "advent/advent_testcase_setup.h"

#include <vector>
#include <string>
#include <sstream>

namespace utils::testing
{
	std::vector<verification_test>& get_all_tests();
	struct TestAdder
	{
		TestAdder(std::string test_name, TestFunc test_func, std::string expected_result)
		{
			get_all_tests().push_back(make_test(std::move(test_name), std::move(test_func), std::move(expected_result)));
		}
	};

	enum class TestingState : char
	{
		initialized,
		made_by_copy,
		made_by_move,
		moved_from
	};

	struct TestingType
	{
	private:
		std::string data;
		TestingState state;
		TestingType(std::string in_data, TestingState in_state) : data{ std::move(in_data) }, state{ in_state } {}
	public:
		void check_valid() const;
		explicit TestingType(std::string input);
		explicit TestingType(std::string_view input) : TestingType{ std::string{input} } {}
		explicit TestingType(const char* input) : TestingType{ std::string{input} } {}
		TestingType(const TestingType& other);
		TestingType(TestingType&& other);
		TestingType& operator=(const TestingType& other);
		TestingType& operator=(TestingType&& other);

		std::string to_string() const;

		std::strong_ordering operator<=>(const TestingType& other) const;
		bool operator==(const TestingType& other) const;
	};

	std::string print_container(std::ranges::range auto&& container)
	{
		std::ostringstream oss;
		oss << '[';
		bool is_first = true;
		for (const auto& elem : container)
		{
			if (is_first)
			{
				is_first = false;
			}
			else
			{
				oss << ',';
			}
			oss << elem;
		}
		oss << ']';
		return oss.str();
	}
}

std::ostream& operator<<(std::ostream& os, const utils::testing::TestingType& tt);

#define DECLARE_UTILS_TEST(NAME, FUNC, RESULT) ResultType FUNC(); utils::testing::TestAdder FUNC ## _testcaseadder{NAME,FUNC,RESULT}

#else


#define DECLARE_UTILS_TEST(...)

#endif