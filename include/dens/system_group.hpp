#include <dens/system.hpp>
#include <type_traits>

namespace dens {
template <typename T, typename Data>
concept System = std::is_base_of_v<system<Data>, T>;

///
/// \brief Root container of concrete system instances
///
/// Each attached system must be a unique type
///
template <typename Data>
class system_group : public system<Data> {
  public:
	using sign_t = detail::sign_t;
	using system<Data>::update;

	///
	/// \brief Attach a concrete system with ordering
	///
	template <System<Data> S, typename... Args>
	S& attach(order_t order = 0, Args&&... args);
	///
	/// \brief Find an attached system via its concrete type
	///
	template <System<Data> S>
	S* find() const noexcept;
	///
	/// \brief Check whether system of concrete type S is attached
	///
	template <System<Data> S>
	bool attached() const noexcept;
	///
	/// \brief Detach system of concrete type S
	///
	template <System<Data> S>
	void detach();
	///
	/// \brief Reorder an attached system of concrete type S
	///
	template <System<Data> S>
	bool reorder(order_t order);

	void clear() noexcept { m_entries.clear(); }
	std::size_t size() const noexcept { return m_entries.size(); }
	bool empty() const noexcept { return m_entries.empty(); }

  private:
	void update(registry const& reg) override;

	struct entry_t {
		std::unique_ptr<system<Data>> sys;
		order_t order{};
	};

	std::unordered_map<sign_t, entry_t, sign_t::hasher> m_entries;
};

// impl

template <typename Data>
template <System<Data> S, typename... Args>
S& system_group<Data>::attach(order_t order, Args&&... args) {
	auto s = std::make_unique<S>(std::forward<Args>(args)...);
	auto& ret = *s;
	m_entries.insert_or_assign(sign_t::make<S>(), entry_t{.sys = std::move(s), .order = order});
	return ret;
}

template <typename Data>
template <System<Data> S>
S* system_group<Data>::find() const noexcept {
	if (auto it = m_entries.find(sign_t::make<S>()); it != m_entries.end()) { return static_cast<S*>(it->second.system.get()); }
	return {};
}

template <typename Data>
template <System<Data> S>
bool system_group<Data>::attached() const noexcept {
	return find<S>() != nullptr;
}

template <typename Data>
template <System<Data> S>
void system_group<Data>::detach() {
	m_entries.erase(sign_t::make<S>());
}

template <typename Data>
template <System<Data> S>
bool system_group<Data>::reorder(order_t order) {
	if (auto s = find<S>()) {
		s->order = order;
		return true;
	}
	return false;
}

template <typename Data>
void system_group<Data>::update(registry const& registry) {
	if (m_entries.size() < 2) {
		for (auto& [_, entry] : m_entries) { entry.sys->update(registry, this->data()); }
		return;
	}
	std::vector<entry_t*> sorted;
	sorted.reserve(m_entries.size());
	for (auto& [_, entry] : m_entries) { sorted.push_back(&entry); }
	std::sort(sorted.begin(), sorted.end(), [](entry_t const* l, entry_t const* r) { return l->order < r->order; });
	for (entry_t* entry : sorted) { entry->sys->update(registry, this->data()); }
}
} // namespace dens
