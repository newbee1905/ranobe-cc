#ifndef __ARENA_HH__
#define __ARENA_HH__

#include <array>
#include <vector>
#include <memory>
#include <stddef.h>
#include <stdint.h>

struct __arena_memory_block {
	uint8_t* memory;
	size_t size;
	size_t used;
	bool is_stack;

	__arena_memory_block(uint8_t* mem, size_t block_size, bool stack = false) 
		: memory(mem), size(block_size), used(0), is_stack(stack) {}
};

class arena {
private:
	static constexpr size_t STACK_SIZE = 4 * 1024; // 4KB stack memory
	static constexpr size_t ALIGNMENT = alignof(std::max_align_t);

	std::array<uint8_t, STACK_SIZE> stack_memory_;
	std::vector<std::unique_ptr<uint8_t[]>> heap_blocks_;
	std::vector<__arena_memory_block> blocks_;

	size_t default_block_size_;
	size_t total_allocated_;
	size_t wasted_space_;

	static size_t align_up(size_t n, size_t alignment = ALIGNMENT);
	__arena_memory_block& allocate_new_block(size_t min_size);

public:
	explicit arena(size_t default_block_size = 4096);

	// Disabling copying and assignment
	arena(const arena&) = delete;
	arena& operator=(const arena&) = delete;

	void* allocate(size_t size);
	template<typename T, typename... Args>
	T* construct(Args&&... args) {
		void* ptr = allocate(sizeof(T));
		return new(ptr) T(std::forward<Args>(args)...);
	}

	void reset();
	void clear();

	size_t total_allocated() const;
	size_t wasted_space() const;
	size_t num_blocks() const;
	size_t stack_memory_used() const;

	~arena();
};

#endif // __ARENA__HH__
