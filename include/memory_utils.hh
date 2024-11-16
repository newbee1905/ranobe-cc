#ifndef __MEMORY_UTILS_HH__
#define __MEMORY_UTILS_HH__

#include <string>
#include <vector>

#include "arena_allocator.hh"
#include "fmt/format.h"
#include "fmt/ranges.h"

#ifdef USE_ARENA
#include "arena.hh"
extern arena global_arena;
template <typename T>
using allocator_t = arena_allocator<T>;

#else
template <typename T>
using allocator_t = std::allocator<T>;
#endif

namespace ranobe {

template <typename T>
using vector = std::vector<T, allocator_t<T>>;

using string      = std::basic_string<char, std::char_traits<char>, allocator_t<char>>;
using string_view = std::basic_string_view<char>; // no allocator needed for string_view

template <typename Key, typename Value>
using map = std::map<Key, Value, std::less<Key>, allocator_t<std::pair<const Key, Value>>>;

template <typename A = allocator_t<char>>
string vformat(A alloc, fmt::string_view format_str, fmt::format_args args) {
	fmt::basic_memory_buffer<char, fmt::inline_buffer_size, A> buf{alloc};
	fmt::vformat_to(std::back_inserter(buf), format_str, args);
	return string(buf.data(), buf.size(), alloc);
}

template <typename A = allocator_t<char>, typename... Args>
inline string format(A alloc, fmt::string_view format_str, const Args &...args) {
	return vformat(alloc, format_str, fmt::make_format_args(args...));
}

}  // namespace ranobe

#endif  // __MEMORY_UTILS_HH__
