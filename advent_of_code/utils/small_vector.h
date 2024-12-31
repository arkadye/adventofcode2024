#pragma once

#include <cstddef>
#include <memory>
#include <iterator>
#include <stdexcept>
#include <compare>
#include <algorithm>
#include <cstring>

#include "advent/advent_assert.h"

#define WITH_SMALL_VECTOR_DEBUG_INFO 0

#ifdef FORCE_SMALL_VECTOR_DEBUG_INFO
#define SMALL_VECTOR_DEBUG_INFO_ENABLED 1
#elif WITH_SMALL_VECTOR_DEBUG_INFO
#ifdef NDEBUG
#define SMALL_VECTOR_DEBUG_INFO_ENABLED 0
#else
#define SMALL_VECTOR_DEBUG_INFO_ENABLED WITH_SMALL_VECTOR_DEBUG_INFO
#endif
#endif

#if SMALL_VECTOR_DEBUG_INFO_ENABLED
#define CONSTEXPR
#else
#define CONSTEXPR constexpr
#endif

#if SMALL_VECTOR_DEBUG_INFO_ENABLED
#include <iostream>
#include <format>
#include <string>
#endif

namespace utils
{
	template <typename T, std::size_t STACK_SIZE, typename ALLOC = std::allocator<T>>
	class small_vector
	{
	public:
		// Typedefs
		using value_type = T;
		using allocator_type = ALLOC;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;
		using reference = value_type&;
		using const_reference = const value_type&;
		using pointer = typename std::allocator_traits<ALLOC>::pointer;
		using const_pointer = typename std::allocator_traits<ALLOC>::const_pointer;
		using iterator = T*;
		using const_iterator = const T*;
		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;
		using iterator_type = std::contiguous_iterator_tag;

		// Constructors
		CONSTEXPR small_vector() noexcept(noexcept(allocator_type())) : small_vector(allocator_type()) {}
		CONSTEXPR explicit small_vector(const allocator_type& alloc) noexcept;
		CONSTEXPR small_vector(size_type count, const T& init, const allocator_type& alloc = allocator_type());
		CONSTEXPR explicit small_vector(size_type count, const allocator_type& alloc = allocator_type());
		template <std::input_iterator InputIt>
		CONSTEXPR small_vector(InputIt first, InputIt last, const allocator_type& alloc = allocator_type());
		CONSTEXPR small_vector(const small_vector& other);
		CONSTEXPR small_vector(const small_vector& other, const allocator_type& alloc);
		CONSTEXPR small_vector(small_vector&& other) noexcept(std::is_nothrow_move_constructible_v<T>);
		CONSTEXPR small_vector(small_vector&& other, const allocator_type& alloc);
		CONSTEXPR small_vector(std::initializer_list<T> init, const allocator_type& alloc = allocator_type());
		~small_vector();

		// Assignment
		CONSTEXPR small_vector& operator=(const small_vector& other);
		CONSTEXPR small_vector& operator=(small_vector&& other) noexcept(std::is_nothrow_move_assignable_v<T> && std::is_nothrow_move_constructible_v<T>);
		CONSTEXPR small_vector& operator=(std::initializer_list<T> init);

		CONSTEXPR void assign(size_type count, const T& value);
		template <std::input_iterator InputIt >
		CONSTEXPR void assign(InputIt first, InputIt last);
		CONSTEXPR void assign(std::initializer_list<T> init);

		// Allocator
		CONSTEXPR allocator_type get_allocator() const noexcept { return allocator_type(); }

		// Element access
		CONSTEXPR reference at(size_type pos);
		CONSTEXPR const_reference at(size_type pos) const;
		CONSTEXPR reference operator[](size_type pos);
		CONSTEXPR const_reference operator[](size_type pos) const;
		CONSTEXPR reference front();
		CONSTEXPR const_reference front() const;
		CONSTEXPR reference back();
		CONSTEXPR const_reference back() const;
		CONSTEXPR T* data() noexcept;
		CONSTEXPR const T* data() const noexcept;

		// Iterators
		[[nodiscard]] iterator begin() noexcept { return data(); }
		[[nodiscard]] iterator end() noexcept { return data() + size(); }
		[[nodiscard]] const_iterator begin() const noexcept { return cbegin(); }
		[[nodiscard]] const_iterator end() const noexcept { return cend(); }
		[[nodiscard]] const_iterator cbegin() const noexcept { return data(); }
		[[nodiscard]] const_iterator cend() const noexcept { return data() + size(); }
		[[nodiscard]] reverse_iterator rbegin() noexcept { return std::make_reverse_iterator(end()); }
		[[nodiscard]] reverse_iterator rend() noexcept { return std::make_reverse_iterator(begin()); }
		[[nodiscard]] const_reverse_iterator rbegin() const noexcept { return crbegin(); }
		[[nodiscard]] const_reverse_iterator rend() const noexcept { return crend(); }
		[[nodiscard]] const_reverse_iterator crbegin() const noexcept { return std::make_reverse_iterator(cend()); }
		[[nodiscard]] const_reverse_iterator crend() const noexcept { return std::make_reverse_iterator(cbegin()); }

		// Capacity
		[[nodiscard]] CONSTEXPR bool empty() const noexcept { return size() == 0; }
		[[nodiscard]] CONSTEXPR size_type size() const noexcept { return m_num_elements; }
		[[nodiscard]] CONSTEXPR size_type max_size() const noexcept;
		CONSTEXPR void reserve(size_type new_cap);
		[[nodiscard]] CONSTEXPR size_type capacity() const noexcept { return m_capacity; }
		CONSTEXPR void shrink_to_fit();

