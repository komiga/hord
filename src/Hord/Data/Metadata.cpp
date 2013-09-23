
#include <Hord/String.hpp>
#include <Hord/Error.hpp>
#include <Hord/Data/Metadata.hpp>
#include <Hord/IO/Defs.hpp>

#include <murk/Desc.hpp>
#include <murk/TieCompound.hpp>
#include <murk/serialize.hpp>

#include <ceformat/print.hpp>

#include <new>

namespace Hord {
namespace Data {

// class MetaField implementation

murk::DescCompound const
MetaField::s_comp_base{
	{murk::StringDesc{0u}},
	{murk::DescType::terminate}
};

MetaField::MetaField() = default;
MetaField::MetaField(MetaField&&) = default;
MetaField::~MetaField() noexcept = default;

MetaField& MetaField::operator=(MetaField&&) = default;

#define MURK_METAFIELD_BIND__	\
	binder						\
		(&this->name)

void
MetaField::bind_base(
	murk::TieBinder& binder
) noexcept {
	MURK_METAFIELD_BIND__;
}

void
MetaField::bind_const_base(
	murk::TieBinder& binder
) const noexcept {
	MURK_METAFIELD_BIND__;
}

#undef MURK_METAFIELD_BIND__

#define MURK_METAFIELD_BIND__	\
	binder						\
		(&this->value)

// class StringMetaField implementation

namespace {
static murk::DescCompound const
s_comp_string{
	{murk::RefDesc{MetaField::s_comp_base}},
	{murk::StringDesc{0u}},
	{murk::DescType::terminate}
};

MetaField*
s_construct_string() noexcept {
	return new (std::nothrow) StringMetaField();
}

static MetaField::type_info const
s_type_info_string{
	MetaFieldType::String,
	{s_comp_string},
	s_construct_string
};
} // anonymous namespace

MetaField::type_info const&
StringMetaField::get_type_info_impl() const noexcept {
	return s_type_info_string;
}

void
StringMetaField::bind_impl(
	murk::TieBinder& binder
) noexcept {
	MURK_METAFIELD_BIND__;
}

void
StringMetaField::bind_const_impl(
	murk::TieBinder& binder
) const noexcept {
	MURK_METAFIELD_BIND__;
}

StringMetaField::StringMetaField() = default;
StringMetaField::StringMetaField(StringMetaField&&) = default;
StringMetaField::~StringMetaField() noexcept = default;

StringMetaField& StringMetaField::operator=(StringMetaField&&) = default;

// class Int32MetaField implementation

namespace {
static murk::DescCompound const
s_comp_int32{
	{murk::RefDesc{MetaField::s_comp_base}},
	{murk::DescType::int32},
	{murk::DescType::terminate}
};

MetaField*
s_construct_int32() noexcept {
	return new (std::nothrow) Int32MetaField();
}

static MetaField::type_info const
s_type_info_int32{
	MetaFieldType::Int32,
	{s_comp_int32},
	s_construct_int32
};
} // anonymous namespace

MetaField::type_info const&
Int32MetaField::get_type_info_impl() const noexcept {
	return s_type_info_int32;
}

void
Int32MetaField::bind_impl(
	murk::TieBinder& binder
) noexcept {
	MURK_METAFIELD_BIND__;
}

void
Int32MetaField::bind_const_impl(
	murk::TieBinder& binder
) const noexcept {
	MURK_METAFIELD_BIND__;
}

Int32MetaField::Int32MetaField() = default;
Int32MetaField::Int32MetaField(Int32MetaField&&) = default;
Int32MetaField::~Int32MetaField() noexcept = default;

Int32MetaField& Int32MetaField::operator=(Int32MetaField&&) = default;

// class Int64MetaField implementation

namespace {
static murk::DescCompound const
s_comp_int64{
	{murk::RefDesc{MetaField::s_comp_base}},
	{murk::DescType::int64},
	{murk::DescType::terminate}
};

MetaField*
s_construct_int64() noexcept {
	return new (std::nothrow) Int64MetaField();
}

static MetaField::type_info const
s_type_info_int64{
	MetaFieldType::Int64,
	{s_comp_int64},
	s_construct_int64
};
} // anonymous namespace

MetaField::type_info const&
Int64MetaField::get_type_info_impl() const noexcept {
	return s_type_info_int64;
}

void
Int64MetaField::bind_impl(
	murk::TieBinder& binder
) noexcept {
	MURK_METAFIELD_BIND__;
}

void
Int64MetaField::bind_const_impl(
	murk::TieBinder& binder
) const noexcept {
	MURK_METAFIELD_BIND__;
}

Int64MetaField::Int64MetaField() = default;
Int64MetaField::Int64MetaField(Int64MetaField&&) = default;
Int64MetaField::~Int64MetaField() noexcept = default;

Int64MetaField& Int64MetaField::operator=(Int64MetaField&&) = default;

// class BoolMetaField implementation

namespace {
static murk::DescCompound const
s_comp_bool{
	{murk::RefDesc{MetaField::s_comp_base}},
	{murk::DescType::boolean},
	{murk::DescType::terminate}
};

MetaField*
s_construct_bool() noexcept {
	return new (std::nothrow) BoolMetaField();
}

static MetaField::type_info const
s_type_info_bool{
	MetaFieldType::Bool,
	{s_comp_bool},
	s_construct_bool
};
} // anonymous namespace

MetaField::type_info const&
BoolMetaField::get_type_info_impl() const noexcept {
	return s_type_info_bool;
}

void
BoolMetaField::bind_impl(
	murk::TieBinder& binder
) noexcept {
	MURK_METAFIELD_BIND__;
}

void
BoolMetaField::bind_const_impl(
	murk::TieBinder& binder
) const noexcept {
	MURK_METAFIELD_BIND__;
}

BoolMetaField::BoolMetaField() = default;
BoolMetaField::BoolMetaField(BoolMetaField&&) = default;
BoolMetaField::~BoolMetaField() noexcept = default;

BoolMetaField& BoolMetaField::operator=(BoolMetaField&&) = default;

// class Metadata implementation

#define HORD_SCOPE_CLASS_IDENT__ Metadata

namespace {
namespace meta_impl {
	static murk::DescCompound const
	comp_header{
		{murk::DescType::uint32}, // size
		{murk::DescType::terminate}
	};
	static murk::TieCompound
	tcomp_header{comp_header};

