#include <fmt/format.h>
#include <fmt/ranges.h>
#include <curl/curl.h>
#include <string>
#include <vector>
#include <thread>
#include <mutex>

static inline size_t __write_callback(char* contents, size_t size, size_t nmemb, void* user_output);

static inline int get(CURL* curl, const std::string& url, std::string& response);
static inline std::vector<std::string> threaded_get(const std::vector<std::string>& urls, CURLSH* share_handle);

int main() {
	curl_global_init(CURL_GLOBAL_ALL);
	CURLSH* share_handle = curl_share_init();
	curl_share_setopt(share_handle, CURLSHOPT_SHARE, CURL_LOCK_DATA_CONNECT);
	curl_share_setopt(share_handle, CURLSHOPT_SHARE, CURL_LOCK_DATA_DNS);

	std::vector<std::string> urls = {
		"https://dummyjson.com/users/1",
		"https://dummyjson.com/users/2",
		"https://dummyjson.com/users/3",
	};
	fmt::print("{:?}", fmt::join(urls, ",\n"));

	auto responses = threaded_get(urls, share_handle);
	fmt::print("{:?}", fmt::join(responses, ",\n"));

	curl_share_cleanup(share_handle);
	curl_global_cleanup();
	return 0;
}

static inline
size_t __write_callback(char* contents, size_t size, size_t nmemb, void* user_output) {
	auto* output = static_cast<std::string*>(user_output);
	size_t totalSize = size * nmemb;

	try {
		output->append(contents, totalSize);
	} catch (const std::bad_alloc& e) {
		fmt::print(stderr, "Memory allocation failed: {}\n", e.what());
		return 0; // Returning 0 will signal CURL to abort the transfer
	}

	return totalSize;
}

static inline
int get(CURL* curl, const std::string& url, std::string& response) {
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, __write_callback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, 102400L);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

	CURLcode res = curl_easy_perform(curl);
	if (res != CURLE_OK) {
		fmt::print(stderr, "Request to {} failed: {}\n", url, curl_easy_strerror(res));
		return 0;
	}

	fmt::print("{}\n", response);

	return 1;
}

static inline
std::vector<std::string> threaded_get(const std::vector<std::string>& urls, CURLSH* share_handle) {
	std::vector<std::string> output(urls.size());
	std::vector<std::thread> threads;
	threads.reserve(urls.size());
	std::mutex output_mutex;

	for (size_t i = 0; i < urls.size(); ++i) {
		threads.emplace_back([&, i]() {
			CURL* curl = curl_easy_init();
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

	for (auto& t : threads) {
		t.join();
	}

	return output;
}
