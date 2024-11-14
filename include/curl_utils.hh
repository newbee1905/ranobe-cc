#ifndef __CURL_HH__
#define __CURL_HH__

#include <curl/curl.h>
#include <fmt/format.h>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

size_t __write_callback(char *contents, size_t size, size_t nmemb, void *user_output);

int get(CURL *curl, const std::string &url, std::string &response);

template <typename A = std::allocator<std::string>>
std::vector<std::string, A> threaded_get(const std::vector<std::string> &urls, CURLSH *share_handle,
                                         const A &allocator = A()) {
	std::vector<std::string, A> output(urls.size(), allocator);
	std::vector<std::thread> threads;
	threads.reserve(urls.size());
	std::mutex output_mutex;

	for (size_t i = 0; i < urls.size(); ++i) {
		threads.emplace_back([&, i]() {
			CURL *curl = curl_easy_init();
			if (!curl) {
				fmt::print(stderr, "Failed to initialize CURL.\n");
				return 1;
			}

			curl_easy_setopt(curl, CURLOPT_SHARE, share_handle);
			std::string response;
			if (get(curl, urls[i], response)) {
				std::lock_guard<std::mutex> lock(output_mutex);
				output[i] = std::move(response);
			}
			curl_easy_cleanup(curl);

			return 0;
		});
	}

	for (auto &t : threads) {
		t.join();
	}

	return output;
}

#endif //__CURL_HH__
