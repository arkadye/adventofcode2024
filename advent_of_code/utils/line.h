#pragma once

#include "advent/advent_assert.h"
#include "coords.h"
#include "range_contains.h"

#include <optional>

namespace utils
{
	template <typename CoordsT>
	class line
	{
		CoordsT a, b;

		template <typename ElemT>
		static std::optional<std::pair<ElemT,ElemT>> get_linear_overlap(ElemT static1, ElemT static2, ElemT low1, ElemT high1, ElemT low2, ElemT high2)
		{
			if (static1 != static2) return std::nullopt;
			if (low1 > high2) return std::nullopt;
			if (high1 < low2) return std::nullopt;
			const ElemT lowRet = std::max(low1, low2);
			const ElemT highRet = std::min(high1, high2);
			return std::pair{ lowRet,highRet };
		}

		std::optional<line> get_overlap_v_h(const line& other) const
		{
			if (!range_contains_inc(other.a.y, a.y, b.y)) return std::nullopt;
			if (!range_contains_inc(a.x, other.a.x, other.b.x)) return std::nullopt;
			const CoordsT point{ a.x,other.a.y };
			return line{ point,point };
		}
		std::optional<line> get_overlap_h_v(const line& other) const
		{
			return other.get_overlap_v_h(*this);
		}

		std::optional<line> get_overlap_h_h(const line& other) const
		{
			const auto result = get_linear_overlap(a.y, other.a.y, a.x, b.x, other.a.x, other.b.x);
			if (result.has_value())
			{
				return make_horizontal_line(a.y, result->first, result->second);
			}
			return std::nullopt;
		}

		std::optional<line> get_overlap_v_v(const line& other) const
		{
			const auto result = get_linear_overlap(a.x, other.a.x, a.y, b.y, other.a.y, other.b.y);
			if (result.has_value())
			{
				return make_vertical_line(a.x, result->first, result->second);
			}
			return std::nullopt;
		}

		bool is_vertical_impl() const { return a.x == b.x; }
		bool is_horizontal_impl() const { return a.y == b.y; }

	public:
		using coord_type = CoordsT;
		using elem_type = decltype(CoordsT{}.x);
		line(coord_type one_end, coord_type other_end) noexcept
			: a{ std::min(one_end.x,other_end.x), std::min(one_end.y, other_end.y) }
			, b{ std::max(one_end.x,other_end.x), std::max(one_end.y, other_end.y) }
		{
			AdventCheck(one_end.x == other_end.x || one_end.y == other_end.y);
		}

		line(const line&) noexcept = default;
		line& operator=(const line&) noexcept = default;
		~line() = default;

		static line make_vertical_line(elem_type x_pos, elem_type y_one_end, elem_type y_other_end)
		{
			return line{ CoordsT{x_pos,y_one_end}, CoordsT{x_pos,y_other_end} };
		}

		static line make_horizontal_line(elem_type y_pos, elem_type x_one_end, elem_type x_other_end)
		{
			return line{ CoordsT{x_one_end,y_pos}, CoordsT{x_other_end,y_pos} };
		}

		std::optional<line> get_crossing(const line& other) const
		{
			const bool my_v = is_vertical();
			const bool other_v = other.is_vertical();

			if (my_v && other_v)
			{
				return get_overlap_v_v(other);
			}
			if (my_v && !other_v)
			{
				return get_overlap_v_h(other);
			}
			if (!my_v && other_v)
			{
				return get_overlap_h_v(other);
			}
			if (!my_v && !other_v)
			{
				return get_overlap_h_h(other);
			}
			AdventUnreachable();
			return std::nullopt;
		}

		bool is_on_line(coord_type other) const
		{
			const std::optional<line> crossing_result = get_crossing(line{ other,other });
			return crossing_result.has_value();
		}

		coord_type operator[](std::size_t idx) const
		{
			AdventCheck(idx < size());
			coord_type result = a;
			(is_vertical() ? result.y : result.x) += static_cast<elem_type>(idx);
			return result;
		}

		elem_type length() const noexcept
		{
			return elem_type{ 1 } + (is_vertical() ? (b.y - a.y) : (b.x - a.x));
		}

		std::size_t size() const noexcept
		{
			return static_cast<std::size_t>(length());
		}

		bool is_vertical() const
		{
			const bool result = is_vertical_impl();
			if (!result)
			{
				AdventCheck(is_horizontal_impl());
			}
			return result;
		}
		bool is_horizontal() const
		{
			const bool result = is_horizontal_impl();
			if (!result)
			{
				AdventCheck(is_vertical_impl());
			}
			return result;
		}
	};
}