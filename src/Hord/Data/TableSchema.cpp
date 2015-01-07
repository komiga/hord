
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
	HashCombiner hc;
	std::uint32_t const num = static_cast<std::uint32_t>(m_columns.size());
	hc.add(reinterpret_cast<char const*>(&num), sizeof(std::uint32_t));
	for (auto const& column : m_columns) {
		hc.add(reinterpret_cast<char const*>(&column.type), sizeof(Data::TypeValue));
		hc.add_string(column.name);
	}
	m_hash = hc.value();
	return m_hash != previous;
}

#define HORD_SCOPE_FUNC insert
namespace {
HORD_DEF_FMT_FQN(
	s_err_column_name_shared,
	"cannot insert column with non-unique name `%s`"
);
} // anonymous namespace

void
TableSchema::insert(
	unsigned const index,
	String name,
	Data::Type const type
) {
	DUCT_ASSERTE(index <= m_columns.size());
	for (auto const& column : m_columns) {
		if (column.name == name) {
			HORD_THROW_FMT(
				ErrorCode::table_schema_column_name_shared,
				s_err_column_name_shared,
				name
			);
		}
	}
	m_columns.insert(
		m_columns.cbegin() + index,
		Data::TableSchema::Column{std::move(name), type}
	);
}
#undef HORD_SCOPE_FUNC

void
TableSchema::remove(
	unsigned const index
) noexcept {
	DUCT_ASSERTE(index < m_columns.size());
	m_columns.erase(m_columns.cbegin() + index);
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
