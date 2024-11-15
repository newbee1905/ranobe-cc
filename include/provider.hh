#ifndef __PROVIDER_HH__
#define __PROVIDER_HH__

#include <algorithm>
#include <functional>
#include <optional>
#include <string>
#include <vector>

template <typename A = std::allocator<char>>
struct provider_types {
	using __string_allocator_t =
			typename std::allocator_traits<A>::template rebind_alloc<std::string>;
	using __string_t = std::basic_string<char, std::char_traits<char>, __string_allocator_t>;

	using __vector_allocator_t = typename std::allocator_traits<A>::template rebind_alloc<__string_t>;
	using __vector_t           = std::vector<__string_t, __vector_allocator_t>;
};

template <typename A = std::allocator<char>>
struct ranobe_provider {
	using types      = provider_types<A>;
	using __string_t = typename types::__string_t;
	using __vector_t = typename types::__vector_t;

	using __list_func_t                   = std::function<__vector_t()>;
	using __search_func_t                 = std::function<__vector_t(const __string_t &)>;
	using __chapter_list_func_t           = std::function<__vector_t(const __string_t &)>;
	using __chapter_html_func_t           = std::function<__string_t(const __string_t &)>;
	using __parse_html_to_markdown_func_t = std::function<__string_t(const __string_t &)>;

	__string_t name;
	__string_t base_url;

	__list_func_t get_latest_list;
	__search_func_t search_novels;
	__chapter_list_func_t get_chapter_list;
	__chapter_html_func_t get_chapter_html;
	__parse_html_to_markdown_func_t parse_html_to_markdown;

	ranobe_provider(__string_t provider_name, __string_t base_url, __list_func_t latest,
	                __search_func_t search, __chapter_list_func_t chapters,
	                __chapter_html_func_t html, __parse_html_to_markdown_func_t markdown)
			: name(std::move(provider_name)), base_url(std::move(base_url)),
				get_latest_list(std::move(latest)), search_novels(std::move(search)),
				get_chapter_list(std::move(chapters)), get_chapter_html(std::move(html)),
				parse_html_to_markdown(std::move(markdown)) {
	}
};

#endif // __PROVIDER_HH__