		// Modifiers
		CONSTEXPR void clear() noexcept;
		CONSTEXPR iterator insert(const_iterator pos, const T& value) { return insert(pos, 1, value); }
		CONSTEXPR iterator insert(const_iterator pos, T&& value);
		CONSTEXPR iterator insert(const_iterator pos, size_type count, const T& value);
		template <std::input_iterator InputIt>
		CONSTEXPR iterator insert(const_iterator pos, InputIt first, InputIt last);
		CONSTEXPR iterator insert(const_iterator pos, std::initializer_list<T> init);
		template <typename ...Args>
		CONSTEXPR iterator emplace(const_iterator pos, Args&& ... args);
		CONSTEXPR iterator erase(const_iterator pos) { return erase(pos, pos + 1); }
		CONSTEXPR iterator erase(const_iterator first, const_iterator last);
		CONSTEXPR void push_back(const T& value) { emplace_back(value); }
		CONSTEXPR void push_back(T&& value) { emplace_back(std::forward<T>(value)); }
		template <typename ...Args>
		CONSTEXPR reference emplace_back(Args&&...args);
		CONSTEXPR void pop_back();
		CONSTEXPR void resize(size_type count) { resize(count, T()); }
		CONSTEXPR void resize(size_type count, const T& value);
		CONSTEXPR void swap(small_vector& other) noexcept;

	private:
#if SMALL_VECTOR_DEBUG_INFO_ENABLED
		static constexpr int DEBUG_SAFETY_BUFFER = 1;
		static constexpr int DEAD_MEM = 0xAD;
#else
		static constexpr int DEBUG_SAFETY_BUFFER = 0;
#endif
		constexpr static std::size_t stack_buffer_size() noexcept
		{
			CONSTEXPR std::size_t min_storage = sizeof(T*) / sizeof(T);
			return min_storage > STACK_SIZE ? min_storage : STACK_SIZE;
		}
		CONSTEXPR void grow(std::size_t min_capacity)
		{
			if (min_capacity <= capacity())
			{
				return;
			}
			std::size_t target_capacity = capacity();
			while (target_capacity < min_capacity)
			{
				target_capacity += 1 + target_capacity / 2;
			}
			reserve(target_capacity);
		}

		struct alignas(T) stack_buffer_t
		{
			std::byte memory[sizeof(T) * (stack_buffer_size() + DEBUG_SAFETY_BUFFER)];
		};

		union data_access
		{
			stack_buffer_t stack_buffer;
			T* heap_data;
		};

		CONSTEXPR T* get_stack_buffer() noexcept
		{
			return reinterpret_cast<T*>(m_data.stack_buffer.memory);
		}

		CONSTEXPR const T* get_stack_buffer() const noexcept
		{
			return reinterpret_cast<const T*>(m_data.stack_buffer.memory);
		}

		data_access m_data;
		std::size_t m_num_elements;
		std::size_t m_capacity;

		CONSTEXPR bool using_heap() const noexcept
		{
			return m_capacity > stack_buffer_size();
		}

		CONSTEXPR bool using_stack() const
		{
			return !using_heap();
		}

		constexpr static bool can_use_move_internally() noexcept
		{
			return !std::is_trivially_copyable_v<T> &&
				(std::is_nothrow_move_assignable_v<T> || !std::is_nothrow_copy_assignable_v<T>);
		}

		template <typename U>
		struct BasicBuffer
		{
			U* start;
			U* finish;
			BasicBuffer(U* init_start, U* init_finish) : start{ init_start }, finish{ init_finish }{}
			BasicBuffer() : BasicBuffer{ nullptr,nullptr } {}
			CONSTEXPR std::size_t size() const noexcept { return std::distance(start, finish); }
			CONSTEXPR bool empty() const noexcept { return start == finish; }
			CONSTEXPR std::size_t bytes() const noexcept { return std::distance(static_cast<char*>(start), static_cast<char*>(finish)); }
			CONSTEXPR char* raw_data() const noexcept { return static_cast<char*>(start); }
			U* begin() { return start; }
			U* end() { return finish; }
		};

		using Buffer = BasicBuffer<T>;

		struct InitialisedBuffer : Buffer
		{
			using Buffer::Buffer;
			InitialisedBuffer(Buffer init) : Buffer(init) {}
		};
		struct RawMemory : Buffer
		{
			using Buffer::Buffer;
			RawMemory(Buffer init) : Buffer(init) {}
		};

		CONSTEXPR InitialisedBuffer get_initialised_memory() noexcept
		{
			return InitialisedBuffer{ begin(),end() };
		}

		CONSTEXPR RawMemory get_unitialised_memory() noexcept
		{
			return RawMemory{ end(),begin() + capacity() };
		}

		template <typename Op>
		CONSTEXPR static void buffer_pair_operation(Buffer a_buf, Buffer b_buf, const Op& operation)
		{
			AdventCheck(a_buf.size() <= b_buf.size());
			for (T* a = a_buf.start, *b = b_buf.start; a != a_buf.finish; ++a, ++b)
			{
				operation(a, b);
			}
		}

		template <typename Op>
		CONSTEXPR static void buffer_copy_op(InitialisedBuffer from, Buffer to, const Op& copy_op)
		{
			AdventCheck(from.size() <= to.size());
			if constexpr (std::is_trivially_copyable_v<T>)
			{
				std::memmove(to.start, from.start, sizeof(T) * std::distance(from.start, from.finish));
			}
			else
			{
				buffer_pair_operation(from, to, copy_op);
			}
		}

		CONSTEXPR static void copy_buffer_to_raw_memory(InitialisedBuffer from, RawMemory to)
		{
			buffer_copy_op(from, to, [](T* f, T* t)
				{
					check_memory_dead(t);
					new(t) T(*f); 
				});
		}

		CONSTEXPR static void copy_buffer_to_initialised_memory(InitialisedBuffer from, InitialisedBuffer to)
		{
			buffer_copy_op(from, to, [](T* f, T* t) { *t = *f; });
		}

