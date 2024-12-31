#include "utils/tests/small_vector_tests.h"

#if UTILS_TESTING

#include "utils/small_vector.h"

using utils::testing::TestingType;

template <typename SMALL_VECTOR_TYPE>
ResultType insert_test_impl(SMALL_VECTOR_TYPE& target)
{
	auto insert_it = stdr::find(target, TestingType{ "cc" });
	target.insert(insert_it, TestingType{ "bb" });
	return print_container(target);
}

ResultType small_vector_insert_non_trivial_at_location_stack()
{
	utils::small_vector<TestingType, 3> data{ TestingType{"aa"},TestingType{"cc"} };
	return insert_test_impl(data);
}

ResultType small_vector_insert_at_location_heap_no_realloc()
{
	utils::small_vector<TestingType, 1> data;
	data.reserve(3);
	data.emplace_back("aa");
	data.emplace_back("cc");
	return insert_test_impl(data);
}

ResultType small_vector_insert_at_location_heap_with_realloc()
{
	utils::small_vector<TestingType, 2> data;
	data.reserve(3);
	data.emplace_back("aa");
	data.emplace_back("cc");
	return insert_test_impl(data);
}

#endif