#ifndef __PROVIDER_REGISTRY_HH__
#define __PROVIDER_REGISTRY_HH__

#include "provider.hh"
#include <functional>
#include <map>

template <typename A = std::allocator<char>>
class ranobe_provider_registry {
private:
	using provider_t = ranobe_provider<A>;
	using __string_t = typename provider_types<A>::__string_t;

	using __map_allocator_t = typename std::allocator_traits<A>::template rebind_alloc<
			std::pair<const __string_t, std::function<provider_t()>>>;
	std::map<__string_t, std::function<provider_t()>, std::less<>, __map_allocator_t> providers;

	using __string_view_allocator_t =
			typename std::allocator_traits<A>::template rebind_alloc<std::string_view>;
	std::vector<std::string_view, __string_view_allocator_t> provider_names;

public:
	explicit ranobe_provider_registry(const A &alloc) : providers(alloc), provider_names(alloc) {
	}

	void add_provider(const __string_t &name, std::function<provider_t()> provider) {
		auto [it, inserted] = providers.emplace(name, std::move(provider));
		if (inserted) {
			provider_names.push_back(it->first);
		}
	}

	std::optional<provider_t>
	find_provider(const typename provider_types<A>::__string_t &name) const {
		auto it = providers.find(name);
		if (it != providers.end()) {
			return it->second();
		}
		return std::nullopt;
	}

	const std::vector<std::string_view, __string_view_allocator_t> &list_providers() const {
		return provider_names;
	}
};

#endif // __PROVIDER_REGISTRY_HH__