		CONSTEXPR static void delete_data_in_buffer(InitialisedBuffer buf)
		{
			if constexpr (!std::is_trivially_destructible_v<T>)
			{
				for (T* it = buf.start; it != buf.finish; ++it)
				{
					it->~T();
				}
			}
			debug_mark_memory_dead(buf.begin(), buf.end());
		}

		CONSTEXPR static void move_buffer_to_raw_memory(InitialisedBuffer from, RawMemory to)
		{
			AdventCheck(from.size() <= to.size());
			if constexpr (!can_use_move_internally())
			{
				copy_buffer_to_raw_memory(from,to);
			}
			else
			{
				buffer_pair_operation(from, to, [](T* from_it, T* to_it)
					{
						check_memory_dead(to_it);
						new(to_it) T(std::move(*from_it));
					});
			}
		}

		CONSTEXPR static void move_buffer_to_initialised_memory(InitialisedBuffer from, InitialisedBuffer to)
		{
			AdventCheck(from.size() <= to.size());
			if constexpr (!can_use_move_internally())
			{
				copy_buffer_to_initialised_memory(from,to);
			}
			else
			{
				buffer_pair_operation(from, to, [](T* from_it, T* to_it) {*to_it = std::move(*from_it); });
			}
		}

		struct GapDescription
		{
			InitialisedBuffer initialised_memory;
			RawMemory uninitialised_memory;
			CONSTEXPR std::size_t size() const noexcept { return initialised_memory.size() + uninitialised_memory.size(); }
			CONSTEXPR Buffer get_unified_buffer() const noexcept
			{
				if (uninitialised_memory.empty())
				{
					return initialised_memory;
				}
				if (initialised_memory.empty())
				{
					return uninitialised_memory;
				}
				AdventCheck(initialised_memory.finish == uninitialised_memory.start);
				return Buffer{ initialised_memory.start,uninitialised_memory.finish };
			}
		};

		CONSTEXPR static void move_buffer_to_memory(InitialisedBuffer from, const GapDescription to)
		{
			if (from.empty())
			{
				return;
			}
			if constexpr (std::is_trivially_copyable_v<T>)
			{
				const std::size_t num_bytes = from.size() * sizeof(T);
				const Buffer target_buffer = to.get_unified_buffer();
				std::memmove(target_buffer.start, from.start, num_bytes);
			}
			else
			{
				if (!to.initialised_memory.empty())
				{
					if (from.size() <= to.initialised_memory.size())
					{
						move_buffer_to_initialised_memory(from, to.initialised_memory);
						return;
					}
					else
					{
						T* split_point = from.start + to.initialised_memory.size();
						const Buffer from_initialised{ from.start,split_point };
						move_buffer_to_initialised_memory(from_initialised, to.initialised_memory);
						from.start = split_point;
					}
				}

				AdventCheck(from.size() <= to.uninitialised_memory.size());
				if (!to.uninitialised_memory.empty())
				{
					move_buffer_to_raw_memory(from, to.uninitialised_memory);
				}
			}
		}

		CONSTEXPR static void copy_buffer_to_memory(InitialisedBuffer from, const GapDescription to)
		{
			if (from.empty())
			{
				return;
			}
			if constexpr (std::is_trivially_copyable_v<T>)
			{
				const std::size_t num_bytes = from.size() * sizeof(T);
				const Buffer target_buffer = to.get_unified_buffer();
				std::memmove(target_buffer.start, from.start, num_bytes);
			}
			else
			{
				if (!to.initialised_memory.empty())
				{
					if (from.size() <= to.initialised_memory.size())
					{
						copy_buffer_to_initialised_memory(from, to.initialised_memory);
						return;
					}
					else
					{
						T* split_point = from.start + to.initialised_memory.size();
						const InitialisedBuffer from_initialised{ from.start,split_point };
						copy_buffer_to_initialised_memory(from_initialised, to.initialised_memory);
						from.start = split_point;
					}
				}

				AdventCheck(from.size() <= to.uninitialised_memory.size());
				if (!to.uninitialised_memory.empty())
				{
					copy_buffer_to_raw_memory(from, to.uninitialised_memory);
				}
			}
		}

		void check_iterator(const_iterator it) noexcept
		{
			AdventCheck(cbegin() <= it);
			AdventCheck(it <= cend());
		}

		CONSTEXPR GapDescription make_gap_for_insert(const_iterator pos, size_type gap_size)
		{
			AdventCheck(size() > 0);
			check_iterator(pos);
			const size_type distance_from_start = std::distance(cbegin(), pos);
			const size_type distance_from_end = std::distance(pos, cend());
			grow(size() + gap_size);
			pos = cbegin() + distance_from_start;
			check_iterator(pos);
			if (distance_from_end == 0)
			{
				return GapDescription{ InitialisedBuffer{},RawMemory{end(),end() + gap_size} };
			}

			T* const source = const_cast<T*>(pos);
			T* const target = const_cast<T*>(pos) + gap_size;

			if constexpr (std::is_trivially_copyable_v<T>)
			{
				const std::size_t bytes = distance_from_end * sizeof(T);
				std::memmove(target, source, bytes);
				return GapDescription{ InitialisedBuffer{},RawMemory{source,target} };
			}
			else
			{
				for (T* one_past_elem_to_move = data() + size(); one_past_elem_to_move != pos; --one_past_elem_to_move)
				{
					AdventCheck(one_past_elem_to_move > pos);
					T* from_loc = one_past_elem_to_move - 1;
					T* to_loc = from_loc + gap_size;
					const std::size_t target_idx = std::distance(data(), to_loc);
					AdventCheck(target_idx < capacity());
					const bool target_initialized = (target_idx >= size());
					const InitialisedBuffer from_buf{ from_loc,from_loc + 1 };
					if (target_idx < size())
					{
						const InitialisedBuffer to_buf{ to_loc,to_loc + 1 };
						move_buffer_to_initialised_memory(from_buf, to_buf);
					}
					else
					{
						const RawMemory to_buf{ to_loc,to_loc + 1 };
						move_buffer_to_raw_memory(from_buf, to_buf);
					}
				}
				return distance_from_end >= gap_size ?
					GapDescription{ InitialisedBuffer{source,target},RawMemory{} } :
					GapDescription{ InitialisedBuffer{source,end()},RawMemory{end(),target} };
			}
		}

