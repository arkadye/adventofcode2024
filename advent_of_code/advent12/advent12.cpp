#include "advent12.h"
#include "advent/advent_utils.h"

#ifdef FORCE_DAY12DBG
#define DAY12DBG 1
#else
#define ENABLE_DAY12DBG 1
#ifdef NDEBUG
#define DAY12DBG 0
#else
#define DAY12DBG ENABLE_DAY12DBG
#endif
#endif

#if DAY12DBG
	#include <iostream>
#endif

namespace
{
#if DAY12DBG
	std::ostream& log = std::cout;
#else
	struct {	template <typename T> auto& operator<<(const T&) const noexcept { return *this; } } log;
#endif
}

#include "coords.h"
#include "enums.h"

#include <variant>
#include <list>
#include <ranges>

namespace
{
	using RegionId = char;
	using Dir = utils::direction;

	struct RegionData
	{
		RegionId id = '\0';
		std::size_t area = 0u;
		std::size_t perimeter = 0u;
		void combine(const RegionData& other)
		{
			AdventCheck(id == other.id);
			area += other.area;
			perimeter += other.perimeter;
		}
		std::size_t evaluate() const { return area * perimeter; }
	};

	class Region
	{
		std::variant<RegionData, Region*> m_data_or_pointer_to_parent;
	public:
		Region(RegionId id, std::size_t area, std::size_t perimeter) : m_data_or_pointer_to_parent{ RegionData{id,area,perimeter} } {}
		bool is_root() const { return std::holds_alternative<RegionData>(m_data_or_pointer_to_parent); }

		const RegionData& get_region_data() const
		{
			struct Getter
			{
				const RegionData& operator()(const RegionData& rd) { return rd; }
				const RegionData& operator()(Region* region) { AdventCheck(region != nullptr); return region->get_region_data(); }
			};
			return std::visit(Getter{}, m_data_or_pointer_to_parent);
		}

		RegionData& get_region_data()
		{
			const Region& const_this = const_cast<const Region&>(*this);
			const RegionData& const_result = const_this.get_region_data();
			return const_cast<RegionData&>(const_result);
		}

		Region& get_top_level()
		{
			struct Getter
			{
				explicit Getter(Region& r) : self{ r } {}
				Region& self;
				Region& operator()(const RegionData& rd) { return self; }
				Region& operator()(Region* region) { AdventCheck(region != nullptr); return region->get_top_level(); }
			};
			return std::visit(Getter{*this}, m_data_or_pointer_to_parent);
		}

		void set_parent(Region& new_parent)
		{
			if (Region** parent_ptr = std::get_if<Region*>(&m_data_or_pointer_to_parent))
			{
				Region* parent = *parent_ptr;
				AdventCheck(parent != nullptr);
				parent->set_parent(new_parent);
			}
			m_data_or_pointer_to_parent = &new_parent;
		}

		void merge_with(Region& other)
		{
			Region& top_region = get_top_level();
			Region& other_top = other.get_top_level();
			if (&top_region == &other_top) return; // Already merged.
			RegionData& my_data = top_region.get_region_data();
			const RegionData& other_data = other_top.get_region_data();
			my_data.combine(other_data);
			other.set_parent(*this);
		}

		std::size_t evaluate() const
		{
			struct Evaluator
			{
				std::size_t operator()(Region*) { return 0u; }
				std::size_t operator()(const RegionData& rd) { return rd.evaluate(); }
			};

			return std::visit(Evaluator{}, m_data_or_pointer_to_parent);
		}
	};

	std::ostream& operator<<(std::ostream& os, const Region& r)
	{
		os << r.get_region_data().id << "[0x" << &r.get_region_data() << ']';
		return os;
	}

	class Square
	{
		Region& region;
		std::array<bool, 4> fence_sides{ false,false,false,false };
	public:
		explicit Square(Region& r) : region{ r } {}
		Region& get_region() const { return region; }
		RegionId get_id() const { return region.get_region_data().id; }
		template <class Self> auto& area(this Self&& self) { return self.region.get_region_data().area; }
		template <class Self> auto& perimeter(this Self&& self) { return self.region.get_region_data().perimeter; }
		template <class Self> auto& fence(this Self&& self, Dir dir) { return self.fence_sides[utils::to_idx(dir)]; }
	};

	std::ostream& operator<<(std::ostream& os, Square s)
	{
		os << s.get_region();
		return os;
	}

