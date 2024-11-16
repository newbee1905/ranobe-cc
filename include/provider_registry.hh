#ifndef __PROVIDER_REGISTRY_HH__
#define __PROVIDER_REGISTRY_HH__

#include "provider.hh"
#include <functional>
#include <map>

class ranobe_provider_registry {
private:
	std::map<std::string, std::function<ranobe_provider()>> providers;
	std::vector<std::string_view> provider_names;

public:
	explicit ranobe_provider_registry();

	void add_provider(const std::string &name, std::function<ranobe_provider()> provider);

	std::optional<ranobe_provider> find_provider(const std::string &name) const;

	const std::vector<std::string_view> &list_providers() const;
};

#endif // __PROVIDER_REGISTRY_HH__
