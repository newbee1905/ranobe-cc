#include <curl/curl.h>
#include <fmt/format.h>
#include <fmt/ranges.h>

#include <map>
#include <string>
#include <thread>
#include <vector>

#include "arena.hh"
#include "arena_allocator.hh"
#include "curl_utils.hh"
#include "memory_utils.hh"
#include "provider/lightnovelcave.hh"
#include "provider/lightnovelworld.hh"
#include "provider_registry.hh"

arena global_arena;

int main() {
#ifdef USE_ARENA
	allocator_t<char> alloc(global_arena);
#else
	allocator_t<char> alloc;
#endif

	// ranobe::string tmp("Light Novel Cave", alloc);
	ranobe::string tmp = "Light Novel Cave";
	fmt::print("{}\n", tmp);

	fmt::print("Total allocated memory: {} bytes\n", global_arena.total_allocated());
	fmt::print("Wasted space (from resets): {} bytes\n", global_arena.wasted_space());
	fmt::print("Number of memory blocks used: {} bytes\n", global_arena.num_blocks());
	fmt::print("Stack memory used: {} bytes\n\n", global_arena.stack_memory_used());

	auto tmp2 = ranobe::format(alloc, "{} test", tmp);
	fmt::print("{}\n", tmp2);

	fmt::print("Total allocated memory: {} bytes\n", global_arena.total_allocated());
	fmt::print("Wasted space (from resets): {} bytes\n", global_arena.wasted_space());
	fmt::print("Number of memory blocks used: {} bytes\n", global_arena.num_blocks());
	fmt::print("Stack memory used: {} bytes\n\n", global_arena.stack_memory_used());

	// using __allocator_t = arena_allocator<char>;
	// __allocator_t allocator(global_arena);

	// using __string___allocator_t =
	// 		typename std::__allocator_traits<__allocator_t>::template rebind_alloc<std::string>;
	// using __string_t = std::basic_string<char, std::char_traits<char>, __string___allocator_t>;

	// using __vector___allocator_t =
	// 		typename std::__allocator_traits<__allocator_t>::template rebind_alloc<__string_t>;
	// using __vector_t = std::vector<__string_t, __vector___allocator_t>;

	// curl_global_init(CURL_GLOBAL_ALL);
	// CURLSH *share_handle = curl_share_init();
	// curl_share_setopt(share_handle, CURLSHOPT_SHARE, CURL_LOCK_DATA_CONNECT);
	// curl_share_setopt(share_handle, CURLSHOPT_SHARE, CURL_LOCK_DATA_DNS);

	// __vector_t urls(allocator);
	// urls.emplace_back("https://dummyjson.com/users/1", allocator);
	// urls.emplace_back("https://dummyjson.com/users/2", allocator);
	// urls.emplace_back("https://dummyjson.com/users/3", allocator);

	// fmt::print("{}\n\n", fmt::join(urls, ",\n"));

	// // auto responses_arena =
	// // 		threaded_get(urls, share_handle, arena_allocator<std::string>(global_arena));
	// // fmt::print("{}", fmt::join(responses_arena, ",\n"));
	// // fmt::print("-----\n\n");

	// fmt::print("Total allocated memory: {} bytes\n", global_arena.total_allocated());
	// fmt::print("Wasted space (from resets): {} bytes\n", global_arena.wasted_space());
	// fmt::print("Number of memory blocks used: {} bytes\n", global_arena.num_blocks());
	// fmt::print("Stack memory used: {} bytes\n\n", global_arena.stack_memory_used());

	// ranobe_provider_registry<__allocator_t> registry(allocator);

	// registry.add_provider(__string_t("Light Novel Cave", allocator), [&allocator]() {
	// 	return create_lightnovelcave_provider<__allocator_t>(allocator);
	// });
	// registry.add_provider(__string_t("Light Novel World", allocator), [&allocator]() {
	// 	return create_lightnovelworld_provider<__allocator_t>(allocator);
	// });

	// fmt::print("Total allocated memory: {} bytes\n", global_arena.total_allocated());
	// fmt::print("Wasted space (from resets): {} bytes\n", global_arena.wasted_space());
	// fmt::print("Number of memory blocks used: {} bytes\n", global_arena.num_blocks());
	// fmt::print("Stack memory used: {} bytes\n\n", global_arena.stack_memory_used());

	// auto providers = registry.list_providers();
	// fmt::print("Available Providers: {}\n", providers);

	// auto provider_opt = registry.find_provider(__string_t("Light Novel World", allocator));
	// if (provider_opt) {
	// 	ranobe_provider<__allocator_t> &provider = *provider_opt;
	// 	auto latest_novels                     = provider.get_latest_list();
	// 	fmt::print("Latest noves from {}:\n {}\n\n", provider.name, latest_novels);
	// }

	// fmt::print("Total allocated memory: {} bytes\n", global_arena.total_allocated());
	// fmt::print("Wasted space (from resets): {} bytes\n", global_arena.wasted_space());
	// fmt::print("Number of memory blocks used: {} bytes\n", global_arena.num_blocks());
	// fmt::print("Stack memory used: {} bytes\n\n", global_arena.stack_memory_used());

	// provider_opt = registry.find_provider(__string_t("Light Novel Cave", allocator));
	// if (provider_opt) {
	// 	ranobe_provider<__allocator_t> &provider = *provider_opt;
	// 	auto latest_novels                     = provider.get_latest_list();
	// 	fmt::print("Latest noves from {}:\n {}\n", provider.name, latest_novels);
	// }

	// fmt::print("Total allocated memory: {} bytes\n", global_arena.total_allocated());
	// fmt::print("Wasted space (from resets): {} bytes\n", global_arena.wasted_space());
	// fmt::print("Number of memory blocks used: {} bytes\n", global_arena.num_blocks());
	// fmt::print("Stack memory used: {} bytes\n\n", global_arena.stack_memory_used());

	// global_arena.clear();
	// curl_share_cleanup(share_handle);
	// curl_global_cleanup();
	// return 0;
}
