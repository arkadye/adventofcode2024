#include "utils/tests/utils_tests.h"

#if UTILS_TESTING

#include "advent/advent_assert.h"

std::vector<verification_test>& utils::testing::get_all_tests()
{
	static std::vector<verification_test> singleton_test_vector;
	return singleton_test_vector;
}

void utils::testing::TestingType::check_valid() const
{
	AdventCheck(state != TestingState::moved_from);
}

utils::testing::TestingType::TestingType(std::string input)
	: TestingType{ std::move(input) , TestingState::initialized }
{
}

utils::testing::TestingType::TestingType(const TestingType& other)
	: TestingType{ other.data , TestingState::made_by_copy }
{
	other.check_valid();
}

utils::testing::TestingType::TestingType(TestingType&& other)
	: TestingType{ std::move(other.data) , TestingState::made_by_move }
{
	other.check_valid();
	other.state = TestingState::moved_from;
}

utils::testing::TestingType& utils::testing::TestingType::operator=(const TestingType& other)
{
	other.check_valid();
	data = other.data;
	state = TestingState::made_by_copy;
	return *this;
}

utils::testing::TestingType& utils::testing::TestingType::operator=(TestingType&& other)
{
	other.check_valid();
	data = other.data;
	state = TestingState::made_by_move;
	other.state = TestingState::moved_from;
	return *this;
}

std::string utils::testing::TestingType::to_string() const
{
	check_valid();
	return (state != TestingState::moved_from) ? data : std::string{ "ERROR - USE AFTER MOVE!" };
}

std::strong_ordering utils::testing::TestingType::operator<=>(const TestingType& other) const
{
	check_valid();
	other.check_valid();
	return (data <=> other.data);
}

bool utils::testing::TestingType::operator==(const TestingType& other) const
{
	return (operator<=>(other) == std::strong_ordering::equal);
}

std::ostream& operator<<(std::ostream& os, const utils::testing::TestingType& tt)
{
	os << tt.to_string();
	return os;
}

#endif