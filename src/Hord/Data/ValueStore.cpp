
#include <Hord/utility.hpp>
#include <Hord/String.hpp>
#include <Hord/serialization.hpp>
#include <Hord/Data/Defs.hpp>
#include <Hord/Data/ValueStore.hpp>

#include <duct/debug.hpp>

#include <cstring>

#include <Hord/detail/gr_ceformat.hpp>

namespace Hord {
namespace Data {

namespace {

enum : unsigned {
	VTP_NONE = 0,
	VTP_DYNAMIC_SIZE = 1 << 0,
};

inline constexpr static unsigned
data_size_fixed(
	Data::Size const size
) noexcept {
	return 1u << enum_cast(size);
}

inline constexpr static unsigned
data_size_dynamic(
	Data::Size const size
) noexcept {
	return min_ce(4u, 1u << enum_cast(size));
}

inline static std::int64_t
value_clamp_signed(
	std::int64_t const value,
	Data::Size const size
) noexcept {
	auto const min = std::int64_t{1} << ((enum_cast(size) + 3) - 1);
	return max_ce(min, min_ce(value, ~(~std::int64_t{0} << (enum_cast(size) + 3))));
}

inline static std::uint64_t
value_clamp_unsigned(
	std::uint64_t const value,
	Data::Size const size
) noexcept {
	return max_ce(std::uint64_t{0}, min_ce(value, ~std::uint64_t{0} >> (enum_cast(size) + 3)));
}

struct ValueTypeProperties {
	unsigned const flags;
	unsigned const chunk_capacity;
	// TODO: Use approximate_size in value read/write instead of data_size_fixed()
	// (approximate_size is poorly named; it is the fixed size when non-dynamic, and may differ from the ordering of the data size)
	unsigned const approximate_size[4];
};

#define HORD_DYNAMIC_SIZE(s, approx) \
	(sizeof(Data::TypeValue) + data_size_dynamic(Data::Size:: s) + approx)

constexpr static ValueTypeProperties const
s_properties[]{
	// null
	{VTP_NONE, 0, {0}},

	// dynamic
	{
		VTP_NONE,
		0x80,
		{
			HORD_DYNAMIC_SIZE(b8, 0x20),
			HORD_DYNAMIC_SIZE(b16, 0x200),
			HORD_DYNAMIC_SIZE(b32, 0x400),
			HORD_DYNAMIC_SIZE(b64, 0x1000)
		}
	},

	// integer
	{VTP_NONE, 0x1000, {1, 2, 4, 8}},

	// decimal
	{VTP_NONE, 0x1000, {4, 4, 4, 8}},

	// object_id
	{VTP_NONE, 0x1000, {sizeof(Object::IDValue)}},

	// string
	{
		VTP_DYNAMIC_SIZE,
		0x100,
		{1 + 0x20, 2 + 0x400, 4 + 0x1000, 8 + 0x4000}
	},
};

#undef HORD_DYNAMIC_SIZE

inline constexpr static ValueTypeProperties const&
value_properties(
	Data::Type const type
) noexcept {
	return s_properties[enum_cast(type.type())];
}

/*inline constexpr static unsigned
value_approximate_size(
	Data::Type const type
) noexcept {
	return
		s_properties[enum_cast(type.type())]
		.approximate_size[enum_cast(type.size())]
	;
}*/

inline static unsigned
value_zero_size(
	Data::Type const type
) noexcept {
	if (type.type() == Data::ValueType::dynamic) {
		return sizeof(Data::TypeValue);
	} else if (value_properties(type).flags & VTP_DYNAMIC_SIZE) {
		return data_size_dynamic(type.size());
	} else {
		return data_size_fixed(type.size());
	}
}

inline constexpr static unsigned
value_chunk_capacity(
	Data::Type const type
) {
	return value_properties(type).chunk_capacity;
}

inline constexpr static unsigned
value_chunk_size_vp(
	Data::Type const type,
	ValueTypeProperties const& vp
) {
	return vp.chunk_capacity * vp.approximate_size[enum_cast(type.size())];
}

inline constexpr static unsigned
value_chunk_size(
	Data::Type const type
) {
	return value_chunk_size_vp(type, value_properties(type));
}

/*inline constexpr static unsigned
value_chunk_size_zero(
	Data::Type const type
) {
	return value_properties(type).chunk_capacity * value_zero_size(type);
}*/

inline static unsigned
value_data_size(
	Data::ValueRef const& value
) {
	DUCT_DEBUG_ASSERTE(value.type.type() != Data::ValueType::dynamic);
	if (value_properties(value.type).flags & VTP_DYNAMIC_SIZE) {
		return value.size;
	} else {
		return data_size_fixed(value.type.size());
	}
}

static void
value_morph(
	Data::ValueRef& value,
	Data::Type const type
) {
	if (type == value.type || type.type() == Data::ValueType::dynamic) {
		return;
	} else if (type.type() != value.type.type()) {
		value.data.u64 = 0;
	} else {
		if (value_properties(type).flags & VTP_DYNAMIC_SIZE) {
			switch (data_size_dynamic(type.size())) {
			case 1: value.size = min_ce(value.size, static_cast<unsigned>(~std::uint8_t {0})); break;
			case 2: value.size = min_ce(value.size, static_cast<unsigned>(~std::uint16_t{0})); break;
			case 4: value.size = min_ce(value.size, ~unsigned{0}); break;
			}
		} else if (type.type() == Data::ValueType::integer) {
			if (enum_bitand(type.flags(), ValueFlag::integer_signed)) {
				std::int64_t x;
				switch (value.type.size()) {
				case Data::Size::b8 : x = value.data.s8 ; break;
				case Data::Size::b16: x = value.data.s16; break;
				case Data::Size::b32: x = value.data.s32; break;
				case Data::Size::b64: x = value.data.s64; break;
				}
				x = value_clamp_signed(x, type.size());
				switch (type.size()) {
				case Data::Size::b8 : x = value.data.s8  = static_cast<std::int8_t >(x); break;
				case Data::Size::b16: x = value.data.s16 = static_cast<std::int16_t>(x); break;
				case Data::Size::b32: x = value.data.s32 = static_cast<std::int32_t>(x); break;
				case Data::Size::b64: x = value.data.s64 = x; break;
				}
			} else {
				std::uint64_t x;
				switch (value.type.size()) {
				case Data::Size::b8 : x = value.data.u8 ; break;
				case Data::Size::b16: x = value.data.u16; break;
				case Data::Size::b32: x = value.data.u32; break;
				case Data::Size::b64: x = value.data.u64; break;
				}
				x = value_clamp_unsigned(x, type.size());
				switch (type.size()) {
				case Data::Size::b8 : x = value.data.u8  = static_cast<std::uint8_t >(x); break;
				case Data::Size::b16: x = value.data.u16 = static_cast<std::uint16_t>(x); break;
				case Data::Size::b32: x = value.data.u32 = static_cast<std::uint32_t>(x); break;
				case Data::Size::b64: x = value.data.u64 = x; break;
				}
			}
		}
	}
	value.type = type;
}

inline static unsigned
value_written_size(
	Data::ValueRef const& value,
	bool const write_type
) {
	DUCT_DEBUG_ASSERTE(value.type.type() != Data::ValueType::dynamic);
	unsigned meta_size = write_type ? sizeof(Data::TypeValue) : 0;
	if (value_properties(value.type).flags & VTP_DYNAMIC_SIZE) {
		meta_size += data_size_dynamic(value.type.size());
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
	if (value_properties(type).flags & VTP_DYNAMIC_SIZE) {
		switch (data_size_dynamic(type.size())) {
		case 1: return *reinterpret_cast<std::uint8_t const*>(data);
		case 2: return *reinterpret_cast<std::uint16_t const*>(data);
		case 4: return *reinterpret_cast<std::uint32_t const*>(data);
		default: return 0;
		}
	} else {
		return data_size_fixed(type.size());
	}
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
	if (value_properties(type).flags & VTP_DYNAMIC_SIZE) {
		switch (data_size_dynamic(type.size())) {
		case 1: value.size = *reinterpret_cast<std::uint8_t const*>(data); break;
		case 2: value.size = *reinterpret_cast<std::uint16_t const*>(data); break;
		case 4: value.size = *reinterpret_cast<std::uint32_t const*>(data); break;
		}
		data += data_size_dynamic(type.size());
		value.data.dynamic = data;
	} else {
		switch (data_size_fixed(type.size())) {
		case 1: value.data.u8 = *reinterpret_cast<std::uint8_t const*>(data); break;
		case 2: value.data.u16 = *reinterpret_cast<std::uint16_t const*>(data); break;
		case 4: value.data.u32 = *reinterpret_cast<std::uint32_t const*>(data); break;
		case 8: value.data.u64 = *reinterpret_cast<std::uint64_t const*>(data); break;
		}
	}
	return value;
}

static void
value_write(
	Data::ValueRef const& value,
	std::uint8_t* output,
	bool const write_type
) {
	DUCT_DEBUG_ASSERTE(value.type.type() != Data::ValueType::dynamic);
	if (write_type) {
		*reinterpret_cast<Data::TypeValue*>(output) = value.type.value();
		output += sizeof(Data::TypeValue);
	}
	if (value_properties(value.type).flags & VTP_DYNAMIC_SIZE) {
		switch (data_size_dynamic(value.type.size())) {
		case 1: *reinterpret_cast<std::uint8_t*>(output) = min_ce(value.size, static_cast<unsigned>(~std::uint8_t{0})); break;
		case 2: *reinterpret_cast<std::uint16_t*>(output) = min_ce(value.size, static_cast<unsigned>(~std::uint16_t{0})); break;
		case 4: *reinterpret_cast<std::uint32_t*>(output) = min_ce(value.size, static_cast<unsigned>(~std::uint32_t{0})); break;
		}
		output += data_size_dynamic(value.type.size());
		std::memcpy(output, value.data.dynamic, value.size);
	} else {
		switch (data_size_fixed(value.type.size())) {
		case 1: *reinterpret_cast<std::uint8_t*>(output) = value.data.u8; break;
		case 2: *reinterpret_cast<std::uint16_t*>(output) = value.data.u16; break;
		case 4: *reinterpret_cast<std::uint32_t*>(output) = value.data.u32; break;
		case 8: *reinterpret_cast<std::uint64_t*>(output) = value.data.u64; break;
		}
	}
}

inline static unsigned
chunk_next_value(
	Data::ValueStore::Chunk const& chunk,
	Data::Type const type,
	unsigned const offset
) noexcept {
	DUCT_DEBUG_ASSERTE(chunk.size > offset);
	return offset + value_read_size(type, chunk.data + offset);
}

inline static void
chunk_zero(
	Data::ValueStore::Chunk& chunk
) noexcept {
	std::memset(chunk.head, 0, chunk.space_tail());
}

inline static void
chunk_clear(
	Data::ValueStore::Chunk& chunk
) noexcept {
	chunk.head = chunk.data;
	chunk.tail = chunk.data;
	chunk.num_values = 0;
}

static void
chunk_free(
	Data::ValueStore::Chunk& chunk
) noexcept {
	if (chunk.data) {
		delete[] chunk.data;
	}
	chunk.data = nullptr;
	chunk.head = nullptr;
	chunk.tail = nullptr;
	chunk.size = 0;
	chunk.num_values = 0;
}

static void
chunk_allocate(
	Data::ValueStore::Chunk& chunk,
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
	Data::ValueStore::Chunk& chunk,
	Data::ValueStore::Chunk& split,
	unsigned const min_size,
	unsigned const num_values,
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
	split.num_values = num_values;
	chunk.num_values -= num_values;
}

static void
chunk_split_head(
	Data::ValueStore::Chunk& chunk,
	Data::ValueStore::Chunk& split,
	Data::ValueStore::Iterator& it,
	unsigned const min_size,
	unsigned const tail_space
) {
	chunk_split(
		chunk, split,
		min_size, it.inner_index,
		chunk.offset_head(), it.value_offset,
		0, tail_space
	);
	it.value_offset = split.offset_tail();
}

static void
chunk_split_tail(
	Data::ValueStore::Chunk& chunk,
	Data::ValueStore::Chunk& split,
	Data::ValueStore::Iterator& it,
	unsigned const min_size,
	unsigned const head_space
) {
	chunk_split(
		chunk, split,
		min_size, chunk.num_values - it.inner_index,
		it.value_offset, chunk.offset_tail(),
		head_space, 0
	);
	++it.chunk_index;
	it.inner_index = 0;
	it.value_offset = split.offset_head();
}

inline static void
chunk_set_bounds(
	Data::ValueStore::Chunk& chunk,
	unsigned const num_values,
	unsigned const head,
	unsigned const tail
) {
	DUCT_ASSERTE(
		head < tail &&
		tail <= chunk.size &&
		chunk.size >= (tail - head)
	);
	chunk.num_values = num_values;
	chunk.head = chunk.data + head;
	chunk.tail = chunk.data + tail;
}

static bool
segment_resize(
	Data::ValueStore::Chunk& chunk,
	Data::ValueStore::Chunk& split,
	Data::ValueStore::Iterator& it,
	unsigned const chunk_size,
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
	unsigned const from_head = it.value_offset - chunk.offset_head();
	unsigned const from_tail = chunk.offset_tail() - it.value_offset;
	unsigned const diff
		= old_size > new_size
		? old_size - new_size
		: new_size - old_size
	;
	if (old_size > new_size) {
		if (from_head < from_tail) {
			std::memmove(chunk.head + diff, chunk.head, from_head);
			chunk.head += diff;
			it.value_offset += diff;
		} else {
			std::memmove(chunk.head + from_head + new_size, chunk.head + from_head + old_size, from_tail - old_size);
			chunk.tail -= diff;
		}
	} else if (chunk.space_head() >= new_size) {
		std::memmove(chunk.head - diff, chunk.head, from_head);
		chunk.head -= diff;
		it.value_offset -= diff;
	} else if (chunk.space_tail() >= new_size) {
		std::memmove(chunk.head + from_head + new_size, chunk.head + from_head + old_size, from_tail - old_size);
		chunk.tail += diff;
	} else if (from_head < from_tail) {
		chunk_split_head(chunk, split, it, chunk_size, new_size);
		split.tail += new_size;
		return true;
	} else {
		chunk_split_tail(chunk, split, it, chunk_size, new_size);
		split.head -= new_size;
		it.value_offset -= new_size;
		return true;
	}
	return false;
}

} // anonymous namespace

// class ValueStore::Iterator implementation

ValueStore::Iterator&
ValueStore::Iterator::operator++() noexcept {
	++index;
	if (index < value_store->m_num_values) {
		auto const* chunk = &value_store->m_chunks[chunk_index];
		++inner_index;
		if (inner_index < chunk->num_values) {
			value_offset = chunk_next_value(*chunk, value_store->type(), value_offset);
		} else {
			++chunk_index;
			inner_index = 0;
			DUCT_ASSERTE(value_store->m_chunks.size() > chunk_index);
			chunk = &value_store->m_chunks[chunk_index];
			value_offset = chunk->offset_head();
		}
	}
	return *this;
}

ValueStore::Iterator&
ValueStore::Iterator::operator+=(
	unsigned count
) noexcept {
	index += count;
	if (index < value_store->m_num_values) {
		auto const* chunk = &value_store->m_chunks[chunk_index];
		while (inner_index + count > chunk->num_values) {
			count -= chunk->num_values - inner_index;
			++chunk_index;
			inner_index = 0;
			chunk = &value_store->m_chunks[chunk_index];
			value_offset = chunk->offset_head();
		}
		while (count) {
			value_offset = chunk_next_value(*chunk, value_store->type(), value_offset);
		}
	} else {
		index = value_store->m_num_values;
		chunk_index = value_store->m_chunks.size();
		inner_index = 0;
	}
	return *this;
}

// class ValueStore implementation

#define HORD_SCOPE_CLASS ValueStore

ValueStore::~ValueStore() noexcept {
	free();
}

ValueStore::ValueStore(
	Data::Type const type
) noexcept {
	set_type(type);
}

#define HORD_SCOPE_FUNC set_type
bool
ValueStore::set_type(
	Data::Type const type
) noexcept {
	if (type == m_type) {
		return false;
	} else if (m_num_values == 0) {
		m_type = type;
		return true;
	} else if (type.type() == Data::ValueType::null) {
		free();
		m_type = type;
		return true;
	}

	unsigned const zero_size = value_zero_size(type);
	unsigned const alloc_size = value_chunk_size(type);
	unsigned const chunk_capacity = value_chunk_capacity(type);
	unsigned num_remaining = m_num_values;
	unsigned num_take;
	auto it = m_chunks.begin();
	for (; it != m_chunks.end();) {
		auto& chunk = *it;
		if (chunk.size < alloc_size) {
			chunk_allocate(chunk, alloc_size);
		}
		num_take = min_ce(num_remaining, chunk_capacity);
		num_remaining -= num_take;
		chunk_set_bounds(chunk, num_take, 0, zero_size * num_take);
		chunk_zero(chunk);
		++it;
		if (num_remaining == 0) {
			break;
		} else if (it == m_chunks.end()) {
			it = m_chunks.insert(it, Data::ValueStore::Chunk{});
		}
	}
	for (auto it_free = it; it_free != m_chunks.end(); ++it_free) {
		chunk_free(*it_free);
	}
	m_chunks.erase(it, m_chunks.end());
	m_type = type;
	return true;
	
}
#undef HORD_SCOPE_FUNC

void
ValueStore::zero() noexcept {
	if (m_num_values == 0) {
		return;
	}
	for (auto& chunk : m_chunks) {
		chunk_zero(chunk);
	}
}

void
ValueStore::clear() noexcept {
	for (auto& chunk : m_chunks) {
		chunk_clear(chunk);
	}
	m_num_values = 0;
}

void
ValueStore::free() noexcept {
	for (auto& chunk : m_chunks) {
		chunk_free(chunk);
	}
	m_chunks.clear();
	m_num_values = 0;
}

void
ValueStore::resize(
	unsigned const size
) noexcept {
	if (m_num_values == size) {
		return;
	} else if (size == 0) {
		clear();
	}

	m_num_values = 0;
	auto it_chunk = m_chunks.begin();
	for (; it_chunk != m_chunks.end(); ++it_chunk) {
		m_num_values += it_chunk->num_values;
		if (m_num_values > size) {
			break;
		}
	}

	if (m_num_values < size) {
		unsigned const zero_size = value_zero_size(m_type);
		unsigned const alloc_size = value_chunk_size(m_type);
		unsigned const chunk_capacity = value_chunk_capacity(m_type);
		while (m_num_values < size) {
			unsigned const num_fill = min_ce(size - m_num_values, chunk_capacity);
			Data::ValueStore::Chunk chunk{};
			chunk_allocate(chunk, alloc_size);
			chunk_set_bounds(chunk, num_fill, 0, num_fill * zero_size);
			chunk_zero(chunk);
			m_chunks.push_back(chunk);
			m_num_values += num_fill;
		}
	} else if (m_num_values > size) {
		m_num_values -= it_chunk->num_values;
		++it_chunk;
		for (auto it_free = it_chunk; it_free != m_chunks.end(); ++it_free) {
			chunk_free(*it_free);
		}
		m_chunks.erase(it_chunk, m_chunks.end());
		auto& chunk = m_chunks.back();
		Data::ValueStore::Iterator it{
			this,
			m_num_values,
			static_cast<unsigned>(m_chunks.size() - 1),
			0,
			chunk.offset_head()
		};
		while (m_num_values < size) {
			++it;
			++m_num_values;
		}
		chunk_set_bounds(chunk, it.index, chunk.offset_head(), it.value_offset);
	}
}

void
ValueStore::set_value(
	Data::ValueStore::Iterator& it,
	Data::ValueRef& new_value,
	bool const insert
) {
	if (m_type.type() == Data::ValueType::null) {
		return;
	}
	bool const is_dynamic = Data::ValueType::dynamic == m_type.type();
	DUCT_ASSERTE(
		it.value_store == this &&
		(it.index < m_num_values || insert)
	);
	value_morph(new_value, m_type);

	// TODO: Handle null values
	unsigned const chunk_size = value_chunk_size(m_type);
	unsigned const old_size = insert ? 0 : value_written_size(get_value(it), is_dynamic);
	unsigned const new_size = value_written_size(new_value, is_dynamic);
	if (m_chunks.empty()) {
		Data::ValueStore::Chunk chunk{};
		chunk_allocate(chunk, max_ce(chunk_size, new_size));
		m_chunks.push_back(chunk);
		it = begin();
	}
	auto* chunk = &m_chunks[it.chunk_index];
	Data::ValueStore::Chunk split{};
	if (segment_resize(*chunk, split, it, chunk_size, old_size, new_size)) {
		chunk = &(*m_chunks.insert(m_chunks.cbegin() + it.chunk_index, split));
	}
	value_write(new_value, chunk->data + it.value_offset, is_dynamic);
	if (insert) {
		++chunk->num_values;
		++m_num_values;
	}
}

void
ValueStore::remove(
	Data::ValueStore::Iterator& it
) {
	if (m_type.type() == Data::ValueType::null) {
		return;
	}
	DUCT_ASSERTE(
		it.value_store == this &&
		it.index < m_num_values
	);

	bool const is_dynamic = Data::ValueType::dynamic == m_type.type();
	unsigned const size = value_written_size(get_value(it), is_dynamic);
	auto& chunk = m_chunks[it.chunk_index];
	Data::ValueStore::Chunk split_unused{};
	segment_resize(chunk, split_unused, it, 0, size, 0);
	--chunk.num_values;
	--m_num_values;
}

Data::ValueRef
ValueStore::get_value(
	Data::ValueStore::Iterator const& it
) noexcept {
	if (!it.can_advance() || m_type.type() == Data::ValueType::null) {
		return {};
	}
	auto const& chunk = m_chunks[it.chunk_index];
	return value_read(m_type, chunk.data + it.value_offset);
}

// TODO: Format versioning
#define HORD_SCOPE_FUNC read
ser_result_type
ValueStore::read(
	ser_tag_read,
	InputSerializer& ser
) {
	free();
	std::uint32_t format_version = 0;
	ser(format_version);
	ser(m_type);

	std::uint32_t num_chunks = 0;
	ser(num_chunks);

	unsigned const alloc_size = value_chunk_size(m_type);
	std::uint32_t num_values = 0;
	std::uint32_t data_size = 0;
	for (; num_chunks > 0; --num_chunks) {
		Data::ValueStore::Chunk chunk{};
		ser(num_values, data_size);
		chunk_allocate(chunk, max_ce(data_size, alloc_size));
		chunk_set_bounds(chunk, num_values, 0, data_size);
		ser(Cacophony::make_binary_blob(chunk.head, data_size));
		m_num_values += chunk.num_values;
		m_chunks.push_back(chunk);
	}
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC write
ser_result_type
ValueStore::write(
	ser_tag_write,
	OutputSerializer& ser
) const {
	std::uint32_t const format_version = 0;
	ser(format_version);
	ser(m_type);

	ser(static_cast<std::uint32_t>(m_chunks.size()));
	std::uint32_t num_values;
	std::uint32_t data_size;
	for (auto const& chunk : m_chunks) {
		num_values = static_cast<std::uint32_t>(chunk.num_values);
		data_size = static_cast<std::uint32_t>(chunk.space_used());
		ser(num_values, data_size);
		ser(Cacophony::make_binary_blob(chunk.head, data_size));
	}
}
#undef HORD_SCOPE_FUNC

#undef HORD_SCOPE_CLASS // ValueStore

} // namespace Data
} // namespace Hord
