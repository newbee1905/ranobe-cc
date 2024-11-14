#include <curl/curl.h>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <string>
#include <thread>
#include <vector>

#include "arena.hh"
#include "arena_allocator.hh"
#include "curl_utils.hh"

int main() {
	arena memory_pool;
	arena_allocator<int> allocator(memory_pool);

	curl_global_init(CURL_GLOBAL_ALL);
	CURLSH *share_handle = curl_share_init();
	curl_share_setopt(share_handle, CURLSHOPT_SHARE, CURL_LOCK_DATA_CONNECT);
	curl_share_setopt(share_handle, CURLSHOPT_SHARE, CURL_LOCK_DATA_DNS);

	std::vector<std::string> urls = {
			"https://dummyjson.com/users/1",
			"https://dummyjson.com/users/2",
			"https://dummyjson.com/users/3",
	};
	fmt::print("{}", fmt::join(urls, ",\n"));

	fmt::print("DEFAULT:\n");
	auto responses_default = threaded_get(urls, share_handle);
	fmt::print("{}", fmt::join(responses_default, ",\n"));
	fmt::print("-----\n\n");

	fmt::print("ARENA:\n");
	auto responses_arena =
			threaded_get(urls, share_handle, arena_allocator<std::string>(memory_pool));
	fmt::print("{}", fmt::join(responses_arena, ",\n"));
	fmt::print("-----\n\n");

	fmt::print("Total allocated memory: {} bytes\n", memory_pool.total_allocated());
	fmt::print("Wasted space (from resets): {} bytes\n", memory_pool.wasted_space());
	fmt::print("Number of memory blocks used: {} bytes\n", memory_pool.num_blocks());
	fmt::print("Stack memory used: {} bytes\n", memory_pool.stack_memory_used());

	curl_share_cleanup(share_handle);
	curl_global_cleanup();
	return 0;
}
