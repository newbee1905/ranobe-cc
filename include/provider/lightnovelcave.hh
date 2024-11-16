#ifndef __PROVIDER_LIGHTNOVELCAVE_HH__
#define __PROVIDER_LIGHTNOVELCAVE_HH__

#include "provider.hh"

std::vector<std::string> lightnovelcave_get_latest_list() {
	return std::vector<std::string>({"Novel 1", "Novel 2", "Novel 3"});
}

std::vector<std::string> lightnovelcave_search_novels(const std::string &query) {
	return std::vector<std::string>({"Search Result 1", "Search Result 2", "Search Result 3"});
}

std::vector<std::string> lightnovelcave_get_chapter_list(const std::string &novel) {
	return std::vector<std::string>({"Chapter 1", "Chapter 2", "Chapter 3"});
}

std::string lightnovelcave_get_chapter_html(const std::string &chapter) {
	return "<html>Chapter Content</html>";
}

std::string lightnovelcave_parse_html_to_markdown(const std::string &html) {
	return "# Chapter Title\n\nContent in markdown";
}

ranobe_provider create_lightnovelcave_provider() {
	return ranobe_provider(
		"Light Novel Cave", "https://lightnovelcave.co", lightnovelcave_get_latest_list,
		lightnovelcave_search_novels, lightnovelcave_get_chapter_list, lightnovelcave_get_chapter_html,
		lightnovelcave_parse_html_to_markdown
	);
}

#endif // __PROVIDER_LIGHTNOVELCAVE_HH__