		static constexpr bool can_fill_with_memset() noexcept
		{
			return std::is_trivially_constructible_v<T> && (sizeof(T) == 1);
		}

		CONSTEXPR void memset_buffer(Buffer memory, const T& value)
		{
			static_assert(can_fill_with_memset());
			if constexpr (std::is_integral_v<T>)
			{
				std::memset(memory.start,static_cast<int>(value),memory.size());
			}
			else
			{
				std::memset(memory.start, reinterpret_cast<int>(value), memory.size());
			}
		}

		template <typename Op>
		CONSTEXPR void fill_memory_with_op(Buffer memory, const T& value, const Op& op)
		{
			if constexpr (can_fill_with_memset())
			{
				memset_buffer(memory, value);
			}
			for (T* it = memory.start; it != memory.finish; ++it)
			{
				op(it, value);
			}
		}

		CONSTEXPR void fill_initialised_memory(InitialisedBuffer memory, const T& value)
		{
			fill_memory_with_op(memory, value, [](T* loc, const T& val) {*loc = val; });
		}

		CONSTEXPR void fill_raw_memory(RawMemory memory, const T& value)
		{
			fill_memory_with_op(memory, value, [&value](T* loc, const T& val)
				{
					check_memory_dead(loc);
					loc = new (loc) T(value); 
				});
		}

		CONSTEXPR void fill_memory(GapDescription memory, const T& value)
		{
			if constexpr (can_fill_with_memset())
			{
				memset_buffer(memory.get_unified_buffer(), value);
			}
			fill_initialised_memory(memory.initialised_memory, value);
			fill_raw_memory(memory.uninitialised_memory, value);
		}

		CONSTEXPR T* allocate_memory(std::size_t num_items)
		{
			log(std::format("Allocating {} bytes for {} items...", sizeof(T) * num_items, num_items));
			num_items += DEBUG_SAFETY_BUFFER;
			T* result = get_allocator().allocate(num_items);
			log(std::format("    Got address {}", static_cast<void*>(result)));
			debug_mark_memory_dead(result, result + num_items);
			return result;
		}

		CONSTEXPR void deallocate_memory(T* location, std::size_t num_items)
		{
			log(std::format("Deallocating {} bytes for {} items at address {}", sizeof(T) * num_items, num_items, static_cast<void*>(location)));
			num_items += DEBUG_SAFETY_BUFFER;
			check_memory_dead(location, location + num_items);
			get_allocator().deallocate(location, num_items);
		}

		void log(const std::string& msg)
		{
#if SMALL_VECTOR_DEBUG_INFO_ENABLED
			const std::string_view type_name = typeid(T).name();
			const auto name_split = type_name.rfind("::");
			const auto short_name = type_name.substr(name_split + 2);
			const auto stack_size = stack_buffer_size();
			const auto my_loc = static_cast<void*>(this);
			const auto heap_loc = [this]()
				{
					return using_heap() ? std::format("{}", static_cast<void*>(m_data.heap_data)) : "S";
				}();
			std::cout << std::format("\nutils::small_vector<{},{}>[{}][{}]: {}", short_name, stack_size, my_loc, heap_loc, msg);
#endif
		}

		static void check_memory_dead(const void* first, const void* last)
		{
#if SMALL_VECTOR_DEBUG_INFO_ENABLED
			const auto f = static_cast<const char*>(first);
			const auto l = static_cast<const char*>(last);
			std::span mem_view(f, l);
			for (auto it = f; it != l; ++it)
			{
				const char val = *it;
				const char ref = static_cast<char>(DEAD_MEM);
				if (val != ref)
				{
					std::cout << std::format("\nByte {} is not 0xAD as expected (Value={:#x})", static_cast<const void*>(it), val);
				}
				AdventCheckMsg(val == ref, "Memory checked against reference value of '0xAD' failed");
			}
#endif
		}

		static void check_memory_dead(const T* item)
		{
			check_memory_dead(item, item + 1);
		}

		void validate_memory() const
		{
			check_memory_dead(data() + size(), data() + capacity() + DEBUG_SAFETY_BUFFER);
		}

		static void debug_mark_memory_dead(void* first, void* last)
		{
#if SMALL_VECTOR_DEBUG_INFO_ENABLED
			const std::size_t len = std::distance(static_cast<std::byte*>(first), static_cast<std::byte*>(last));
			std::memset(first, DEAD_MEM, len);
#endif
		}

		static void debug_mark_memory_dead(T* item)
		{
			debug_mark_memory_dead(item, item + 1);
		}
	};
}

template <typename T, std::size_t STACK_SIZE, typename ALLOC>
inline CONSTEXPR utils::small_vector<T, STACK_SIZE, ALLOC>::small_vector(const allocator_type& alloc) noexcept
	: m_num_elements{ 0 }, m_capacity{ stack_buffer_size() }
{
	debug_mark_memory_dead(m_data.stack_buffer.memory, m_data.stack_buffer.memory + sizeof(T) * (m_capacity + DEBUG_SAFETY_BUFFER));
}