	static murk::DescCompound const
	comp_entry{
		{murk::DescType::uint8}, // type
		{murk::DescType::terminate}
	};
	static murk::TieCompound
	tcomp_entry{comp_entry};

	struct field_data final {
		murk::TieCompound tcomp;
		MetaField::type_info const& info;
	};

	static field_data
	fields[]{
		{{s_comp_string}, s_type_info_string},
		{{s_comp_int32 }, s_type_info_int32 },
		{{s_comp_int64 }, s_type_info_int64 },
		{{s_comp_bool  }, s_type_info_bool  }
	};
}
} // anonymous namespace

#define HORD_THROW_MURK_ERROR__(err__, part__, ex__)				\
	HORD_THROW_ERROR_F(												\
		ErrorCode::metadata_serialization_io_failed,				\
		err__,														\
		HORD_STR_LIT(part__),										\
		&(ex__.get_tie().get_desc()),								\
		murk::get_desc_name(ex__.get_tie().get_desc().get_type()),	\
		ex__.what()													\
	)
//

// Just stop reading here. Save your sanity
#define HORD_SCOPE_FUNC_IDENT__ deserialize

HORD_FMT_SCOPED_FQN(
	s_err_metadata_murk_deserialize,
	"failed to deserialize %s at desc=(%#08p, %s);"
	" murk message:\n"
	"  >%s"
);

void
Metadata::deserialize(
	IO::InputPropStream& prop_stream
) {
	assert(IO::PropType::object_metadata == prop_stream.get_info().prop_type);
	std::istream& stream = prop_stream.get_stream();

	this->fields.clear();

	// header
	uint32_t size = 0u;
	try {
		meta_impl::tcomp_header
			.unbind()
			.bind_begin(murk::BindMutable)
				(&size)
			.bind_end()
		;
		murk::deserialize(
			stream, meta_impl::tcomp_header, murk::Endian::LITTLE
		);
	} catch (murk::SerializeError& se) {
		HORD_THROW_MURK_ERROR__(
			s_err_metadata_murk_deserialize,
			"header",
			se
		);
	}

	// fields
	this->fields.reserve(static_cast<std::size_t>(size));
	uint8_t field_type = 0x00u;
	try {
		for (
			auto it = this->fields.begin();
			it != this->fields.end();
			++it
		) {
			meta_impl::tcomp_entry
				.bind_begin(murk::BindMutable)
					(&field_type)
				.bind_end()
			;
			murk::deserialize(
				stream, meta_impl::tcomp_entry, murk::Endian::LITTLE
			);
			if (
				MetaFieldType::String > static_cast<MetaFieldType>(field_type)
			||	MetaFieldType::Bool   < static_cast<MetaFieldType>(field_type)
			) {
				HORD_THROW_ERROR_SCOPED_FQN(
					ErrorCode::metadata_serialization_type_invalid,
					"invalid field type encountered"
				);
			} else {
				meta_impl::field_data& fd = meta_impl::fields[field_type - 1];
				it->reset(fd.info.construct());
				fd.tcomp
					.bind_object_mutable(**it)
				;
				murk::deserialize(
					stream, fd.tcomp, murk::Endian::LITTLE
				);
			}
		}
	} catch (murk::SerializeError& se) {
		// Ensure empty
		this->fields.clear();
		HORD_THROW_MURK_ERROR__(
			s_err_metadata_murk_deserialize,
			"entry",
			se
		);
	}
}

#undef HORD_SCOPE_FUNC_IDENT__

#define HORD_SCOPE_FUNC_IDENT__ serialize

HORD_FMT_SCOPED_FQN(
	s_err_metadata_murk_serialize,
	"failed to serialize %s at desc=(%#08p, %s);"
	" murk message:\n"
	"  >%s"
);

void
Metadata::serialize(
	IO::OutputPropStream& prop_stream
) const {
	assert(IO::PropType::object_metadata == prop_stream.get_info().prop_type);
	std::ostream& stream = prop_stream.get_stream();

	// header
	uint32_t const size = static_cast<uint32_t>(this->fields.size());
	try {
		meta_impl::tcomp_header
			.bind_begin(murk::BindImmutable)
				(&size)
			.bind_end()
		;
		murk::serialize(
			stream, meta_impl::tcomp_header, murk::Endian::LITTLE
		);
	} catch (murk::SerializeError& se) {
		HORD_THROW_MURK_ERROR__(
			s_err_metadata_murk_serialize,
			"header",
			se
		);
	}

	// fields
	try {
		for (
			auto it = this->fields.begin();
			it != this->fields.end();
			++it
		) {
			uint8_t field_type = static_cast<uint8_t>(
				(*it)->get_type_info().type
			);
			meta_impl::tcomp_entry
				.bind_begin(murk::BindImmutable)
					(&field_type)
				.bind_end()
			;
			murk::serialize(
				stream, meta_impl::tcomp_entry, murk::Endian::LITTLE
			);
			meta_impl::field_data& fd = meta_impl::fields[field_type - 1];
			fd.tcomp
				.bind_object_immutable(**it)
			;
			murk::serialize(
				stream, fd.tcomp, murk::Endian::LITTLE
			);
		}
	} catch (murk::SerializeError& se) {
		HORD_THROW_MURK_ERROR__(
			s_err_metadata_murk_serialize,
			"entry",
			se
		);
	}
}

#undef HORD_SCOPE_FUNC_IDENT__

#undef HORD_THROW_MURK_ERROR__

#undef HORD_SCOPE_CLASS_IDENT__ // Metadata

} // namespace Data
} // namespace Hord
