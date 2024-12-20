#pragma once

#include <string>
#include <string_view>

namespace advent
{
	template <std::size_t LEN>
	class consteval_string
	{
		template <std::size_t A, std::size_t B>
		friend constexpr consteval_string<A + B - 1> combine(consteval_string<A> a, consteval_string<B> b);

		char m_data[LEN];
		constexpr consteval_string() = default;
		constexpr static void cpy(char* dest, const char* src, std::size_t size)
		{
			for (std::size_t i{ 0u }; i < size; ++i)
			{
				dest[i] = src[i];
			}
		}
	public:
		constexpr consteval_string(const char(&init)[LEN])
		{
			cpy(m_data, init, LEN);
		}

		constexpr explicit consteval_string(char init)
		{
			for (std::size_t i{ 0u }; i < LEN-1; ++i)
			{
				m_data[i] = init;
			}
			m_data[LEN-1] = '\0';
		}

		constexpr consteval_string(const consteval_string& other) = default;

		operator std::string() const { return std::string(m_data, m_data + LEN - 1); }
		constexpr operator std::string_view() const { return std::string_view(m_data, m_data + LEN - 1); }
	};

	template <std::size_t A, std::size_t B>
	constexpr consteval_string<A + B - 1> combine(consteval_string<A> a, consteval_string<B> b)
	{
		consteval_string<A + B - 1> result;
		result.cpy(result.m_data, a.m_data, A - 1);
		result.cpy(result.m_data + A-1, b.m_data, B);
		return result;
	}
}

template <std::size_t LEN_L, std::size_t LEN_R>
constexpr advent::consteval_string<LEN_L + LEN_R - 1> operator+(advent::consteval_string<LEN_L> l, advent::consteval_string<LEN_R> r)
{
	return advent::combine(l, r);
}