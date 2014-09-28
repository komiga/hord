
#include <Hord/serialization.hpp>
#include <Hord/utility.hpp>
#include <Hord/String.hpp>
#include <Hord/Data/Metadata.hpp>
#include <Hord/IO/Defs.hpp>

#include <cassert>
#include <type_traits>
#include <new>
#include <limits>

#include <Hord/detail/gr_ceformat.hpp>

namespace Hord {
namespace Data {

// class Metadata implementation

#define HORD_SCOPE_CLASS Metadata

#define HORD_SCOPE_FUNC deserialize
namespace {
HORD_DEF_FMT_FQN(
	s_err_read_failed,
	HORD_SER_ERR_MSG_IO_PROP("read")
);
} // anonymous namespace

void
Metadata::deserialize(
	IO::InputPropStream& prop_stream
) try {
	assert(IO::PropType::metadata == prop_stream.get_type());
	auto ser = prop_stream.make_serializer();
	std::uint32_t size = 0;
	ser(size);

	// fields
	field_vector_type des_fields{static_cast<std::size_t>(size)};
	for (auto& field : des_fields) {
		ser(field);
		if (
			enum_cast(Data::FieldType::Text)    > enum_cast(field.value.type) ||
			enum_cast(Data::FieldType::Boolean) < enum_cast(field.value.type)
		) {
			HORD_THROW_FQN(
				ErrorCode::serialization_data_malformed,
				"invalid field type encountered"
			);
		}
	}

	// commit
	this->fields.operator=(std::move(des_fields));
} catch (SerializerError& serr) {
	HORD_THROW_SER_PROP(
		s_err_read_failed,
		serr,
		prop_stream.get_info().object_id,
		"metadata"
	);
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC serialize
namespace {
HORD_DEF_FMT_FQN(
	s_err_write_failed,
	HORD_SER_ERR_MSG_IO_PROP("write")
);
} // anonymous namespace

void
Metadata::serialize(
	IO::OutputPropStream& prop_stream
) const try {
	assert(IO::PropType::metadata == prop_stream.get_type());
	auto ser = prop_stream.make_serializer();
	assert(std::numeric_limits<std::uint32_t>::max() >= this->fields.size());
	ser(static_cast<std::uint32_t>(this->fields.size()));

	// fields
	for (auto const& field : this->fields) {
		ser(field);
	}
} catch (SerializerError& serr) {
	HORD_THROW_SER_PROP(
		s_err_write_failed,
		serr,
		prop_stream.get_info().object_id,
		"metadata"
	);
}
#undef HORD_SCOPE_FUNC

#undef HORD_SCOPE_CLASS // Metadata

} // namespace Data
} // namespace Hord
