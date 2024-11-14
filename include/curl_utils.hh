#ifndef __CURL_HH__
#define __CURL_HH__

#include <curl/curl.h>
#include <string>
#include <vector>

size_t __write_callback(char *contents, size_t size, size_t nmemb, void *user_output);

int get(CURL *curl, const std::string &url, std::string &response);

template <typename A = std::allocator<std::string>>
std::vector<std::string, A> threaded_get(const std::vector<std::string> &urls, CURLSH *share_handle,
                                         const A &allocator = A());

#include "curl_utils.tt"

#endif //__CURL_HH__
