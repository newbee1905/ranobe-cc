#ifndef __PROVIDER_HH__
#define __PROVIDER_HH__

#include <algorithm>
#include <functional>
#include <optional>
#include <string>
#include <vector>

struct ranobe_provider {
	using __list_func_t         = std::function<std::vector<std::string>()>;
	using __search_func_t       = std::function<std::vector<std::string>(const std::string &)>;
	using __chapter_list_func_t = std::function<std::vector<std::string>(const std::string &)>;
	using __chapter_html_func_t = std::function<std::string(const std::string &)>;
	using __parse_html_to_markdown_func_t = std::function<std::string(const std::string &)>;

	std::string name;
	std::string base_url;

	__list_func_t get_latest_list;
	__search_func_t search_novels;
	__chapter_list_func_t get_chapter_list;
	__chapter_html_func_t get_chapter_html;
	__parse_html_to_markdown_func_t parse_html_to_markdown;

	ranobe_provider(
		std::string provider_name, std::string base_url, __list_func_t latest, __search_func_t search,
		__chapter_list_func_t chapters, __chapter_html_func_t html,
		__parse_html_to_markdown_func_t markdown
	)
			: name(std::move(provider_name)), base_url(std::move(base_url)),
				get_latest_list(std::move(latest)), search_novels(std::move(search)),
				get_chapter_list(std::move(chapters)), get_chapter_html(std::move(html)),
				parse_html_to_markdown(std::move(markdown)) {
	}
};

#endif // __PROVIDER_HH__
