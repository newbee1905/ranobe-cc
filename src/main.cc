#include <curl/curl.h>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <string>
#include <thread>
#include <vector>

#include "arena.hh"
#include "arena_allocator.hh"
#include "curl_utils.hh"

#include "provider/lightnovelcave.hh"
#include "provider/lightnovelworld.hh"
#include "provider_registry.hh"

int main() {
	arena memory_pool;
	using allocator_t = arena_allocator<char>;
	allocator_t allocator(memory_pool);

	using __string_allocator_t =
			typename std::allocator_traits<allocator_t>::template rebind_alloc<std::string>;
	using __string_t = std::basic_string<char, std::char_traits<char>, __string_allocator_t>;

	using __vector_allocator_t =
			typename std::allocator_traits<allocator_t>::template rebind_alloc<__string_t>;
	using __vector_t = std::vector<__string_t, __vector_allocator_t>;

	curl_global_init(CURL_GLOBAL_ALL);
	CURLSH *share_handle = curl_share_init();
	curl_share_setopt(share_handle, CURLSHOPT_SHARE, CURL_LOCK_DATA_CONNECT);
	curl_share_setopt(share_handle, CURLSHOPT_SHARE, CURL_LOCK_DATA_DNS);

	__vector_t urls(allocator);
	urls.emplace_back("https://dummyjson.com/users/1", allocator);
	urls.emplace_back("https://dummyjson.com/users/2", allocator);
	urls.emplace_back("https://dummyjson.com/users/3", allocator);

	fmt::print("{}\n\n", fmt::join(urls, ",\n"));

	// auto responses_arena =
	// 		threaded_get(urls, share_handle, arena_allocator<std::string>(memory_pool));
	// fmt::print("{}", fmt::join(responses_arena, ",\n"));
	// fmt::print("-----\n\n");

	fmt::print("Total allocated memory: {} bytes\n", memory_pool.total_allocated());
	fmt::print("Wasted space (from resets): {} bytes\n", memory_pool.wasted_space());
	fmt::print("Number of memory blocks used: {} bytes\n", memory_pool.num_blocks());
	fmt::print("Stack memory used: {} bytes\n\n", memory_pool.stack_memory_used());

	ranobe_provider_registry<allocator_t> registry(allocator);

	registry.add_provider(__string_t("Light Novel Cave", allocator), [&allocator]() {
		return create_lightnovelcave_provider<allocator_t>(allocator);
	});
	registry.add_provider(__string_t("Light Novel World", allocator), [&allocator]() {
		return create_lightnovelworld_provider<allocator_t>(allocator);
	});

	fmt::print("Total allocated memory: {} bytes\n", memory_pool.total_allocated());
	fmt::print("Wasted space (from resets): {} bytes\n", memory_pool.wasted_space());
	fmt::print("Number of memory blocks used: {} bytes\n", memory_pool.num_blocks());
	fmt::print("Stack memory used: {} bytes\n\n", memory_pool.stack_memory_used());

	auto providers = registry.list_providers();
	fmt::print("Available Providers: {}\n", providers);

	auto provider_opt = registry.find_provider(__string_t("Light Novel World", allocator));
	if (provider_opt) {
		ranobe_provider<allocator_t> &provider = *provider_opt;
		auto latest_novels                     = provider.get_latest_list();
		fmt::print("Latest noves from {}:\n {}\n\n", provider.name, latest_novels);
	}

	fmt::print("Total allocated memory: {} bytes\n", memory_pool.total_allocated());
	fmt::print("Wasted space (from resets): {} bytes\n", memory_pool.wasted_space());
	fmt::print("Number of memory blocks used: {} bytes\n", memory_pool.num_blocks());
	fmt::print("Stack memory used: {} bytes\n\n", memory_pool.stack_memory_used());

	provider_opt = registry.find_provider(__string_t("Light Novel Cave", allocator));
	if (provider_opt) {
		ranobe_provider<allocator_t> &provider = *provider_opt;
		auto latest_novels                     = provider.get_latest_list();
		fmt::print("Latest noves from {}:\n {}\n", provider.name, latest_novels);
	}

	fmt::print("Total allocated memory: {} bytes\n", memory_pool.total_allocated());
	fmt::print("Wasted space (from resets): {} bytes\n", memory_pool.wasted_space());
	fmt::print("Number of memory blocks used: {} bytes\n", memory_pool.num_blocks());
	fmt::print("Stack memory used: {} bytes\n\n", memory_pool.stack_memory_used());

	memory_pool.clear();
	curl_share_cleanup(share_handle);
	curl_global_cleanup();
	return 0;
}
