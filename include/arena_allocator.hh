#ifndef __ARENA_ALLOCATOR_HH__
#define __ARENA_ALLOCATOR_HH__

#include "arena.hh"
#include <memory>

template<typename T>
class arena_allocator {
	arena* arena_;

public:
	using value_type = T;
	using pointer = T*;
	using const_pointer = const T*;
	using reference = T&;
	using const_reference = const T&;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;

	template<typename U>
	struct rebind {
		using other = arena_allocator<U>;
	};

	explicit arena_allocator(arena& a) noexcept;

	template<typename U>
	arena_allocator(const arena_allocator<U>& other) noexcept;

	pointer allocate(size_type n);
	void deallocate(pointer, size_type) noexcept;

	template<typename U, typename... Args>
	void construct(U* p, Args&&... args);

	template<typename U>
	void destroy(U* p);
};

// Template implementations need to be included in the header
#include "arena_allocator.tt"

#endif // ARENA_ALLOCATOR_H