template <typename T, std::size_t STACK_SIZE, typename ALLOC>
inline CONSTEXPR utils::small_vector<T, STACK_SIZE, ALLOC>::small_vector(size_type count, const allocator_type& alloc)
	: small_vector(count, T(), alloc) {}

template<typename T, std::size_t STACK_SIZE, typename ALLOC>
inline CONSTEXPR typename utils::small_vector<T, STACK_SIZE, ALLOC>::size_type utils::small_vector<T, STACK_SIZE, ALLOC>::max_size() const noexcept
{
	return std::allocator_traits<ALLOC>::max_size(ALLOC());
}

template<typename T, std::size_t STACK_SIZE, typename ALLOC>
inline CONSTEXPR utils::small_vector<T, STACK_SIZE, ALLOC>::small_vector(size_type count, const T& init, const allocator_type& alloc)
	: small_vector(alloc)
{
	assign(count, init);
}

template<typename T, std::size_t STACK_SIZE, typename ALLOC>
template<std::input_iterator InputIt>
inline CONSTEXPR utils::small_vector<T, STACK_SIZE, ALLOC>::small_vector(InputIt first, InputIt last, const allocator_type& alloc)
	: small_vector(alloc)
{
	assign(first, last);
}

template<typename T, std::size_t STACK_SIZE, typename ALLOC>
inline CONSTEXPR utils::small_vector<T, STACK_SIZE, ALLOC>::small_vector(const small_vector<T, STACK_SIZE, ALLOC>& other)
	: small_vector(other,allocator_type())
{
}

template<typename T, std::size_t STACK_SIZE, typename ALLOC>
inline CONSTEXPR utils::small_vector<T, STACK_SIZE, ALLOC>::small_vector(const small_vector<T, STACK_SIZE, ALLOC>& other, const allocator_type& alloc)
	: small_vector(other.begin(),other.end(),alloc)
{
}

template<typename T, std::size_t STACK_SIZE, typename ALLOC>
inline CONSTEXPR utils::small_vector<T, STACK_SIZE, ALLOC>::small_vector(small_vector<T, STACK_SIZE, ALLOC>&& other) noexcept(std::is_nothrow_move_constructible_v<T>)
	: small_vector(std::forward<small_vector<T,STACK_SIZE,ALLOC>>(other),allocator_type())
{
}

template<typename T, std::size_t STACK_SIZE, typename ALLOC>
inline CONSTEXPR utils::small_vector<T, STACK_SIZE, ALLOC>::small_vector(std::initializer_list<T> init, const allocator_type& alloc)
	: small_vector(alloc)
{
	assign(init);
}

template<typename T, std::size_t STACK_SIZE, typename ALLOC>
inline CONSTEXPR utils::small_vector<T, STACK_SIZE, ALLOC>::small_vector(small_vector<T,STACK_SIZE,ALLOC>&& other, const allocator_type& alloc)
	: small_vector(alloc)
{
	*this = std::forward<small_vector<T, STACK_SIZE, ALLOC>>(other);
}

template<typename T, std::size_t STACK_SIZE, typename ALLOC>
inline CONSTEXPR utils::small_vector<T, STACK_SIZE, ALLOC>& utils::small_vector<T, STACK_SIZE, ALLOC>::operator=(const small_vector<T, STACK_SIZE, ALLOC>& other)
{
	if (&other != this)
	{
		assign(other.begin(), other.end());
	}
	return *this;
}

template<typename T, std::size_t STACK_SIZE, typename ALLOC>
inline CONSTEXPR utils::small_vector<T, STACK_SIZE, ALLOC>& utils::small_vector<T, STACK_SIZE, ALLOC>::operator=(std::initializer_list<T> init)
{
	assign(init);
	return *this;
}

template<typename T, std::size_t STACK_SIZE, typename ALLOC>
inline CONSTEXPR void utils::small_vector<T, STACK_SIZE, ALLOC>::assign(std::initializer_list<T> init)
{
	auto move_it = [](auto it)
	{
		return std::make_move_iterator(it);
	};
	assign(move_it(init.begin()), move_it(init.end()));
}

template<typename T, std::size_t STACK_SIZE, typename ALLOC>
inline utils::small_vector<T, STACK_SIZE, ALLOC>::~small_vector()
{
	log(std::format("Destroying with {} elements", size()));
	clear();
	shrink_to_fit();
}

template<typename T, std::size_t STACK_SIZE, typename ALLOC>
inline CONSTEXPR typename utils::small_vector<T, STACK_SIZE, ALLOC>::reference utils::small_vector<T, STACK_SIZE, ALLOC>::operator[](size_type pos)
{
	AdventCheck(pos < size());
	return data()[pos];
}

template<typename T, std::size_t STACK_SIZE, typename ALLOC>
inline CONSTEXPR typename utils::small_vector<T, STACK_SIZE, ALLOC>::const_reference utils::small_vector<T, STACK_SIZE, ALLOC>::operator[](size_type pos) const
{
	AdventCheck(pos < size());
	return data()[pos];
}

template<typename T, std::size_t STACK_SIZE, typename ALLOC>
inline CONSTEXPR typename utils::small_vector<T, STACK_SIZE, ALLOC>::reference utils::small_vector<T, STACK_SIZE, ALLOC>::at(size_type pos)
{
	if (pos >= size())
	{
		throw std::out_of_range("Out of range error: pos >= small_vector::size()");
	}
	return operator[](pos);
}

template<typename T, std::size_t STACK_SIZE, typename ALLOC>
inline CONSTEXPR typename utils::small_vector<T, STACK_SIZE, ALLOC>::const_reference utils::small_vector<T, STACK_SIZE, ALLOC>::at(size_type pos) const
{
	if (pos >= size())
	{
		throw std::out_of_range("Out of range error: pos >= small_vector::size()");
	}
	return operator[](pos);
}

