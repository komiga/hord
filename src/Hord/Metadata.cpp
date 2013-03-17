
#include <Hord/Metadata.hpp>

namespace Hord {

namespace {
static constexpr MetaField::type_info s_type_info_string{
	static_cast<MetaFieldType>(StandardMetaFieldTypes::String)
};

static constexpr MetaField::type_info s_type_info_int32{
	static_cast<MetaFieldType>(StandardMetaFieldTypes::Int32)
};

static constexpr MetaField::type_info s_type_info_int64{
	static_cast<MetaFieldType>(StandardMetaFieldTypes::Int64)
};

static constexpr MetaField::type_info s_type_info_bool{
	static_cast<MetaFieldType>(StandardMetaFieldTypes::Bool)
};
} // anonymous namespace

// class MetaField implementation

MetaField::MetaField()=default;
MetaField::MetaField(MetaField&&)=default;
MetaField::~MetaField() noexcept=default;

MetaField& MetaField::operator=(MetaField&&)=default;

// class StringMetaField implementation

MetaField::type_info const& StringMetaField::get_type_info_impl()
const noexcept {
	return s_type_info_string;
}

StringMetaField::StringMetaField()=default;
StringMetaField::StringMetaField(StringMetaField&&)=default;
StringMetaField::~StringMetaField() noexcept=default;

StringMetaField& StringMetaField::operator=(StringMetaField&&)=default;

// class Int32MetaField implementation

MetaField::type_info const& Int32MetaField::get_type_info_impl()
const noexcept {
	return s_type_info_int32;
}

Int32MetaField::Int32MetaField()=default;
Int32MetaField::Int32MetaField(Int32MetaField&&)=default;
Int32MetaField::~Int32MetaField() noexcept=default;

Int32MetaField& Int32MetaField::operator=(Int32MetaField&&)=default;

// class Int64MetaField implementation

MetaField::type_info const& Int64MetaField::get_type_info_impl()
const noexcept {
	return s_type_info_int64;
}

Int64MetaField::Int64MetaField()=default;
Int64MetaField::Int64MetaField(Int64MetaField&&)=default;
Int64MetaField::~Int64MetaField() noexcept=default;

Int64MetaField& Int64MetaField::operator=(Int64MetaField&&)=default;

// class BoolMetaField implementation

MetaField::type_info const& BoolMetaField::get_type_info_impl()
const noexcept {
	return s_type_info_bool;
}

BoolMetaField::BoolMetaField()=default;
BoolMetaField::BoolMetaField(BoolMetaField&&)=default;
BoolMetaField::~BoolMetaField() noexcept=default;

BoolMetaField& BoolMetaField::operator=(BoolMetaField&&)=default;

} // namespace Hord
