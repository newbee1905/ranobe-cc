#include "provider_registry.hh"

ranobe_provider_registry::ranobe_provider_registry() : providers(), provider_names() {
}

void ranobe_provider_registry::add_provider(
	const std::string &name, std::function<ranobe_provider()> provider
) {
	auto [it, inserted] = providers.emplace(name, std::move(provider));
	if (inserted) {
		provider_names.push_back(it->first);
	}
}

std::optional<ranobe_provider> ranobe_provider_registry::find_provider(const std::string &name
) const {
	auto it = providers.find(name);
	if (it != providers.end()) {
		return it->second();
	}
	return std::nullopt;
}

const std::vector<std::string_view> &ranobe_provider_registry::list_providers() const {
	return provider_names;
}
