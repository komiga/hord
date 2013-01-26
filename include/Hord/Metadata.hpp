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

public:
/** @name Constructors and destructor */ /// @{
	/** Constructor. */
	MetaField()=default;
	/**
		Constructor with name.
		@param name Name.
	*/
	explicit MetaField(String name)
		: name{std::move(name)}
	{}
	/** Move constructor. */
	MetaField(MetaField&&)=default;
	/** Destructor. */
	inline virtual ~MetaField()=0;
/// @}

/** @name Operators */ /// @{
	/** Move assignment operator. */
	MetaField& operator=(MetaField&&)=default;
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
inline MetaField::~MetaField()=default;

/**
	String MetaField.
*/
struct StringMetaField final : public MetaField {
	/** Value. */
	String value{};

private:
	StringMetaField(StringMetaField const&)=delete;
	StringMetaField& operator=(StringMetaField const&)=delete;

public:
/** @name Constructors and destructor */ /// @{
	/** Constructor. */
	StringMetaField()=default;
	/**
		Constructor with name and value.
		@param name Name.
		@param value Value.
	*/
	StringMetaField(String name, String value)
		: MetaField{std::move(name)}
		, value{std::move(value)}
	{}
	/** Move constructor. */
	StringMetaField(StringMetaField&&)=default;
	/** Destructor. */
	~StringMetaField() override=default;
/// @}

/** @name Operators */ /// @{
	/** Move assignment operator. */
	StringMetaField& operator=(StringMetaField&&)=default;
/// @}

private:
	static constexpr MetaField::type_info s_type_info{
		static_cast<MetaFieldType>(StandardMetaFieldTypes::String)
	};
	MetaField::type_info const& get_type_info_impl() const noexcept override { return s_type_info; }
};

/**
	Int32 MetaField.
*/
struct Int32MetaField final : public MetaField {
	/** Value. */
	int32_t value{0};

private:
	Int32MetaField(Int32MetaField const&)=delete;
	Int32MetaField& operator=(Int32MetaField const&)=delete;

public:
/** @name Constructors and destructor */ /// @{
	/** Constructor. */
	Int32MetaField()=default;
	/**
		Constructor with name and value.
		@param name Name.
		@param value Value.
	*/
	Int32MetaField(String name, int32_t value)
		: MetaField{std::move(name)}
		, value{value}
	{}
	/** Move constructor. */
	Int32MetaField(Int32MetaField&&)=default;
	/** Destructor. */
	~Int32MetaField() override=default;
/// @}

/** @name Operators */ /// @{
	/** Move assignment operator. */
	Int32MetaField& operator=(Int32MetaField&&)=default;
/// @}

private:
	static constexpr MetaField::type_info s_type_info{
		static_cast<MetaFieldType>(StandardMetaFieldTypes::Int32)
	};
	MetaField::type_info const& get_type_info_impl() const noexcept override { return s_type_info; }
};

/**
	Int64 MetaField.
*/
struct Int64MetaField final : public MetaField {
	/** Value. */
	int64_t value{0};

private:
	Int64MetaField(Int64MetaField const&)=delete;
	Int64MetaField& operator=(Int64MetaField const&)=delete;

public:
/** @name Constructors and destructor */ /// @{
	/** Constructor. */
	Int64MetaField()=default;
	/**
		Constructor with name and value.
		@param name Name.
		@param value Value.
	*/
	Int64MetaField(String name, int64_t value)
		: MetaField{std::move(name)}
		, value{value}
	{}
	/** Move constructor. */
	Int64MetaField(Int64MetaField&&)=default;
	/** Destructor. */
	~Int64MetaField() override=default;
/// @}

/** @name Operators */ /// @{
	/** Move assignment operator. */
	Int64MetaField& operator=(Int64MetaField&&)=default;
/// @}

private:
	static constexpr MetaField::type_info s_type_info{
		static_cast<MetaFieldType>(StandardMetaFieldTypes::Int64)
	};
	MetaField::type_info const& get_type_info_impl() const noexcept override { return s_type_info; }
};

/**
	Bool MetaField.
*/
struct BoolMetaField final : public MetaField {
	/** Value. */
	bool value{false};

private:
	BoolMetaField(BoolMetaField const&)=delete;
	BoolMetaField& operator=(BoolMetaField const&)=delete;

public:
/** @name Constructors and destructor */ /// @{
	/** Constructor. */
	BoolMetaField()=default;
	/**
		Constructor with name and value.
		@param name Name.
		@param value Value.
	*/
	BoolMetaField(String name, bool value)
		: MetaField{std::move(name)}
		, value{value}
	{}
	/** Move constructor. */
	BoolMetaField(BoolMetaField&&)=default;
	/** Destructor. */
	~BoolMetaField() override=default;
/// @}

/** @name Operators */ /// @{
	/** Move assignment operator. */
	BoolMetaField& operator=(BoolMetaField&&)=default;
/// @}

private:
	static constexpr MetaField::type_info s_type_info{
		static_cast<MetaFieldType>(StandardMetaFieldTypes::Bool)
	};
	MetaField::type_info const& get_type_info_impl() const noexcept override { return s_type_info; }
};

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

public:
/** @name Constructors and destructor */ /// @{
	/** Constructor. */
	Metadata()=default;
	/**
		Constructor with field vector.
		@param fields MetaField vector.
	*/
	explicit Metadata(field_vector_type fields)
		: fields{std::move(fields)}
	{}
	/** Move constructor. */
	Metadata(Metadata&&)=default;
	/** Destructor. */
	~Metadata()=default;
/// @}

/** @name Operators */ /// @{
	/** Move assignment operator. */
	Metadata& operator=(Metadata&&)=default;
/// @}
};

/** @} */ // end of doc-group metadata

} // namespace Hord

#endif // HORD_METADATA_HPP_