	// This feels possible in a single-pass.
	template <AdventDay day>
	std::size_t solve_generic(std::istream& input)
	{
		std::list<Region> all_regions;
		std::vector<Square> prev_line, current_line;
		std::size_t x = 0;

		while(!input.eof())
		{
			const char c = input.get();
			if (c == '\n' || c == EOF)
			{
				AdventCheck(prev_line.size() == 0u || prev_line.size() == current_line.size());
				AdventCheck(!current_line.empty());
				Square& last_square = current_line.back();
				log << "\nProcessing ENDL\n    Bumping " << last_square << " perimeter " << last_square.perimeter() << " --> " << (last_square.perimeter() + 1);
				++last_square.perimeter();
				last_square.fence(Dir::up) = last_square.fence(Dir::left) = true;
				x = 0u;
				std::swap(prev_line, current_line);
				current_line.clear();
				continue;
			}

			log << "\nProcessing ID [" << c << "]: ";

			AdventCheck(std::isalpha(c));

			AdventCheck(prev_line.empty() || (x < prev_line.size()));
			Square* above = prev_line.empty() ? nullptr : &prev_line[x];
			Square* left = current_line.empty() ? nullptr : &current_line.back();

			const bool binds_above = above && above->get_id() == c;
			const bool binds_left = left && left->get_id() == c;

			Region* my_region = nullptr;

			if (!binds_above && !binds_left)
			{
				// No boundaries found. Start a new region.
				my_region = &(all_regions.emplace_back(c, /*area = */ 1, /*perimter = */ 2));
				log << "No links. Adding new region " << *my_region << " A=1 P=2";
				if (above)
				{
					log << "\n    Bumping " << *above << " perimiter " << above->perimeter() << " --> " << (above->perimeter() + 1);
					++(above->perimeter());
				}
				if (left)
				{
					log << "\n    Bumping " << *left << " perimiter " << left->perimeter() << " --> " << (left->perimeter() + 1);
					++(left->perimeter());
				}
				
			}

			if (binds_above != binds_left)
			{
				// Bind to one but not the other.
				Square* bound = binds_above ? above : left;
				Square* unbound = binds_above ? left : above;
				log << "Binding to " << *bound;
				log << "\n    Bumping area " << bound->area() << " --> " << (bound->area() + 1);
				log << "\n    Bumping perimiter " << bound->perimeter() << " --> " << (bound->perimeter() + 1);
				my_region = &bound->get_region();
				AdventCheck(bound->get_id() == c);
				++bound->area();
				++bound->perimeter();
				if (unbound != nullptr)
				{
					log << "\n    Bumping " << *unbound << " perimeter " << unbound->perimeter() << " --> " << (unbound->perimeter() + 1);
					++unbound->perimeter();
				}
			}

			if (binds_above && binds_left)
			{
				log << "Merging " << *left << " into " << *above;
				above->get_region().merge_with(left->get_region());
				log << "    Bumping area " << above->area() << " --> " << (above->area() + 1);
				++above->area();
				my_region = &above->get_region();
			}

			AdventCheck(my_region != nullptr);
			current_line.emplace_back(*my_region);
			++x;
		}

		// Now we're done, add the bottom perimeter.
		log << "\nProcessing final line";
		for (Square& bottom : prev_line)
		{
			log << "\n    " << bottom << " bumping perimeter " << bottom.perimeter() << " --> " << (bottom.perimeter() + 1);
			++bottom.perimeter();
		}

		for (const Region& r : all_regions)
		{
			if (r.is_root())
			{
				const RegionData& rd = r.get_region_data();
				log << "\nRegion '" << rd.id << "[0x" << &r << "] P = " << rd.perimeter << " A = " << rd.area << " Score = " << rd.evaluate();
			}
		}

		return stdr::fold_left(all_regions | stdv::transform([](const Region& r) {return r.evaluate(); }), std::size_t{ 0u }, std::plus<std::size_t>{});
	}
}

ResultType day_twelve_p1_a(std::istream& input)
{
	return solve_generic<AdventDay::one>(input);
}

ResultType day_twelve_p2_a(std::istream& input)
{
	return solve_generic<AdventDay::two>(input);
}

ResultType advent_twelve_p1()
{
	auto input = advent::open_puzzle_input(12);
	return solve_generic<AdventDay::one>(input);
}

ResultType advent_twelve_p2()
{
	auto input = advent::open_puzzle_input(12);
	return solve_generic<AdventDay::two>(input);
}

#undef DAY12DBG
#undef ENABLE_DAY12DBG
