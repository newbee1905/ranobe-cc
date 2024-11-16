#ifndef __ARENA_ALLOCATOR_HH__
#define __ARENA_ALLOCATOR_HH__

#include <memory>
#include <stdexcept>

#include "arena.hh"

extern arena global_arena;

template <typename T>
class arena_allocator {
	arena *arena_;

 public:
	using value_type      = T;
	using pointer         = T *;
	using const_pointer   = const T *;
	using reference       = T &;
	using const_reference = const T &;
	using size_type       = std::size_t;
	using difference_type = std::ptrdiff_t;

	template <typename U>
	struct rebind {
		using other = arena_allocator<U>;
	};

	arena_allocator() noexcept : arena_(&global_arena) {
	}

	explicit arena_allocator(arena &a) noexcept : arena_(&a){};

	template <typename U>
	arena_allocator(const arena_allocator<U> &other) noexcept : arena_(other.arena_){};

	pointer allocate(size_type n) {
		if (!arena_) {
			throw std::runtime_error("Allocator not bound to arena");
		}

		return static_cast<pointer>(arena_->allocate(n * sizeof(T)));
	}

	void deallocate(pointer, size_type) noexcept {
		// No-op - memory is freed when arena is destroyed
	}

	template <typename U, typename... Args>
	void construct(U *p, Args &&...args) {
		new (static_cast<void *>(p)) U(std::forward<Args>(args)...);
	}

	template <typename U>
	void destroy(U *p) {
		p->~U();
	}

	template <typename>
	friend class arena_allocator;

	bool operator==(const arena_allocator &other) const noexcept {
		return arena_ == other.arena_;
	}

	bool operator!=(const arena_allocator &other) const noexcept {
		return arena_ != other.arena_;
	}
};

#endif  // __ARENA_ALLOCATOR_HH__
