
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

// class StringMetaField implementation

namespace {
MetaField*
s_construct_string() noexcept {
	return new (std::nothrow) StringMetaField();
}

static constexpr MetaField::type_info const
s_type_info_string{
	MetaFieldType::String,
	s_construct_string
};
} // anonymous namespace

MetaField::type_info const&
StringMetaField::get_type_info_impl() const noexcept {
	return s_type_info_string;
}

// class Int32MetaField implementation

namespace {
MetaField*
s_construct_int32() noexcept {
	return new (std::nothrow) Int32MetaField();
}

static constexpr MetaField::type_info const
s_type_info_int32{
	MetaFieldType::Int32,
	s_construct_int32
};
} // anonymous namespace

MetaField::type_info const&
Int32MetaField::get_type_info_impl() const noexcept {
	return s_type_info_int32;
}

// class Int64MetaField implementation

namespace {
MetaField*
s_construct_int64() noexcept {
	return new (std::nothrow) Int64MetaField();
}

static constexpr MetaField::type_info const
s_type_info_int64{
	MetaFieldType::Int64,
	s_construct_int64
};
} // anonymous namespace

MetaField::type_info const&
Int64MetaField::get_type_info_impl() const noexcept {
	return s_type_info_int64;
}

// class BoolMetaField implementation

namespace {
MetaField*
s_construct_bool() noexcept {
	return new (std::nothrow) BoolMetaField();
}

static constexpr MetaField::type_info const
s_type_info_bool{
	MetaFieldType::Bool,
	s_construct_bool
};
} // anonymous namespace

MetaField::type_info const&
BoolMetaField::get_type_info_impl() const noexcept {
	return s_type_info_bool;
}

// class Metadata implementation

#define HORD_SCOPE_CLASS Metadata

namespace {
namespace meta_impl {
	static MetaField::type_info const* const
	info[]{
		// Allow direct indexing (MetaFieldType starts at 0x01)
		nullptr,
		&s_type_info_string,
		&s_type_info_int32,
		&s_type_info_int64,
		&s_type_info_bool
	};

	static_assert(
		std::extent<decltype(info)>::value
		== enum_cast(Data::MetaFieldType::LAST),
		"field_info needs to be updated"
	);
}
} // anonymous namespace

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
	uint32_t size = 0;
	ser(size);

	// fields
	field_vector_type des_fields{static_cast<std::size_t>(size)};
	uint8_t field_type = 0x00u;
	for (auto& field : des_fields) {
		ser(field_type);
		if (
			MetaFieldType::String > static_cast<MetaFieldType>(field_type)
		||	MetaFieldType::Bool   < static_cast<MetaFieldType>(field_type)
		) {
			HORD_THROW_FQN(
				ErrorCode::serialization_data_malformed,
				"invalid field type encountered"
			);
		}
		field.reset(meta_impl::info[field_type]->construct());
		assert(static_cast<bool>(field));
		ser(*field);
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
	assert(std::numeric_limits<uint32_t>::max() >= this->fields.size());
	ser(static_cast<uint32_t>(this->fields.size()));

	// fields
	for (auto const& field : this->fields) {
		assert(static_cast<bool>(field));
		ser(
			static_cast<uint8_t>(field->get_type_info().type),
			*field
		);
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
