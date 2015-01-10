/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <Hord/utility.hpp>
#include <Hord/String.hpp>
#include <Hord/serialization.hpp>
#include <Hord/Data/Defs.hpp>
#include <Hord/Data/ValueRef.hpp>
#include <Hord/Data/TableSchema.hpp>
#include <Hord/Data/Table.hpp>
#include <Hord/IO/Defs.hpp>

#include <duct/debug.hpp>

#include <cstring>
#include <utility>

#include <Hord/detail/gr_ceformat.hpp>

namespace Hord {
namespace Data {

namespace {

constexpr static unsigned const
CHUNK_SIZE = 0x2000;

struct Record {
	unsigned size;
	std::uint8_t* data;
};

/*inline static unsigned
value_zero_size(
	Data::Type const type
) noexcept {
	if (type.type() == Data::ValueType::dynamic) {
		return sizeof(Data::TypeValue);
	}
	auto const& vp = Data::type_properties(type);
	if (vp.flags & Data::VTP_DYNAMIC_SIZE) {
		return Data::size_meta(type.size());
	} else {
		return vp.fixed_size[enum_cast(type.size())];
	}
}*/

inline static unsigned
value_init_size(
	Data::Type const type
) {
	if (type.type() == Data::ValueType::dynamic) {
		return sizeof(Data::TypeValue) + 0x20;
	}
	auto const& vp = Data::type_properties(type);
	if (vp.flags & Data::VTP_DYNAMIC_SIZE) {
		return Data::size_meta(type.size()) * 0x10;
	} else {
		return vp.fixed_size[enum_cast(type.size())];
	}
}

inline static unsigned
value_data_size(
	Data::ValueRef const& value
) {
	DUCT_DEBUG_ASSERTE(value.type.type() != Data::ValueType::dynamic);
	auto const& vp = Data::type_properties(value.type);
	if (vp.flags & Data::VTP_DYNAMIC_SIZE) {
		return value.size;
	} else {
		return vp.fixed_size[enum_cast(value.type.size())];
	}
}

inline static unsigned
value_written_size(
	Data::ValueRef const& value,
	bool const write_type
) {
	DUCT_DEBUG_ASSERTE(value.type.type() != Data::ValueType::dynamic);
	unsigned meta_size = write_type ? sizeof(Data::TypeValue) : 0;
	if (Data::type_properties(value.type).flags & Data::VTP_DYNAMIC_SIZE) {
		meta_size += Data::size_meta(value.type.size());
	}
	return meta_size + value_data_size(value);
}

static unsigned
value_read_size(
	Data::Type type,
	std::uint8_t const* data
) {
	if (type.type() == Data::ValueType::dynamic) {
		type.set_value(*reinterpret_cast<Data::TypeValue const*>(data));
		data += sizeof(Data::TypeValue);
		DUCT_DEBUG_ASSERTE(type.type() != Data::ValueType::dynamic);
	}
	auto const& vp = Data::type_properties(type);
	if (vp.flags & Data::VTP_DYNAMIC_SIZE) {
		switch (Data::size_meta(type.size())) {
		case 1: return *reinterpret_cast<std::uint8_t const*>(data);
		case 2: return *reinterpret_cast<std::uint16_t const*>(data);
		case 4: return *reinterpret_cast<std::uint32_t const*>(data);
		default: return 0;
		}
	} else {
		return vp.fixed_size[enum_cast(type.size())];
	}
}

static unsigned
value_read_size_whole(
	Data::Type type,
	std::uint8_t const* data
) {
	unsigned meta_size = 0;
	if (type.type() == Data::ValueType::dynamic) {
		type.set_value(*reinterpret_cast<Data::TypeValue const*>(data));
		data += sizeof(Data::TypeValue);
		meta_size += sizeof(Data::TypeValue);
		DUCT_DEBUG_ASSERTE(type.type() != Data::ValueType::dynamic);
	}
	if (Data::type_properties(type).flags & Data::VTP_DYNAMIC_SIZE) {
		meta_size += Data::size_meta(type.size());
	}
	return meta_size + value_read_size(type, data);
}

static ValueRef
value_read(
	Data::Type type,
	std::uint8_t const* data
) {
	if (type.type() == Data::ValueType::dynamic) {
		type.set_value(*reinterpret_cast<Data::TypeValue const*>(data));
		data += sizeof(Data::TypeValue);
		DUCT_DEBUG_ASSERTE(type.type() != Data::ValueType::dynamic);
	}
	ValueRef value{};
	value.type = type;
	auto const& vp = Data::type_properties(type);
	if (type.type() == Data::ValueType::null) {
		// Do nothing
	} else if (vp.flags & Data::VTP_DYNAMIC_SIZE) {
		switch (Data::size_meta(type.size())) {
		case 1: value.size = *reinterpret_cast<std::uint8_t const*>(data); break;
		case 2: value.size = *reinterpret_cast<std::uint16_t const*>(data); break;
		case 4: value.size = *reinterpret_cast<std::uint32_t const*>(data); break;
		}
		data += Data::size_meta(type.size());
		value.data.dynamic = data;
	} else {
		switch (vp.fixed_size[enum_cast(type.size())]) {
		case 1: value.data.u8 = *reinterpret_cast<std::uint8_t const*>(data); break;
		case 2: value.data.u16 = *reinterpret_cast<std::uint16_t const*>(data); break;
		case 4: value.data.u32 = *reinterpret_cast<std::uint32_t const*>(data); break;
		case 8: value.data.u64 = *reinterpret_cast<std::uint64_t const*>(data); break;
		}
	}
	return value;
}

static unsigned
value_write(
	Data::ValueRef const& value,
	std::uint8_t* output,
	bool const write_type
) {
	DUCT_DEBUG_ASSERTE(value.type.type() != Data::ValueType::dynamic);
	auto* const head = output;
	if (write_type) {
		*reinterpret_cast<Data::TypeValue*>(output) = value.type.value();
		output += sizeof(Data::TypeValue);
	}
	auto const& vp = Data::type_properties(value.type);
	if (value.type.type() == Data::ValueType::null) {
		// Do nothing
	} else if (vp.flags & Data::VTP_DYNAMIC_SIZE) {
		unsigned const meta_size = Data::size_meta(value.type.size());
		switch (meta_size) {
		case 1: *reinterpret_cast<std::uint8_t*>(output) = min_ce(value.size, static_cast<unsigned>(~std::uint8_t{0})); break;
		case 2: *reinterpret_cast<std::uint16_t*>(output) = min_ce(value.size, static_cast<unsigned>(~std::uint16_t{0})); break;
		case 4: *reinterpret_cast<std::uint32_t*>(output) = min_ce(value.size, static_cast<unsigned>(~std::uint32_t{0})); break;
		}
		output += meta_size;
		// NB: When modifying a record, the destination and source could overlap
		std::memmove(output, value.data.dynamic, value.size);
		output += value.size;
	} else {
		unsigned const size = vp.fixed_size[enum_cast(value.type.size())];
		switch (size) {
		case 1: *reinterpret_cast<std::uint8_t*>(output) = value.data.u8; break;
		case 2: *reinterpret_cast<std::uint16_t*>(output) = value.data.u16; break;
		case 4: *reinterpret_cast<std::uint32_t*>(output) = value.data.u32; break;
		case 8: *reinterpret_cast<std::uint64_t*>(output) = value.data.u64; break;
		}
		output += size;
	}
	return output - head;
}

inline static void
chunk_clear(
	Data::Table::Chunk& chunk
) noexcept {
	chunk.head = chunk.data;
	chunk.tail = chunk.data;
	chunk.num_records = 0;
}

static void
chunk_free(
	Data::Table::Chunk& chunk
) noexcept {
	if (chunk.data) {
		delete[] chunk.data;
	}
	chunk.data = nullptr;
	chunk.head = nullptr;
	chunk.tail = nullptr;
	chunk.size = 0;
	chunk.num_records = 0;
}

static void
chunk_allocate(
	Data::Table::Chunk& chunk,
	unsigned const size
) noexcept {
	DUCT_ASSERTE(size > 0);
	chunk_free(chunk);
	chunk.data = new std::uint8_t[size];
	chunk.size = size;
	chunk_clear(chunk);
}

static void
chunk_split(
	Data::Table::Chunk& chunk,
	Data::Table::Chunk& split,
	unsigned const min_size,
	unsigned const num_records,
	unsigned const begin,
	unsigned const end,
	unsigned const head_space,
	unsigned const tail_space
) {
	// FIXME: Bad for reverse insertion; copy data to:
	// 1. tail of first chunk
	// 2. center of inner chunk
	// 3. head of tail chunk
	// to work better under common usage patterns
	unsigned const size = end - begin;
	chunk_allocate(split, max_ce(min_size, head_space + size + tail_space));
	split.head += head_space;
	split.tail = split.head + size;
	std::memcpy(split.head, chunk.data + begin, size);
	split.num_records = num_records;
	chunk.num_records -= num_records;
}

static void
chunk_split_head(
	Data::Table::Chunk& chunk,
	Data::Table::Chunk& split,
	Data::Table::Iterator& it,
	unsigned const min_size,
	unsigned const tail_space
) {
	chunk_split(
		chunk, split,
		min_size, it.inner_index,
		chunk.offset_head(), it.data_offset,
		0, tail_space
	);
	it.data_offset = split.offset_tail();
}

static void
chunk_split_tail(
	Data::Table::Chunk& chunk,
	Data::Table::Chunk& split,
	Data::Table::Iterator& it,
	unsigned const min_size,
	unsigned const head_space
) {
	chunk_split(
		chunk, split,
		min_size, chunk.num_records - it.inner_index,
		it.data_offset, chunk.offset_tail(),
		head_space, 0
	);
	++it.chunk_index;
	it.inner_index = 0;
	it.data_offset = split.offset_head();
}

inline static void
chunk_set_bounds(
	Data::Table::Chunk& chunk,
	unsigned const num_records,
	unsigned const head,
	unsigned const tail
) {
	DUCT_ASSERTE(
		head < tail &&
		tail <= chunk.size &&
		chunk.size >= (tail - head)
	);
	chunk.num_records = num_records;
	chunk.head = chunk.data + head;
	chunk.tail = chunk.data + tail;
}

static bool
segment_resize(
	Data::Table::Chunk& chunk,
	Data::Table::Chunk& split,
	Data::Table::Iterator& it,
	unsigned const old_size,
	unsigned const new_size
) {
	if (old_size == new_size) {
		return false;
	}
	// TODO: Insertion into adjacent chunks
	// TODO: Move both sections if the value fits within their sum,
	// but not within them individually
	// TODO: Minimum segment size by type
	unsigned const from_head = it.data_offset - chunk.offset_head();
	unsigned const from_tail = chunk.offset_tail() - it.data_offset;
	unsigned const diff
		= old_size > new_size
		? old_size - new_size
		: new_size - old_size
	;
	if (old_size > new_size) {
		if (from_head < from_tail) {
			std::memmove(chunk.head + diff, chunk.head, from_head);
			chunk.head += diff;
			it.data_offset += diff;
		} else {
			std::memmove(chunk.head + from_head + new_size, chunk.head + from_head + old_size, from_tail - old_size);
			chunk.tail -= diff;
		}
	} else if (chunk.space_head() >= new_size) {
		std::memmove(chunk.head - diff, chunk.head, from_head);
		chunk.head -= diff;
		it.data_offset -= diff;
	} else if (chunk.space_tail() >= new_size) {
		std::memmove(chunk.head + from_head + new_size, chunk.head + from_head + old_size, from_tail - old_size);
		chunk.tail += diff;
	} else if (from_head < from_tail) {
		chunk_split_head(chunk, split, it, max_ce(new_size, CHUNK_SIZE), new_size);
		split.tail += new_size;
		return true;
	} else {
		chunk_split_tail(chunk, split, it, max_ce(new_size, CHUNK_SIZE), new_size);
		split.head -= new_size;
		it.data_offset -= new_size;
		return true;
	}
	return false;
}

inline constexpr static unsigned
record_meta_size() {
	return sizeof(std::uint32_t);
}

inline constexpr static unsigned
record_written_size(
	unsigned const data_size
) {
	return record_meta_size() + data_size;
}

inline constexpr static unsigned
record_written_size(
	Record const& record
) {
	return record_written_size(record.size);
}

static unsigned
record_write(
	Record const& record,
	std::uint8_t* output
) {
	*reinterpret_cast<std::uint32_t*>(output) = record.size;
	output += record_meta_size();
	std::memcpy(output, record.data, record.size);
	return record_meta_size() + record.size;
}

static Record
record_read(
	std::uint8_t* data
) {
	return {
		*reinterpret_cast<std::uint32_t const*>(data),
		data + record_meta_size()
	};
}

static bool
record_resize(
	Record& record,
	Data::Table::Chunk& chunk,
	Data::Table::Chunk& split,
	Data::Table::Iterator& it,
	unsigned const new_size
) {
	if (record.size == new_size) {
		return false;
	}
	bool const record_moved = segment_resize(
		chunk, split, it,
		record_written_size(record),
		record_meta_size() + new_size
	);
	record.size = new_size;
	record.data = (record_moved ? split.data : chunk.data) + it.data_offset;
	*reinterpret_cast<std::uint32_t*>(record.data) = record.size;
	record.data += record_meta_size();
	return record_moved;
}

static bool
record_make(
	Record& record,
	Data::Table::Chunk& chunk,
	Data::Table::Chunk& split,
	Data::Table::Iterator& it,
	unsigned const size
) {
	bool const record_moved = segment_resize(
		chunk, split, it,
		0, record_meta_size() + size
	);
	record.size = size;
	record.data = (record_moved ? split.data : chunk.data) + it.data_offset;
	*reinterpret_cast<std::uint32_t*>(record.data) = record.size;
	record.data += record_meta_size();
	return record_moved;
}

static unsigned
field_offset(
	Record const& record,
	Data::TableSchema const& schema,
	unsigned index
) noexcept {
	unsigned const num = min_ce(index, schema.num_columns());
	unsigned offset = 0;
	for (index = 0; index < num; ++index) {
		offset += value_read_size_whole(schema.column(index).type, record.data + offset);
	}
	DUCT_DEBUG_ASSERTE(offset <= record.size);
	return offset;
}

inline static unsigned
record_data_size(
	Record const& record,
	Data::TableSchema const& schema
) noexcept {
	return field_offset(record, schema, schema.num_columns());
}

} // anonymous namespace

// class Table::Iterator implementation

Table::Iterator&
Table::Iterator::operator++() noexcept {
	++index;
	if (index < table->m_num_records) {
		auto const* chunk = &table->m_chunks[chunk_index];
		++inner_index;
		if (inner_index < chunk->num_records) {
			auto const record = record_read(chunk->data + data_offset);
			data_offset += record_written_size(record);
			DUCT_DEBUG_ASSERTE(data_offset <= chunk->size);
		} else {
			++chunk_index;
			inner_index = 0;
			DUCT_ASSERTE(chunk_index < table->m_chunks.size());
			chunk = &table->m_chunks[chunk_index];
			data_offset = chunk->offset_head();
		}
	}
	return *this;
}

Table::Iterator&
Table::Iterator::operator+=(
	unsigned count
) noexcept {
	index += count;
	if (index < table->m_num_records) {
		auto const* chunk = &table->m_chunks[chunk_index];
		while (inner_index + count > chunk->num_records) {
			count -= chunk->num_records - inner_index;
			++chunk_index;
			inner_index = 0;
			chunk = &table->m_chunks[chunk_index];
			data_offset = chunk->offset_head();
		}
		Record record;
		while (count--) {
			record = record_read(chunk->data + data_offset);
			data_offset += record_written_size(record);
			DUCT_DEBUG_ASSERTE(data_offset <= chunk->size);
		}
	} else {
		*this = table->end();
	}
	return *this;
}

// class Table implementation

#define HORD_SCOPE_CLASS Table

void Table::free_chunks() {
	for (auto& chunk : m_chunks) {
		chunk_free(chunk);
	}
	m_chunks.clear();
	m_num_records = 0;
}

Table::~Table() noexcept {
	free_chunks();
}

Table::Table(
	Table&& other
) noexcept {
	(*this) = std::move(other);
}

Table&
Table::operator=(
	Table&& other
) noexcept {
	clear();
	std::swap(m_num_records, other.m_num_records);
	std::swap(m_schema, other.m_schema);
	std::swap(m_chunks, other.m_chunks);
	other.clear();
	return *this;
}

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
	bool changed = m_schema.assign(schema);
	if (changed) {
		clear();
	}
	return changed;
}
#undef HORD_SCOPE_FUNC

void
Table::clear() noexcept {
	for (auto& chunk : m_chunks) {
		chunk_clear(chunk);
	}
	m_num_records = 0;
}

static void
table_write_records(
	Data::Table::Chunk& chunk,
	aux::vector<Record>& records,
	unsigned const size
) {
	if (chunk.size < size) {
		chunk_allocate(chunk, size);
	}
	unsigned offset = 0;
	for (auto const& record : records) {
		offset += record_write(record, chunk.data + offset);
	}
	chunk_set_bounds(chunk, records.size(), 0, offset);
	records.clear();
}

void
Table::optimize_storage() {
	if (empty()) {
		return;
	}

	m_chunks.insert(m_chunks.cbegin(), Data::Table::Chunk{});
	auto it_put = m_chunks.begin();
	auto it_take = it_put + 1;
	unsigned offset;
	unsigned take_count = 0;
	unsigned accum_data_size = 0;
	unsigned put_capacity = CHUNK_SIZE;
	Record orig_record;
	aux::vector<Record> records{};
	records.reserve(256);
	for (; it_take != m_chunks.end(); ++it_take) {
		offset = it_take->offset_head();
		for (unsigned index = 0; index < it_take->num_records; ++index) {
			orig_record = record_read(it_take->data + offset);
			offset += record_written_size(orig_record);
			records.push_back({});
			auto& record = records.back();
			record.data = orig_record.data;
			record.size = record_data_size(orig_record, m_schema);
			accum_data_size += record_written_size(record);
			if (0 < take_count && put_capacity <= accum_data_size) {
				table_write_records(*it_put, records, max_ce(put_capacity, accum_data_size));
				++it_put;
				take_count = 0;
				accum_data_size = 0;
				put_capacity = max_ce(it_put->size, CHUNK_SIZE);
			}
		}
		++take_count;
	}
	if (!records.empty()) {
		table_write_records(*it_put, records, max_ce(put_capacity, accum_data_size));
		++it_put;
	}
	it_take = it_put;
	for (; it_take != m_chunks.end(); ++it_take) {
		chunk_free(*it_take);
	}
	m_chunks.erase(it_put, m_chunks.end());
}

void
Table::insert(
	Data::Table::Iterator& it,
	unsigned num_fields,
	Data::ValueRef* const fields
) noexcept {
	DUCT_ASSERTE(it.table == this);

	Record record;
	{ // Calculate record size
	// TODO: Cache init size
	num_fields = min_ce(num_columns(), num_fields);
	unsigned record_size = 0;
	unsigned index = 0;
	for (; index < num_fields; ++index) {
		auto& value = fields[index];
		auto const type = column(index).type;
		bool const is_dynamic = type == Data::ValueType::dynamic;
		value.morph(type);
		record_size += max_ce(
			value_init_size(type),
			value_written_size(value, is_dynamic)
		);
	}
	for (; index < num_columns(); ++index) {
		record_size += value_init_size(column(index).type);
	}

	if (m_chunks.empty()) {
		Data::Table::Chunk chunk{};
		chunk_allocate(chunk, max_ce(record_written_size(record_size), CHUNK_SIZE));
		m_chunks.push_back(chunk);
		it = begin();
	}
	// Make record
	Data::Table::Chunk split{};
	if (record_make(record, m_chunks[it.chunk_index], split, it, record_size)) {
		m_chunks.insert(m_chunks.cbegin() + it.chunk_index, split);
	}}

	// Write supplied field values
	unsigned offset = 0;
	unsigned index = 0;
	for (; index < num_fields; ++index) {
		auto const& value = fields[index];
		bool const is_dynamic = column(index).type.type() == Data::ValueType::dynamic;
		offset += value_write(value, record.data + offset, is_dynamic);
	}
	// Zero the rest of the record
	if (index < num_columns()) {
		std::memset(record.data + offset, 0, record.size - offset);
	}
	++m_chunks[it.chunk_index].num_records;
	++m_num_records;
}

void
Table::remove(
	Data::Table::Iterator& it
) noexcept {
	DUCT_ASSERTE(it.table == this);
	if (!it.can_advance()) {
		return;
	}
	auto& chunk = m_chunks[it.chunk_index];
	unsigned const size = record_written_size(record_read(chunk.data + it.data_offset));
	Data::Table::Chunk split_unused{};
	DUCT_ASSERTE(!segment_resize(chunk, split_unused, it, size, 0));
	--chunk.num_records;
	--m_num_records;
	if (chunk.num_records == 0) {
		chunk_free(chunk);
		m_chunks.erase(m_chunks.cbegin() + it.chunk_index);
		if (it.chunk_index < m_chunks.size()) {
			it.inner_index = 0;
			it.data_offset = m_chunks[it.chunk_index].offset_head();
		} else {
			it = iterator_at(it.index);
		}
	}
}

void
Table::set_field(
	Data::Table::Iterator& it,
	unsigned column_index,
	Data::ValueRef new_value
) {
	DUCT_ASSERTE(it.table == this);
	if (column_index >= num_columns() || !it.can_advance()) {
		return;
	}
	auto const type = column(column_index).type;
	if (type.type() == Data::ValueType::null) {
		return;
	}
	bool const is_dynamic = type.type() == Data::ValueType::dynamic;
	new_value.morph(type);

	auto record = record_read(m_chunks[it.chunk_index].data + it.data_offset);
	unsigned const offset = field_offset(record, m_schema, column_index);
	unsigned const old_size = value_read_size_whole(type, record.data + offset);
	unsigned const new_size = value_written_size(new_value, is_dynamic);
	++column_index;
	unsigned offset_last = offset + old_size;
	if (new_size != old_size) {
		for (; column_index < num_columns(); ++column_index) {
			offset_last += value_read_size_whole(column(column_index).type, record.data + offset_last);
		}
	}

	// Only resize if the new value cannot fit within the record's current size
	if (record.size - (offset_last - old_size) < new_size) {
		Data::Table::Chunk split{};
		if (record_resize(record, m_chunks[it.chunk_index], split, it, (record.size - old_size) + new_size)) {
			m_chunks.insert(m_chunks.cbegin() + it.chunk_index, split);
		}
	}
	// Shift after if the record will be enlarged
	std::memmove(
		record.data + offset + new_size,
		record.data + offset + old_size,
		offset_last - (offset + old_size)
	);
	value_write(new_value, record.data + offset, is_dynamic);
}

Data::ValueRef
Table::get_field(
	Data::Table::Iterator const& it,
	unsigned const column_index
) const noexcept {
	DUCT_ASSERTE(it.table == this);
	if (column_index >= num_columns() || !it.can_advance()) {
		return {};
	}
	auto const type = column(column_index).type;
	if (type.type() == Data::ValueType::null) {
		return {};
	}
	auto const& chunk = m_chunks[it.chunk_index];
	auto const record = record_read(chunk.data + it.data_offset);
	unsigned const offset = field_offset(record, m_schema, column_index);
	return value_read(type, record.data + offset);
}

// TODO: Format versioning
#define HORD_SCOPE_FUNC read
ser_result_type
Table::read(
	ser_tag_read,
	InputSerializer& ser
) {
	free_chunks();

	std::uint32_t format_version;
	ser(format_version);
	DUCT_ASSERTE(format_version == 0);
	ser(m_schema);

	std::uint32_t num_chunks;
	ser(num_chunks);
	m_chunks.reserve(num_chunks);

	std::uint32_t num_records;
	std::uint32_t data_size;
	for (; num_chunks > 0; --num_chunks) {
		Data::Table::Chunk chunk{};
		ser(num_records, data_size);
		chunk_allocate(chunk, max_ce(data_size, CHUNK_SIZE));
		chunk_set_bounds(chunk, num_records, 0, data_size);
		ser(Cacophony::make_binary_blob(chunk.head, data_size));
		m_num_records += chunk.num_records;
		m_chunks.push_back(chunk);
	}
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC write
ser_result_type
Table::write(
	ser_tag_write,
	OutputSerializer& ser
) const {
	const_cast<Data::Table*>(this)->optimize_storage();
	std::uint32_t const format_version = 0;
	ser(format_version);
	ser(m_schema);

	ser(static_cast<std::uint32_t>(m_chunks.size()));
	std::uint32_t num_records;
	std::uint32_t data_size;
	for (auto const& chunk : m_chunks) {
		num_records = static_cast<std::uint32_t>(chunk.num_records);
		data_size = static_cast<std::uint32_t>(chunk.space_used());
		ser(num_records, data_size);
		ser(Cacophony::make_binary_blob(chunk.head, data_size));
	}
}
#undef HORD_SCOPE_FUNC

#undef HORD_SCOPE_CLASS // Table

} // namespace Data
} // namespace Hord
