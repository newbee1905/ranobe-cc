#ifndef __PROVIDER_LIGHTNOVELWORLD_HH__
#define __PROVIDER_LIGHTNOVELWORLD_HH__

#include "provider.hh"

template <typename A = std::allocator<char>>
typename provider_types<A>::__vector_t lightnovelworld_get_latest_list(const A &alloc) {
	using types      = provider_types<A>;
	using __string_t = typename types::__string_t;
	using __vector_t = typename types::__vector_t;

	return __vector_t(
			{__string_t("Novel 1", alloc), __string_t("Novel 2", alloc), __string_t("Novel 3", alloc)},
			alloc);
}

template <typename A = std::allocator<char>>
typename provider_types<A>::__vector_t
lightnovelworld_search_novels(const typename provider_types<A>::__string_t &query, const A &alloc) {
	using types      = provider_types<A>;
	using __string_t = typename types::__string_t;
	using __vector_t = typename types::__vector_t;

	return __vector_t({__string_t("Search Result 1", alloc), __string_t("Search Result 2", alloc)},
	                  alloc);
}

template <typename A = std::allocator<char>>
typename provider_types<A>::__vector_t
lightnovelworld_get_chapter_list(const typename provider_types<A>::__string_t &novel,
                                 const A &alloc) {
	using types      = provider_types<A>;
	using __string_t = typename types::__string_t;
	using __vector_t = typename types::__vector_t;

	return __vector_t({__string_t("Chapter 1", alloc), __string_t("Chapter 2", alloc),
	                   __string_t("Chapter 3", alloc)},
	                  alloc);
}

template <typename A = std::allocator<char>>
typename provider_types<A>::__string_t
lightnovelworld_get_chapter_html(const typename provider_types<A>::__string_t &chapter,
                                 const A &alloc) {
	using types      = provider_types<A>;
	using __string_t = typename types::__string_t;

	return __string_t("<html>Chapter Content</html>", alloc);
}

template <typename A = std::allocator<char>>
typename provider_types<A>::__string_t
lightnovelworld_parse_html_to_markdown(const typename provider_types<A>::__string_t &html,
                                       const A &alloc) {
	using types      = provider_types<A>;
	using __string_t = typename types::__string_t;

	return __string_t("# Chapter Title\n\nContent in markdown", alloc);
}

template <typename A = std::allocator<char>>
ranobe_provider<A> create_lightnovelworld_provider(const A &alloc) {
	using types      = provider_types<A>;
	using __string_t = typename types::__string_t;

	return ranobe_provider<A>(
			__string_t("Light Novel World", alloc), __string_t("https://lightnovelworld.co", alloc),
			[&alloc]() { return lightnovelworld_get_latest_list<A>(alloc); },
			[&alloc](const typename types::__string_t &query) {
				return lightnovelworld_search_novels<A>(query, alloc);
			},
			[&alloc](const typename types::__string_t &novel) {
				return lightnovelworld_get_chapter_list<A>(novel, alloc);
			},
			[&alloc](const typename types::__string_t &chapter) {
				return lightnovelworld_get_chapter_html<A>(chapter, alloc);
			},
			[&alloc](const typename types::__string_t &html) {
				return lightnovelworld_parse_html_to_markdown<A>(html, alloc);
			});
}

#endif // __PROVIDER_LIGHTNOVELWORLD_HH__
