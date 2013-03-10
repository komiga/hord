
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

// class StringMetaField implementation

MetaField::type_info const& StringMetaField::get_type_info_impl()
const noexcept {
	return s_type_info_string;
}

// class Int32MetaField implementation

MetaField::type_info const& Int32MetaField::get_type_info_impl()
const noexcept {
	return s_type_info_int32;
}

// class Int64MetaField implementation

MetaField::type_info const& Int64MetaField::get_type_info_impl()
const noexcept {
	return s_type_info_int64;
}

// class BoolMetaField implementation

MetaField::type_info const& BoolMetaField::get_type_info_impl()
const noexcept {
	return s_type_info_bool;
}

} // namespace Hord
