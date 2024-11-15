#include "arena.hh"
#include <algorithm>

size_t arena::align_up(size_t n, size_t alignment) {
	return (n + alignment - 1) & ~(alignment - 1);
}

__arena_memory_block &arena::allocate_new_block(size_t min_size) {
	size_t block_size = std::max(default_block_size_, min_size);

	auto new_memory = std::make_unique<uint8_t[]>(block_size);

	heap_blocks_.push_back(std::move(new_memory));
	blocks_.emplace_back(heap_blocks_.back().get(), block_size);

	return blocks_.back();
}

arena::arena(size_t default_block_size)
		: default_block_size_(default_block_size), total_allocated_(0), wasted_space_(0) {

	blocks_.emplace_back(stack_memory_.data(), STACK_SIZE, true);
	heap_blocks_.reserve(4);
}

void *arena::allocate(size_t size) {
	size = align_up(size);

	// TODO: Optimise the finding block process
	// maybe using a tree data structure
	for (auto &block : blocks_) {
		size_t aligned_used = align_up(block.used);
		size_t remaining    = block.size - aligned_used;

		if (remaining < size) {
			continue;
		}

		void *ptr  = block.memory + aligned_used;
		block.used = aligned_used + size;
		total_allocated_ += size;

		return ptr;
	}

	__arena_memory_block &new_block = allocate_new_block(size);
	void *ptr                       = new_block.memory;

	new_block.used = size;
	total_allocated_ += size;

	return ptr;
}

void arena::reset() {
	for (auto &block : blocks_) {
		wasted_space_ += block.used;
		block.used = 0;
	}
	total_allocated_ = 0;
}

void arena::clear() {
	blocks_.clear();
	heap_blocks_.clear();

	blocks_.emplace_back(stack_memory_.data(), STACK_SIZE, true);

	total_allocated_ = 0;
	wasted_space_    = 0;
}

size_t arena::total_allocated() const {
	return total_allocated_;
}

size_t arena::wasted_space() const {
	return wasted_space_;
}

size_t arena::num_blocks() const {
	return blocks_.size();
}

size_t arena::stack_memory_used() const {
	return blocks_.empty() ? 0 : blocks_[0].used;
}

arena::~arena() {
	clear();
}
