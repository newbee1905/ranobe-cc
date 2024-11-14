#ifndef __ARENA_ALLOCATOR_HH__
#define __ARENA_ALLOCATOR_HH__

#include "arena.hh"
#include <memory>

template <typename T> class arena_allocator {
	arena *arena_;

public:
	using value_type      = T;
	using pointer         = T *;
	using const_pointer   = const T *;
	using reference       = T &;
	using const_reference = const T &;
	using size_type       = std::size_t;
	using difference_type = std::ptrdiff_t;

	template <typename U> struct rebind {
		using other = arena_allocator<U>;
	};

	explicit arena_allocator(arena &a) noexcept : arena_(&a){};

	template <typename U>
	arena_allocator(const arena_allocator<U> &other) noexcept : arena_(other.arena_){};

	pointer allocate(size_type n) { return static_cast<pointer>(arena_->allocate(n * sizeof(T))); }

	void deallocate(pointer, size_type) noexcept {
		// No-op - memory is freed when arena is destroyed
	}

	template <typename U, typename... Args> void construct(U *p, Args &&...args) {
		new (p) U(std::forward<Args>(args)...);
	}

	template <typename U> void destroy(U *p) { p->~U(); }

	template <typename>
	friend class arena_allocator;
};

#endif // ARENA_ALLOCATOR_H
