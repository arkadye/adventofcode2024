#pragma once

#include <cstdint>
#include <concepts>

namespace utils
{
	template <std::unsigned_integral T>
	T isqrt(T input) noexcept;

	template <std::signed_integral T>
	T isqrt(T input) noexcept;

	template <>
	uint64_t isqrt(uint64_t input) noexcept;

	template <>
	int64_t isqrt(int64_t input) noexcept;

	template <std::signed_integral T>
	T isqrt(T input) noexcept
	{
		return static_cast<T>(isqrt(static_cast<int64_t>(input)));
	}

	template <std::unsigned_integral T>
	T isqrt(T input) noexcept
	{
		return static_cast<T>(isqrt(static_cast<uint64_t>(input)));
	}

	bool is_square(uint64_t input) noexcept;
	bool is_square(int64_t input) noexcept;
}