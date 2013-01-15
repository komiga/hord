/**
@file Metadata.hpp
@brief Metadata classes.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license; see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_METADATA_HPP_
#define HORD_METADATA_HPP_

#include "./config.hpp"
#include "./common_types.hpp"
#include "./common_enums.hpp"
#include "./aux.hpp"
#include "./String.hpp"

#include <memory>

namespace Hord {

// Forward declarations
struct MetaField;
struct StringMetaField;
struct Int32MetaField;
struct Int64MetaField;
struct BoolMetaField;
struct Metadata;

/**
	@addtogroup metadata
	@{
*/

/**
	Base Metadata field.
*/
struct MetaField {
	/**
		Type info.
	*/
	struct type_info final {
		/** Type. */
		MetaFieldType const type;
	};

	/**
		Name.
		@warning This field will be truncated to 255 code units when serializing.
	*/
	String name{};

private:
	MetaField(MetaField const&)=delete;
	MetaField& operator=(MetaField const&)=delete;
	MetaField& operator=(MetaField&&)=delete;

public:
/** @name Constructors and destructor */ /// @{
	/** Constructor. */
	MetaField()=default;
	/** Move constructor. */
	MetaField(MetaField&&)=default;
	/** Destructor. */
	inline virtual ~MetaField()=0;
/// @}

/** @name Properties */ /// @{
	/**
		Get type info.
		@returns The field's type info.
	*/
	type_info const& get_type_info() const noexcept { return get_type_info_impl(); }
/// @}

private:
	/**
		get_type_info() implementation.
		@returns The field's type info.
	*/
	virtual type_info const& get_type_info_impl() const noexcept=0;
};
/** @cond INTERNAL */
inline MetaField::~MetaField()=default;
/** @endcond */ // INTERNAL

/**
	String MetaField.
*/
struct StringMetaField final : public MetaField {
	/** Value. */
	String value{};

private:
	StringMetaField(StringMetaField const&)=delete;
	StringMetaField& operator=(StringMetaField const&)=delete;
	StringMetaField& operator=(StringMetaField&&)=delete;

public:
/** @name Constructors and destructor */ /// @{
	/** Constructor. */
	StringMetaField()=default;
	/** Move constructor. */
	StringMetaField(StringMetaField&&)=default;
	/** Destructor. */
	~StringMetaField() override=default;
/// @}

private:
	static constexpr MetaField::type_info s_type_info{
		static_cast<MetaFieldType>(StandardMetaFieldTypes::String)
	};
	MetaField::type_info const& get_type_info_impl() const noexcept override { return s_type_info; }
};
/** @cond INTERNAL */
constexpr MetaField::type_info StringMetaField::s_type_info;
/** @endcond */

/**
	Int32 MetaField.
*/
struct Int32MetaField final : public MetaField {
	/** Value. */
	int32_t value{0};

private:
	Int32MetaField(Int32MetaField const&)=delete;
	Int32MetaField& operator=(Int32MetaField const&)=delete;
	Int32MetaField& operator=(Int32MetaField&&)=delete;

public:
/** @name Constructors and destructor */ /// @{
	/** Constructor. */
	Int32MetaField()=default;
	/** Move constructor. */
	Int32MetaField(Int32MetaField&&)=default;
	/** Destructor. */
	~Int32MetaField() override=default;
/// @}

private:
	static constexpr MetaField::type_info s_type_info{
		static_cast<MetaFieldType>(StandardMetaFieldTypes::Int32)
	};
	MetaField::type_info const& get_type_info_impl() const noexcept override { return s_type_info; }
};
/** @cond INTERNAL */
constexpr MetaField::type_info Int32MetaField::s_type_info;
/** @endcond */

/**
	Int64 MetaField.
*/
struct Int64MetaField final : public MetaField {
	/** Value. */
	int64_t value{0};

private:
	Int64MetaField(Int64MetaField const&)=delete;
	Int64MetaField& operator=(Int64MetaField const&)=delete;
	Int64MetaField& operator=(Int64MetaField&&)=delete;

public:
/** @name Constructors and destructor */ /// @{
	/** Constructor. */
	Int64MetaField()=default;
	/** Move constructor. */
	Int64MetaField(Int64MetaField&&)=default;
	/** Destructor. */
	~Int64MetaField() override=default;
/// @}

private:
	static constexpr MetaField::type_info s_type_info{
		static_cast<MetaFieldType>(StandardMetaFieldTypes::Int64)
	};
	MetaField::type_info const& get_type_info_impl() const noexcept override { return s_type_info; }
};
/** @cond INTERNAL */
constexpr MetaField::type_info Int64MetaField::s_type_info;
/** @endcond */

/**
	Bool MetaField.
*/
struct BoolMetaField final : public MetaField {
	/** Value. */
	bool value{false};

private:
	BoolMetaField(BoolMetaField const&)=delete;
	BoolMetaField& operator=(BoolMetaField const&)=delete;
	BoolMetaField& operator=(BoolMetaField&&)=delete;

public:
/** @name Constructors and destructor */ /// @{
	/** Constructor. */
	BoolMetaField()=default;
	/** Move constructor. */
	BoolMetaField(BoolMetaField&&)=default;
	/** Destructor. */
	~BoolMetaField() override=default;
/// @}

private:
	static constexpr MetaField::type_info s_type_info{
		static_cast<MetaFieldType>(StandardMetaFieldTypes::Bool)
	};
	MetaField::type_info const& get_type_info_impl() const noexcept override { return s_type_info; }
};
/** @cond INTERNAL */
constexpr MetaField::type_info BoolMetaField::s_type_info;
/** @endcond */

/**
	Metadata.
*/
struct Metadata final {
	/** MetaField vector. */
	typedef aux::vector<std::shared_ptr<MetaField> > field_vector_type;

	/** Fields. */
	field_vector_type fields{};

private:
	Metadata(Metadata const&)=delete;
	Metadata& operator=(Metadata const&)=delete;
	Metadata& operator=(Metadata&&)=delete;

public:
/** @name Constructors and destructor */ /// @{
	/** Constructor. */
	Metadata()=default;
	/** Move constructor. */
	Metadata(Metadata&&)=default;
	/** Destructor. */
	~Metadata()=default;
/// @}
};

/** @} */ // end of doc-group metadata

} // namespace Hord

#endif // HORD_METADATA_HPP_
