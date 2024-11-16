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

std::vector<std::string> threaded_get(const std::vector<std::string> &urls, CURLSH *share_handle);

#endif //__CURL_HH__
