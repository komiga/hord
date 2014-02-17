
#include <Hord/detail/gr_ceformat.hpp>
#include <Hord/String.hpp>
#include <Hord/Object/Defs.hpp>
#include <Hord/Node/Unit.hpp>
#include <Hord/IO/Defs.hpp>
#include <Hord/IO/PropStream.hpp>

#include <duct/EndianUtils.hpp>
#include <duct/IO/arithmetic.hpp>
#include <duct/IO/unicode.hpp>

#include <cassert>
#include <limits>

namespace Hord {
namespace Node {

// class Unit implementation

#define HORD_SCOPE_CLASS Node::Unit

namespace {
static constexpr Object::type_info const
s_type_info{
	Object::Type::Node,
	true,
	true
};
} // anonymous namespace

Object::type_info const&
Unit::get_type_info_impl() const noexcept {
	return s_type_info;
}

Unit::Unit(
	Node::ID const id,
	Object::ID const parent
) noexcept
	: base(
		s_type_info.supplies_prop_primary,
		s_type_info.supplies_prop_auxiliary,
		static_cast<Object::ID>(id),
		parent
	)
{}

Unit::Unit(Unit&&) = default;
Unit::~Unit() noexcept = default;

Unit& Unit::operator=(Unit&&) = default;

void
Unit::set_layout_ref(
	Node::ID const node_id
) noexcept {
	m_layout_ref = node_id;
	if (Node::NULL_ID != m_layout_ref) {
		m_layout.clear();
	}
}

// serialization

#define HORD_NODE_CHECK_IO_ERROR__(err__)						\
	if (stream.fail()) {										\
		HORD_THROW_FMT(											\
			ErrorCode::serialization_io_failed,					\
			err__,												\
			get_id()											\
		);														\
	}
//

// - primary

/*
Avoiding murk for extra speed. Note that a serialized value is used
for the field count instead of relying on m_layout, whose prop might
be uninitialized.

Field structure:

	uint8 type
	<{uint16 count, char str[count]} | int64 | uint8> value

Record structure:

	Field fields[field_count]

Prop structure:

	uint32 field_count
	uint32 record_count
	Record records[record_count]
*/

// Assumption made about String (which always should be true for Hord)
static_assert(
	1u == sizeof(String::value_type),
	"serialization form assumes String has 1-byte code units"
);

#define HORD_SCOPE_FUNC deserialize_prop_primary
namespace {
HORD_DEF_FMT_FQN(
	s_err_primary_read_failed,
	"failed to deserialize prop %08x -> primary: read error"
);
HORD_DEF_FMT_FQN(
	s_err_primary_malformed_field_type,
	"failed to deserialize prop %08x -> primary:"
	" invalid field type encountered"
);
} // anonymous namespace

void
Unit::deserialize_prop_primary(
	IO::InputPropStream& prop_stream
) {
	std::istream& stream = prop_stream.get_stream();

	std::size_t const field_count
	= duct::IO::read_arithmetic<uint32_t>(
		stream,
		duct::Endian::little
	);
	HORD_NODE_CHECK_IO_ERROR__(s_err_primary_read_failed);

	if (0u < field_count) {
		std::size_t const record_count
		= duct::IO::read_arithmetic<uint32_t>(
			stream,
			duct::Endian::little
		);
		HORD_NODE_CHECK_IO_ERROR__(s_err_primary_read_failed);

		// records
		record_vector_type des_records{record_count};
		uint8_t field_type = 0x00u;
		std::size_t field_string_size = 0u;
		for (
			auto record_it = des_records.begin();
			des_records.end() != record_it;
			++record_it
		) {
			record_it->fields.resize(field_count);

		// fields
		for (
			auto field_it = record_it->fields.begin();
			record_it->fields.end() != field_it;
			++field_it
		) {
			duct::IO::read_arithmetic(
				stream,
				field_type,
				duct::Endian::little
			);
			HORD_NODE_CHECK_IO_ERROR__(s_err_primary_read_failed);

			switch (static_cast<Data::FieldType>(field_type)) {
			case Data::FieldType::Text: {
				field_string_size
				= duct::IO::read_arithmetic<uint16_t>(
					stream,
					duct::Endian::little
				);
				HORD_NODE_CHECK_IO_ERROR__(s_err_primary_read_failed);

				field_it->value.str.reserve(field_string_size);
				duct::IO::read_string_copy(
					stream,
					field_it->value.str,
					field_string_size,
					duct::Endian::system
				);
			} break;

			case Data::FieldType::Number: {
				duct::IO::read_arithmetic(
					stream,
					field_it->value.num,
					duct::Endian::little
				);
			} break;

			case Data::FieldType::Boolean: {
				field_it->value.bin
				= static_cast<bool>(duct::IO::read_arithmetic<uint8_t>(
					stream,
					duct::Endian::little
				));
			} break;

			default:
				HORD_THROW_FMT(
					ErrorCode::serialization_data_malformed,
					s_err_primary_malformed_field_type,
					get_id()
				);
				break;
			}

			// For last IO operation in switch case
			HORD_NODE_CHECK_IO_ERROR__(s_err_primary_read_failed);
		}} // for for

		// commit
		m_records.operator=(std::move(des_records));
	} else {
		// no records
		m_records.clear();
	}
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC serialize_prop_primary
namespace {
HORD_DEF_FMT_FQN(
	s_err_primary_write_failed,
	"failed to serialize prop %08x -> primary: write error"
);
} // anonymous namespace

void
Unit::serialize_prop_primary(
	IO::OutputPropStream& prop_stream
) const {
	std::ostream& stream = prop_stream.get_stream();

	// field_count
	duct::IO::write_arithmetic(
		stream,
		static_cast<uint32_t>(m_layout.size()),
		duct::Endian::little
	);
	HORD_NODE_CHECK_IO_ERROR__(s_err_primary_write_failed);

	// record_count
	duct::IO::write_arithmetic(
		stream,
		static_cast<uint32_t>(m_records.size()),
		duct::Endian::little
	);
	HORD_NODE_CHECK_IO_ERROR__(s_err_primary_write_failed);

	// Quick exit
	if (0u == m_layout.size() || 0u == m_records.size()) {
		return;
	}

	// records
	for (
		auto record_it = m_records.cbegin();
		m_records.cend() != record_it;
		++record_it
	) {
	// fields
	for (
		auto field_it = record_it->fields.cbegin();
		record_it->fields.cend() != field_it;
		++field_it
	) {
		// field type
		duct::IO::write_arithmetic<uint8_t>(
			stream,
			static_cast<uint8_t>(field_it->type),
			duct::Endian::little
		);
		HORD_NODE_CHECK_IO_ERROR__(s_err_primary_write_failed);

		switch (field_it->type) {
		case Data::FieldType::Text:
			assert(
				std::numeric_limits<uint16_t>::max()
				>= field_it->value.str.size()
			);
			duct::IO::write_arithmetic(
				stream,
				static_cast<uint16_t>(field_it->value.str.size()),
				duct::Endian::little
			);
			HORD_NODE_CHECK_IO_ERROR__(s_err_primary_write_failed);

			if (0u < field_it->value.str.size()) {
				duct::IO::write(
					stream,
					field_it->value.str.data(),
					field_it->value.str.size()
				);
			} else {
				// Skip redundant IO check after switch
				continue;
			}
		break;

		case Data::FieldType::Number:
			duct::IO::write_arithmetic(
				stream,
				field_it->value.num,
				duct::Endian::little
			);
		break;

		case Data::FieldType::Boolean:
			duct::IO::write_arithmetic(
				stream,
				static_cast<uint8_t>(field_it->value.bin),
				duct::Endian::little
			);
		break;
		}

		// For last IO operation in switch case
		HORD_NODE_CHECK_IO_ERROR__(s_err_primary_write_failed);
	}} // for for
}
#undef HORD_SCOPE_FUNC


// - auxiliary

#define HORD_SCOPE_FUNC deserialize_prop_auxiliary
namespace {
HORD_DEF_FMT_FQN(
	s_err_auxiliary_read_failed,
	"failed to serialize prop %08x -> auxiliary: read error"
);
} // anonymous namespace

void
Unit::deserialize_prop_auxiliary(
	IO::InputPropStream& prop_stream
) {
	std::istream& stream = prop_stream.get_stream();

	Node::ID const des_layout_ref
	= duct::IO::read_arithmetic<Node::ID>(
		stream,
		duct::Endian::little
	);
	HORD_NODE_CHECK_IO_ERROR__(s_err_auxiliary_read_failed);

	if (Node::NULL_ID != des_layout_ref) {
		// commit
		set_layout_ref(des_layout_ref);
	} else {
		std::size_t const column_count
		= duct::IO::read_arithmetic<uint32_t>(
			stream,
			duct::Endian::little
		);
		HORD_NODE_CHECK_IO_ERROR__(s_err_auxiliary_read_failed);

		column_vector_type des_layout{column_count};
		for (
			auto it = des_layout.begin();
			des_layout.end() != it;
			++it
		) {
			// Will throw if a read error occurs
			it->deserialize(stream);
		}

		// commit
		set_layout_ref(Node::NULL_ID);
		m_layout.operator=(std::move(des_layout));
	}
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC serialize_prop_auxiliary
namespace {
HORD_DEF_FMT_FQN(
	s_err_auxiliary_write_failed,
	"failed to serialize prop %08x -> auxiliary: write error"
);
} // anonymous namespace

void
Unit::serialize_prop_auxiliary(
	IO::OutputPropStream& prop_stream
) const {
	std::ostream& stream = prop_stream.get_stream();

	duct::IO::write_arithmetic<Node::ID>(
		stream,
		m_layout_ref,
		duct::Endian::little
	);
	HORD_NODE_CHECK_IO_ERROR__(s_err_auxiliary_write_failed);

	if (Node::NULL_ID == m_layout_ref) {
		duct::IO::write_arithmetic<uint32_t>(
			stream,
			static_cast<uint32_t>(m_layout.size()),
			duct::Endian::little
		);
		HORD_NODE_CHECK_IO_ERROR__(s_err_auxiliary_write_failed);

		for (
			auto it = m_layout.cbegin();
			m_layout.cend() != it;
			++it
		) {
			// Will throw if a write error occurs
			it->serialize(stream);
		}
	}
}
#undef HORD_SCOPE_FUNC

// - impl

#define HORD_SCOPE_FUNC deserialize_impl
void
Unit::deserialize_impl(
	IO::InputPropStream& prop_stream
) {
	switch (prop_stream.get_type()) {
	case IO::PropType::primary:
		deserialize_prop_primary(prop_stream);
		break;

	case IO::PropType::auxiliary:
		deserialize_prop_auxiliary(prop_stream);
		break;

	default:
		// Object::Unit should protect us from this
		assert(false);
	}
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC serialize_impl
void
Unit::serialize_impl(
	IO::OutputPropStream& prop_stream
) const {
	switch (prop_stream.get_type()) {
	case IO::PropType::primary:
		serialize_prop_primary(prop_stream);
		break;

	case IO::PropType::auxiliary:
		serialize_prop_auxiliary(prop_stream);
		break;

	default:
		// Object::Unit should protect us from this
		assert(false);
	}
}
#undef HORD_SCOPE_FUNC

#undef HORD_SCOPE_CLASS

} // namespace Node
} // namespace Hord
