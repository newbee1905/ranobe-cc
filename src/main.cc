#include <curl/curl.h>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <string>
#include <thread>
#include <vector>

#include "curl_utils.hh"

#include "provider/lightnovelcave.hh"
#include "provider/lightnovelworld.hh"
#include "provider_registry.hh"

int main() {
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

	auto responses_default = threaded_get(urls, share_handle);
	fmt::print("{}", fmt::join(responses_default, ",\n"));
	fmt::print("-----\n\n");

	ranobe_provider_registry registry;

	registry.add_provider("Light Novel Cave", create_lightnovelcave_provider);
	registry.add_provider("Light Novel World", create_lightnovelworld_provider);

	fmt::print("{}\n", registry.list_providers());

	auto provider_opt = registry.find_provider("Light Novel World");
	if (provider_opt) {
		ranobe_provider &provider = *provider_opt;
		auto latest_novels        = provider.get_latest_list();
		fmt::print("Latest noves from {}:\n {}\n\n", provider.name, latest_novels);
	}

	provider_opt = registry.find_provider("Light Novel Cave");
	if (provider_opt) {
		ranobe_provider &provider = *provider_opt;
		auto latest_novels        = provider.get_latest_list();
		fmt::print("Latest noves from {}:\n {}\n", provider.name, latest_novels);
	}

	curl_share_cleanup(share_handle);
	curl_global_cleanup();
	return 0;
}
