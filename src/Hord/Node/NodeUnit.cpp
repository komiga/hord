
#include <Hord/String.hpp>
#include <Hord/serialization.hpp>
#include <Hord/Object/Defs.hpp>
#include <Hord/Node/Unit.hpp>
#include <Hord/IO/Defs.hpp>
#include <Hord/IO/PropStream.hpp>

#include <cassert>
#include <limits>

#include <Hord/detail/gr_ceformat.hpp>

namespace Hord {
namespace Node {

// class Unit implementation

#define HORD_SCOPE_CLASS Node::Unit

Unit::~Unit() noexcept = default;

Unit::Unit(Unit&&) = default;
Unit& Unit::operator=(Unit&&) = default;

Unit::Unit(
	Object::type_info const& tinfo,
	Node::ID const id,
	Object::ID const parent
) noexcept
	: base(
		tinfo,
		static_cast<Object::ID>(id),
		parent
	)
{}

void
Unit::set_layout_ref(
	Node::ID const node_id
) noexcept {
	m_layout_ref = node_id;
	if (Node::ID_NULL != m_layout_ref) {
		m_layout.clear();
	}
}

// serialization

/*
Note that a serialized value is used for the field count instead of
relying on m_layout, whose prop might be uninitialized.

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
	s_err_primary_malformed_field_type,
	"failed to deserialize prop %08x -> primary:"
	" invalid field type encountered"
);
} // anonymous namespace

void
Unit::deserialize_prop_primary(
	IO::InputPropStream& /*prop_stream*/,
	InputSerializer& ser
) {
	std::uint32_t field_count = 0;
	ser(field_count);

	if (0u < field_count) {
		std::uint32_t record_count = 0;
		ser(record_count);

		record_vector_type des_records{
			record_count,
			Data::Record{field_count}
		};
		std::uint8_t field_type = 0x00u;
		for (auto& record : des_records) {
			for (auto& field : record.fields) {
				ser(field_type);
				switch (static_cast<Data::FieldType>(field_type)) {
				case Data::FieldType::Text:
					ser(Cacophony::make_string_cfg<std::uint16_t>(
						field.value.str
					));
					break;

				case Data::FieldType::Number:
					ser(field.value.num);
					break;

				case Data::FieldType::Boolean: {
					std::uint8_t boolu8 = 0;
					ser(boolu8);
					field.value.bin = static_cast<bool>(boolu8);
				} break;

				default:
					HORD_THROW_FMT(
						ErrorCode::serialization_data_malformed,
						s_err_primary_malformed_field_type,
						get_id().value()
					);
					break;
				}
				field.value.type = static_cast<Data::FieldType>(field_type);
			}
		}

		// commit
		m_records.operator=(std::move(des_records));
	} else {
		// no records
		m_records.clear();
	}
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC serialize_prop_primary
void
Unit::serialize_prop_primary(
	IO::OutputPropStream& /*prop_stream*/,
	OutputSerializer& ser
) const {
	assert(std::numeric_limits<std::uint32_t>::max() >= m_layout.size());
	ser(static_cast<std::uint32_t>(m_layout.size()));
	assert(std::numeric_limits<std::uint32_t>::max() >= m_records.size());
	ser(static_cast<std::uint32_t>(m_records.size()));

	// Quick exit
	if (0u == m_layout.size() || 0u == m_records.size()) {
		return;
	}
	for (auto const& record : m_records) {
		for (auto const& field : record.fields) {
			ser(static_cast<std::uint8_t>(field.value.type));
			switch (field.value.type) {
			case Data::FieldType::Text:
				ser(Cacophony::make_string_cfg<std::uint16_t>(
					field.value.str
				));
				break;

			case Data::FieldType::Number:
				ser(field.value.num);
				break;

			case Data::FieldType::Boolean:
				ser(static_cast<std::uint8_t>(field.value.bin));
				break;
			}
		}
	}
}
#undef HORD_SCOPE_FUNC


// - auxiliary

#define HORD_SCOPE_FUNC deserialize_prop_auxiliary
void
Unit::deserialize_prop_auxiliary(
	IO::InputPropStream& /*prop_stream*/,
	InputSerializer& ser
) {
	Node::ID des_layout_ref{Node::ID_NULL};
	ser(des_layout_ref);
	if (Node::ID_NULL != des_layout_ref) {
		// commit
		set_layout_ref(des_layout_ref);
	} else {
		column_vector_type des_layout{};
		ser(Cacophony::make_vector_cfg<std::uint32_t>(des_layout));

		// commit
		set_layout_ref(Node::ID_NULL);
		m_layout.operator=(std::move(des_layout));
	}
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC serialize_prop_auxiliary
void
Unit::serialize_prop_auxiliary(
	IO::OutputPropStream& /*prop_stream*/,
	OutputSerializer& ser
) const {
	if (Node::ID_NULL != m_layout_ref) {
		ser(m_layout_ref);
	} else {
		ser(
			m_layout_ref,
			Cacophony::make_vector_cfg<std::uint32_t>(m_layout)
		);
	}
}
#undef HORD_SCOPE_FUNC

// - impl

#define HORD_SCOPE_FUNC deserialize_impl
namespace {
HORD_DEF_FMT_FQN(
	s_err_read_failed,
	HORD_SER_ERR_MSG_IO_PROP("read")
);
} // anonymous namespace

void
Unit::deserialize_impl(
	IO::InputPropStream& prop_stream
) try {
	auto ser = prop_stream.make_serializer();
	switch (prop_stream.get_type()) {
	case IO::PropType::primary:
		deserialize_prop_primary(prop_stream, ser);
		break;

	case IO::PropType::auxiliary:
		deserialize_prop_auxiliary(prop_stream, ser);
		break;

	default:
		// Object::Unit should protect us from this
		assert(false);
	}
} catch (SerializerError& serr) {
	HORD_THROW_SER_PROP(
		s_err_read_failed,
		serr,
		get_id(),
		IO::get_prop_type_name(prop_stream.get_type())
	);
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC serialize_impl
namespace {
HORD_DEF_FMT_FQN(
	s_err_write_failed,
	HORD_SER_ERR_MSG_IO_PROP("write")
);
} // anonymous namespace

void
Unit::serialize_impl(
	IO::OutputPropStream& prop_stream
) const try {
	auto ser = prop_stream.make_serializer();
	switch (prop_stream.get_type()) {
	case IO::PropType::primary:
		serialize_prop_primary(prop_stream, ser);
		break;

	case IO::PropType::auxiliary:
		serialize_prop_auxiliary(prop_stream, ser);
		break;

	default:
		// Object::Unit should protect us from this
		assert(false);
	}
} catch (SerializerError& serr) {
	HORD_THROW_SER_PROP(
		s_err_write_failed,
		serr,
		get_id(),
		IO::get_prop_type_name(prop_stream.get_type())
	);
}
#undef HORD_SCOPE_FUNC

#undef HORD_SCOPE_CLASS

} // namespace Node
} // namespace Hord