template<typename T, std::size_t STACK_SIZE, typename ALLOC>
inline CONSTEXPR T* utils::small_vector<T, STACK_SIZE, ALLOC>::data() noexcept
{
	return using_heap() ? m_data.heap_data : get_stack_buffer();
}

template<typename T, std::size_t STACK_SIZE, typename ALLOC>
inline CONSTEXPR const T* utils::small_vector<T, STACK_SIZE, ALLOC>::data() const noexcept
{
	return using_heap() ? m_data.heap_data : get_stack_buffer();
}

template<typename T, std::size_t STACK_SIZE, typename ALLOC>
inline CONSTEXPR void utils::small_vector<T, STACK_SIZE, ALLOC>::reserve(size_type new_cap)
{
	if (new_cap <= capacity())
	{
		return;
	}

	T* new_data = allocate_memory(new_cap);
	const InitialisedBuffer old_buffer = get_initialised_memory();
	const RawMemory new_buffer{ new_data,new_data + new_cap };
	move_buffer_to_raw_memory(old_buffer,new_buffer);
	delete_data_in_buffer(old_buffer);

	if (using_heap())
	{
		deallocate_memory(old_buffer.start, capacity());
	}

	m_data.heap_data = new_data;
	m_capacity = new_cap;
	validate_memory();
}

template<typename T, std::size_t STACK_SIZE, typename ALLOC>
inline CONSTEXPR void utils::small_vector<T, STACK_SIZE, ALLOC>::shrink_to_fit()
{
	if (size() == capacity()) // Nothing to do.
	{
		return;
	}

	if (using_stack()) // Can't shrink.
	{
		return;
	}

	const InitialisedBuffer old_buffer = get_initialised_memory();
	const RawMemory new_buffer = [this]()
	{
		if (size() <= stack_buffer_size())
		{
			debug_mark_memory_dead(get_stack_buffer(), get_stack_buffer() + stack_buffer_size() + DEBUG_SAFETY_BUFFER);
			return RawMemory{ get_stack_buffer(),get_stack_buffer() + size() };
		}
		T* new_start = allocate_memory(size());
		return RawMemory{ new_start,new_start + size() };
	}();
	move_buffer_to_raw_memory(old_buffer, new_buffer);
	delete_data_in_buffer(old_buffer);
	deallocate_memory(old_buffer.start,capacity());
	m_capacity = std::max(stack_buffer_size(), size());
}

template<typename T, std::size_t STACK_SIZE, typename ALLOC>
inline CONSTEXPR utils::small_vector<T, STACK_SIZE, ALLOC>& utils::small_vector<T, STACK_SIZE, ALLOC>::operator=(small_vector&& other) noexcept(std::is_nothrow_move_assignable_v<T>&& std::is_nothrow_move_constructible_v<T>)
{
	if (this == &other)
	{
		return *this;
	}
	if (other.using_heap())
	{
		clear();
		shrink_to_fit();
		m_data.heap_data = other.m_data.heap_data;
		m_capacity = other.capacity();
		m_num_elements = other.size();
		other.m_capacity = other.stack_buffer_size();
		other.m_num_elements = 0;
		return *this;
	}
	
	AdventCheck(capacity() >= other.size());
	if constexpr (std::is_trivially_copy_assignable_v<T>)
	{
		std::memcpy(begin(), other.begin(), sizeof(T) * other.size());
		m_num_elements = other.size();
		return *this;
	}

	if (size() == other.size())
	{
		move_buffer_to_initialised_memory(other.get_initialised_memory(), get_initialised_memory());
	}
	else if (size() < other.size())
	{
		move_buffer_to_initialised_memory(InitialisedBuffer{ other.begin(),other.begin() + size() }, get_initialised_memory());
		move_buffer_to_raw_memory(InitialisedBuffer{ other.begin() + size(),other.end() }, get_unitialised_memory());
	}
	else // size() > other.size()
	{
		move_buffer_to_initialised_memory(other.get_initialised_memory(), get_initialised_memory());
		delete_data_in_buffer(InitialisedBuffer{ begin() + other.size(),end() });
	}
	m_num_elements = other.size();
	return *this;
}

template<typename T, std::size_t STACK_SIZE, typename ALLOC>
inline CONSTEXPR void utils::small_vector<T, STACK_SIZE, ALLOC>::clear() noexcept
{
	delete_data_in_buffer(get_initialised_memory());
	m_num_elements = 0;
}

template<typename T, std::size_t STACK_SIZE, typename ALLOC>
inline CONSTEXPR typename utils::small_vector<T, STACK_SIZE, ALLOC>::iterator utils::small_vector<T, STACK_SIZE, ALLOC>::insert(const_iterator pos, T&& value)
{
	const GapDescription gap = make_gap_for_insert(pos, 1);
	//AdventCheck(gap.initialized_memory.size() != gap.uninitialised_memory.size());
	move_buffer_to_memory(Buffer{ &value,(&value) + 1 }, gap);
	++m_num_elements;
	return gap.initialised_memory.empty() ? gap.uninitialised_memory.finish : gap.initialised_memory.finish;
}

template<typename T, std::size_t STACK_SIZE, typename ALLOC>
inline CONSTEXPR typename utils::small_vector<T, STACK_SIZE, ALLOC>::iterator utils::small_vector<T, STACK_SIZE, ALLOC>::insert(const_iterator pos, size_type count, const T& value)
{
	const GapDescription gap = make_gap_for_insert(pos, count);
	fill_memory(gap, value);
	m_num_elements += count;
	return gap.initialised_memory.empty() ? gap.uninitialised_memory.finish : gap.initialised_memory.finish;
}

