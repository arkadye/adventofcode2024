#pragma once

#include "int_range.h"

#include <limits>
#include <concepts>
#include <type_traits>

namespace utils
{
	template <std::unsigned_integral T>
	inline std::size_t count_digits(T num)
	{
		const std::size_t max = std::numeric_limits<T>::digits10;
		T limit = 10;
		for (auto result : utils::int_range<std::size_t>{ 1,max })
		{
			if (num < limit) return result;
			limit *= 10;
		}
		return max;
	}

	template <std::signed_integral T>
	inline std::size_t count_digits(T num)
	{
		using Unsigned = std::make_unsigned_t<T>;
		num = std::abs(num);
		Unsigned unsigned_num = static_cast<Unsigned>(num);
		return count_digits(unsigned_num);
	}
}