#pragma once
#include <dens/registry.hpp>
#include <cassert>

namespace dens {
///
/// \brief Alias for ordering system execution
///
using order_t = std::int64_t;

///
/// \brief Empty data structure
///
struct nodata {};

///
/// \brief Base class template for systems; Data is a customizable execution argument
///
template <typename Data = nodata>
class system {
  public:
	using data_t = Data;

	virtual ~system() = default;

	///
	/// \brief Entry point: user code calls this
	///
	void update(registry const& reg, Data const& data);

  protected:
	///
	/// \brief Customization point: override in concrete systems
	///
	virtual void update(registry const& registry) = 0;
	///
	/// \brief Data argument
	///
	/// Only valid when called within update()
	///
	Data const& data() const;

  private:
	Data const* m_data{};
};

// impl

template <typename Data>
void system<Data>::update(registry const& reg, Data const& data) {
	m_data = &data;
	update(reg);
	m_data = {};
}

template <typename Data>
Data const& system<Data>::data() const {
	assert(m_data != nullptr);
	return *m_data;
}
} // namespace dens