template<typename T, std::size_t STACK_SIZE, typename ALLOC>
inline CONSTEXPR typename utils::small_vector<T, STACK_SIZE, ALLOC>::iterator utils::small_vector<T, STACK_SIZE, ALLOC>::insert(const_iterator pos, std::initializer_list<T> init)
{
	auto move_it = [](auto it)
	{
		return std::make_move_iterator(it);
	};
	return insert(pos, move_it(init.begin()), move_it(init.end()));
}

template<typename T, std::size_t STACK_SIZE, typename ALLOC>
template<typename ...Args>
inline CONSTEXPR typename utils::small_vector<T, STACK_SIZE, ALLOC>::iterator utils::small_vector<T, STACK_SIZE, ALLOC>::emplace(const_iterator pos, Args && ...args)
{
	if (pos == cend())
	{
		emplace_back(std::forward<Args>(args)...);
		return end();
	}
	GapDescription gap = make_gap_for_insert(pos, 1);
	AdventCheck(gap.initialised_memory.size() == 1);
	AdventCheck(gap.uninitualised_memory.empty());
	*gap.initialised_memory.first = T(std::forward<Args>(args)...);
	++m_num_elements;
	validate_memory();
	return gap.initialised_memory.last;
}

template<typename T, std::size_t STACK_SIZE, typename ALLOC>
template<std::input_iterator InputIt>
inline CONSTEXPR typename utils::small_vector<T, STACK_SIZE, ALLOC>::iterator utils::small_vector<T, STACK_SIZE, ALLOC>::insert(const_iterator pos, InputIt first, InputIt last)
{
	using ItCategory = typename std::iterator_traits<InputIt>::iterator_category;
	if constexpr (std::is_convertible_v<ItCategory, std::input_iterator_tag>)
	{
		const auto size_increase = std::distance(first, last);
		GapDescription gap = make_gap_for_insert(pos, size_increase);
		AdventCheck(gap.size() == size_increase);
		InputIt it = first;
		for (T* init = gap.initialised_memory.start; init != gap.initialised_memory.finish; ++init)
		{
			*init = *(it++);
		}
		for (T* uninit = gap.uninitialised_memory.start; uninit != gap.uninitialised_memory.finish; ++uninit)
		{
			check_memory_dead(uninit);
			uninit = new(uninit) T(*(it++));
		}
		m_num_elements += size_increase;
		return gap.get_unified_buffer().finish;
	}
	// Can't use std::difference
	for (auto it = first; it != last; ++it)
	{
		pos = insert(pos, *it);
	}
	return const_cast<iterator>(pos);
}

template<typename T, std::size_t STACK_SIZE, typename ALLOC>
template<typename ...Args>
inline CONSTEXPR typename utils::small_vector<T, STACK_SIZE, ALLOC>::reference utils::small_vector<T, STACK_SIZE, ALLOC>::emplace_back(Args && ...args)
{
	grow(size() + 1);
	check_memory_dead(data() + size());
	new(data() + size()) T(std::forward<Args>(args)...);
	++m_num_elements;
	validate_memory();
	return back();
}

template<typename T, std::size_t STACK_SIZE, typename ALLOC>
inline CONSTEXPR typename utils::small_vector<T, STACK_SIZE, ALLOC>::iterator utils::small_vector<T, STACK_SIZE, ALLOC>::erase(const_iterator first, const_iterator last)
{
	auto to_nc_it = [this](const_iterator it) -> iterator
	{
		const auto idx = std::distance(cbegin(), it);
		return begin() + idx;
	};
	if (first == last)
	{
		return to_nc_it(last);
	}

	const auto num_removed = std::distance(first, last);
	AdventCheck(static_cast<std::size_t>(num_removed) <= size());

	const auto tail_length = static_cast<std::size_t>(std::distance(last, cend()));
	
	if (last != cend())
	{
		if constexpr (std::is_trivially_copy_assignable_v<T>)
		{
			std::memmove(to_nc_it(first), to_nc_it(last), sizeof(T) * tail_length);
		}
		else
		{
			for (auto from_it = to_nc_it(last), to_it = to_nc_it(first); from_it != cend(); ++from_it, ++to_it)
			{
				if constexpr (can_use_move_internally())
				{
					*to_it = std::move(*from_it);
				}
				else
				{
					*to_it = *from_it;
				}
			}
		}
	}

	InitialisedBuffer tail_buffer{ end() - num_removed,end() };
	delete_data_in_buffer(tail_buffer);

	m_num_elements -= num_removed;
	const auto return_idx = std::distance(cbegin(), first);
	return begin() + return_idx;
}

template<typename T, std::size_t STACK_SIZE, typename ALLOC>
inline CONSTEXPR void utils::small_vector<T, STACK_SIZE, ALLOC>::pop_back()
{
	AdventCheck(!empty());
	erase(cend() - 1);
}

template<typename T, std::size_t STACK_SIZE, typename ALLOC>
inline CONSTEXPR void utils::small_vector<T, STACK_SIZE, ALLOC>::resize(size_type count, const T& value)
{
	if (count == 0)
	{
		clear();
	}

	if (count <= size())
	{
		InitialisedBuffer to_remove{ begin() + count,end() };
		delete_data_in_buffer(to_remove);
	}
	else
	{
		grow(count);
		RawMemory to_add{ end(),begin() + count };
		fill_raw_memory(to_add, value);
	}

	m_num_elements = count;
}

