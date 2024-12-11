#pragma once

#include <iterator>
#include <ranges>

namespace utils
{
	template <std::forward_iterator FwdIt>
	bool has_duplicates(FwdIt start_it, FwdIt auto end_it)
	{
		if (start_it == end_it) return false;
		const auto next_it = [start_it]()
		{
			auto result = start_it;
			std::advance(result, 1);
			return result;
		}();

		for (auto it = next_it; it != end_it; std::advance(it,1))
		{
			if (*start_it == *it) return true;
		}

		return has_duplicates(next_it, end_it);
	}

	namespace ranges
	{
		template <std::ranges::forward_range ContainerType>
		bool has_duplicates(const ContainerType& container)
		{
			return utils::has_duplicates(std::cbegin(container), std::cend(container));
		}
	}
}