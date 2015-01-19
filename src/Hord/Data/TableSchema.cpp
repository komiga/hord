/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <Hord/utility.hpp>
#include <Hord/String.hpp>
#include <Hord/serialization.hpp>
#include <Hord/Data/TableSchema.hpp>
#include <Hord/IO/Defs.hpp>

#include <duct/debug.hpp>

#include <cstring>

#include <Hord/detail/gr_ceformat.hpp>

namespace Hord {
namespace Data {

// class TableSchema implementation

#define HORD_SCOPE_CLASS TableSchema

TableSchema::TableSchema(
	std::initializer_list<Data::TableSchema::Column> const ilist
) noexcept
	: m_columns(ilist)
{
	update();
}

bool
TableSchema::update() noexcept {
	auto const previous = m_hash;
	if (num_columns() == 0) {
		m_hash = HASH_EMPTY;
	} else {
		HashCombiner hc;
		std::uint32_t const num = static_cast<std::uint32_t>(m_columns.size());
		hc.add(reinterpret_cast<char const*>(&num), sizeof(std::uint32_t));
		for (auto const& column : m_columns) {
			hc.add(reinterpret_cast<char const*>(&column.type), sizeof(Data::TypeValue));
			hc.add_string(column.name);
		}
		m_hash = hc.value();
	}
	return m_hash != previous;
}

bool
TableSchema::assign(
	TableSchema const& schema
) noexcept {
	auto const& new_columns = schema.get_columns();
	bool changed = m_columns.size() != new_columns.size();
	if (!changed) {
		auto x = m_columns.cbegin();
		auto y = new_columns.cbegin();
		for (; x != m_columns.cend(); ++x, ++y) {
			if (x->type != y->type) {
				changed = true;
				break;
			}
		}
	}
	m_columns = new_columns;
	m_hash = schema.get_hash();
	unsigned index = 0;
	for (auto& column : m_columns) {
		column.index = index++;
	}
	return changed;
}

#define HORD_SCOPE_FUNC read
ser_result_type
TableSchema::read(
	ser_tag_read,
	InputSerializer& ser
) {
	m_hash = HASH_EMPTY;
	std::uint32_t format_version;
	ser(format_version);
	DUCT_ASSERTE(format_version == 0);
	ser(Cacophony::make_vector_cfg<std::uint8_t>(m_columns));
	unsigned index = 0;
	for (auto& column : m_columns) {
		column.index = index++;
	}
	update();
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC write
ser_result_type
TableSchema::write(
	ser_tag_write,
	OutputSerializer& ser
) const {
	std::uint32_t const format_version = 0;
	ser(format_version);
	ser(Cacophony::make_vector_cfg<std::uint8_t>(m_columns));
}
#undef HORD_SCOPE_FUNC

#undef HORD_SCOPE_CLASS

} // namespace Data
} // namespace Hord