template<typename T, std::size_t STACK_SIZE, typename ALLOC>
inline CONSTEXPR void utils::small_vector<T, STACK_SIZE, ALLOC>::swap(small_vector& other) noexcept
{
	reserve(other.size());
	other.reserve(size());
	if (using_heap() && other.using_heap())
	{
		log("Swapping heap pointers. (See next line.)");
		other.log("Swapping heap points. (See previous line)");
		std::swap(m_data.heap_data, other.m_data.heap_data);
		std::swap(m_capacity, other.m_capacity);
		std::swap(m_num_elements, other.m_num_elements);
		log("Finished swapping heap pointers. (See next line");
		other.log("Finished swapping heap pointers. (See previous line)");
	}
	else
	{
		log("Swapping stack data. (See next line.)");
		other.log("Swapping stack data. (See previous line)");
		small_vector<T, STACK_SIZE, ALLOC> temp = std::move(other);
		other = std::move(*this);
		*this = std::move(temp);
		log("Swapping stack data. (See next line");
		other.log("Swapping stack data. (See previous line)");
	}
	validate_memory();
}

template<typename T, std::size_t STACK_SIZE, typename ALLOC>
inline CONSTEXPR void utils::small_vector<T, STACK_SIZE, ALLOC>::assign(size_type count, const T& value)
{
	grow(count);

	T* new_end = begin() + count;
	if (count <= size())
	{
		fill_initialised_memory(InitialisedBuffer{ begin(),new_end }, value);
		delete_data_in_buffer(InitialisedBuffer{ new_end,end() });
	}
	else
	{
		GapDescription memory{ InitialisedBuffer{begin(),end()},RawMemory{end(),new_end} };
		fill_memory(memory, value);
	}

	m_num_elements = count;
}

template<typename T, std::size_t STACK_SIZE, typename ALLOC>
template<std::input_iterator InputIt>
inline CONSTEXPR void utils::small_vector<T, STACK_SIZE, ALLOC>::assign(InputIt first, InputIt last)
{
	using ItCategory = typename std::iterator_traits<InputIt>::iterator_category;

	static_assert(!std::is_same_v<ItCategory, std::output_iterator_tag>, "Cannot assign from an output iterator.");

	const auto clear_tail = [this](std::size_t num_copied)
	{
		if (num_copied < size())
		{
			InitialisedBuffer to_delete{ begin() + num_copied,end() };
			delete_data_in_buffer(to_delete);
			m_num_elements = num_copied;
		}
	};

	const auto memberwise_copy = [this, &first, &last]()
	{
		std::size_t num_copied = 0;
		while (first != last)
		{
			if (num_copied < size())
			{
				(*this)[num_copied] = *first++;
			}
			else
			{
				push_back(*first++);
			}
			++num_copied;
		}
		return num_copied;
	};

	// Only one pass is available of the input, so use it carefully.
	if constexpr (std::is_same_v<ItCategory, std::input_iterator_tag>)
	{
		const std::size_t num_copied = memberwise_copy();

		clear_tail(num_copied);
		
		AdventCheck(size() == num_copied);
		return;
	}

	// This option allows us to check the size of the range and plan accordingly.
	const auto new_size = static_cast<std::size_t>(std::distance(first, last));
	if (new_size > capacity())
	{
		// Clear everything to save copying it later, as it will only be overridden anyway.
		clear();
		reserve(new_size);
	}

	// If we have pointer-style iterators, we can use an optimised version
	if constexpr (std::is_pointer_v<InputIt> && std::is_same_v<InputIt,T>)
	{
		GapDescription target{
			get_initialised_memory(),
			get_unitialised_memory()
		};
		InitialisedBuffer input{ const_cast<T*>(first),const_cast<T*>(last) };
		copy_buffer_to_memory(input, target);
		m_num_elements = new_size;
		return;
	}
	
	// Otherwise we have to do a memberwise copy.
	memberwise_copy();
	m_num_elements = new_size;
	return;
}

template<typename T, std::size_t STACK_SIZE, typename ALLOC>
inline CONSTEXPR typename utils::small_vector<T, STACK_SIZE, ALLOC>::reference utils::small_vector<T, STACK_SIZE, ALLOC>::front()
{
	AdventCheck(!empty());
	return (*this)[0];
}

template<typename T, std::size_t STACK_SIZE, typename ALLOC>
inline CONSTEXPR typename utils::small_vector<T, STACK_SIZE, ALLOC>::const_reference utils::small_vector<T, STACK_SIZE, ALLOC>::front() const
{
	AdventCheck(!empty());
	return (*this)[0];
}

template<typename T, std::size_t STACK_SIZE, typename ALLOC>
inline CONSTEXPR typename utils::small_vector<T, STACK_SIZE, ALLOC>::reference utils::small_vector<T, STACK_SIZE, ALLOC>::back()
{
	AdventCheck(!empty());
	return (*this)[size() - 1];
}

template<typename T, std::size_t STACK_SIZE, typename ALLOC>
inline CONSTEXPR typename utils::small_vector<T, STACK_SIZE, ALLOC>::const_reference utils::small_vector<T, STACK_SIZE, ALLOC>::back() const
{
	AdventCheck(!empty());
	return (*this)[size() - 1];
}

template<typename T_L, typename T_R, std::size_t STACK_SIZE_L, std::size_t STACK_SIZE_R, typename ALLOC_L, typename ALLOC_R>
inline CONSTEXPR bool operator==(const utils::small_vector<T_L, STACK_SIZE_L, ALLOC_L>& left, const utils::small_vector<T_R, STACK_SIZE_R, ALLOC_R>& right) noexcept
{
	return stdr::equal(left, right);;
}

template<typename T_L, typename T_R, std::size_t STACK_SIZE_L, std::size_t STACK_SIZE_R, typename ALLOC_L, typename ALLOC_R>
inline CONSTEXPR auto operator<=>(const utils::small_vector<T_L, STACK_SIZE_L, ALLOC_L>& left, const utils::small_vector<T_R, STACK_SIZE_R, ALLOC_R>& right) noexcept
{
	return std::lexicographical_compare_three_way(begin(left), end(left), begin(right), end(right));
}

#undef CONSTEXPR