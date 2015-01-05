
#include <Hord/utility.hpp>
#include <Hord/String.hpp>
#include <Hord/serialization.hpp>
#include <Hord/Data/Table.hpp>
#include <Hord/IO/Defs.hpp>

#include <duct/debug.hpp>

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
	std::uint64_t const num = static_cast<std::uint64_t>(m_columns.size());
	hc.add(reinterpret_cast<char const*>(&num), sizeof(std::uint64_t));
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

#undef HORD_SCOPE_CLASS

// class Table implementation

#define HORD_SCOPE_CLASS Table

Table::Table(
	Data::TableSchema const& schema
) noexcept {
	configure(schema);
}

#define HORD_SCOPE_FUNC configure
bool
Table::configure(
	Data::TableSchema const& schema
) {
	if (schema.get_hash() == m_hash) {
		return false;
	}
	bool changed = false;
	auto const& schema_columns = schema.get_columns();
	auto it_schema = schema_columns.cbegin();
	auto it_table = m_columns.begin();
	for (; it_schema != schema_columns.cend(); ++it_schema, ++it_table) {
		if (it_table == m_columns.end()) {
			it_table = m_columns.insert(it_table, Data::ValueStore{it_schema->type});
			it_table->resize(m_num_rows);
			changed = true;
		} else {
			changed = changed || it_table->set_type(it_schema->type);
		}
	}
	if (it_table != m_columns.end()) {
		m_columns.erase(it_table, m_columns.end());
		changed = true;
	}
	m_hash = schema.get_hash();
	return changed;
}
#undef HORD_SCOPE_FUNC

void
Table::push_back(
	std::initializer_list<Data::ValueRef> const values
) noexcept {
	ValueRef const null_value{};
	auto it_value = values.begin();
	Data::ValueStore::Iterator it;
	for (auto& column : m_columns) {
		it = column.end();
		if (it_value != values.end()) {
			it.insert(*it_value);
			++it_value;
		} else {
			it.insert(null_value);
		}
	}
	++m_num_rows;
}

void
Table::remove(
	unsigned const index
) noexcept {
	if (m_num_rows > index) {
		for (auto& column : m_columns) {
			column.iterator_at(index).remove();
		}
		--m_num_rows;
	}
}

// TODO: Format versioning
#define HORD_SCOPE_FUNC read
ser_result_type
Table::read(
	ser_tag_read,
	InputSerializer& ser
) {
	std::uint32_t format_version = 0;
	ser(format_version);
	std::uint32_t num_rows;
	ser(num_rows);
	ser(Cacophony::make_vector_cfg<std::uint8_t>(m_columns));
	m_num_rows = num_rows;
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC write
ser_result_type
Table::write(
	ser_tag_write,
	OutputSerializer& ser
) const {
	std::uint32_t const format_version = 0;
	ser(format_version);
	ser(static_cast<std::uint32_t>(m_num_rows));
	ser(Cacophony::make_vector_cfg<std::uint8_t>(m_columns));
}
#undef HORD_SCOPE_FUNC

#undef HORD_SCOPE_CLASS // Table

} // namespace Data
} // namespace Hord
