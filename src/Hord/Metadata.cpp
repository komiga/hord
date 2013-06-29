
#include <Hord/Metadata.hpp>

#include <murk/Desc.hpp>

namespace Hord {

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

void
MetaField::bind_base(
	murk::TieBinder& binder
) noexcept {
	binder
		(&this->name)
	;
}

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
	return new StringMetaField();
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
	binder
		(&this->value)
	;
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
	return new Int32MetaField();
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
	binder
		(&this->value)
	;
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
	return new Int64MetaField();
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
	binder
		(&this->value)
	;
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
	return new BoolMetaField();
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
	binder
		(&this->value)
	;
}

BoolMetaField::BoolMetaField() = default;
BoolMetaField::BoolMetaField(BoolMetaField&&) = default;
BoolMetaField::~BoolMetaField() noexcept = default;

BoolMetaField& BoolMetaField::operator=(BoolMetaField&&) = default;

} // namespace Hord
