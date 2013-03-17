/**
@file Metadata.hpp
@brief Metadata classes.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
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
class MetaField;
class StringMetaField;
class Int32MetaField;
class Int64MetaField;
class BoolMetaField;
struct Metadata;

/**
	@addtogroup metadata
	@{
*/

/**
	Base Metadata field.
*/
class MetaField {
public:
	/**
		Type info.
	*/
	struct type_info final {
		/** Type. */
		MetaFieldType const type;
	};

/** @name Properties */ /// @{
	/**
		Name.
		@warning This field will be truncated to 255 code units
		when serializing.
	*/
	String name{};
/// @}

private:
	MetaField(MetaField const&)=delete;
	MetaField& operator=(MetaField const&)=delete;

protected:
/** @name Implementation */ /// @{
	/**
		get_type_info() implementation.
	*/
	virtual type_info const& get_type_info_impl() const noexcept=0;
/// @}

public:
/** @name Constructors and destructor */ /// @{
	/** Default constructor. */
	MetaField();
	/**
		Constructor with name.
		@param name Name.
	*/
	explicit MetaField(String name) noexcept
		: name{std::move(name)}
	{}
	/** Move constructor. */
	MetaField(MetaField&&);
	/** Destructor. */
	virtual ~MetaField() noexcept=0;
/// @}

/** @name Operators */ /// @{
	/** Move assignment operator. */
	MetaField& operator=(MetaField&&);
/// @}

/** @name Properties */ /// @{
	/**
		Get type info.
		@returns The field's type info.
	*/
	type_info const& get_type_info() const noexcept
		{ return get_type_info_impl(); }
/// @}
};

/**
	String MetaField.
*/
class StringMetaField final
	: public MetaField {
public:
/** @name Properties */ /// @{
	/** Value. */
	String value{};
/// @}

private:
	StringMetaField(StringMetaField const&)=delete;
	StringMetaField& operator=(StringMetaField const&)=delete;

	MetaField::type_info const& get_type_info_impl() const noexcept override;

public:
/** @name Constructors and destructor */ /// @{
	/** Default constructor. */
	StringMetaField();
	/**
		Constructor with name and value.
		@param name Name.
		@param value Value.
	*/
	StringMetaField(String name, String value) noexcept
		: MetaField{std::move(name)}
		, value{std::move(value)}
	{}
	/** Move constructor. */
	StringMetaField(StringMetaField&&);
	/** Destructor. */
	~StringMetaField() noexcept override;
/// @}

/** @name Operators */ /// @{
	/** Move assignment operator. */
	StringMetaField& operator=(StringMetaField&&);
/// @}
};

/**
	Int32 MetaField.
*/
class Int32MetaField final
	: public MetaField {
public:
/** @name Properties */ /// @{
	/** Value. */
	int32_t value{0};
/// @}

private:
	Int32MetaField(Int32MetaField const&)=delete;
	Int32MetaField& operator=(Int32MetaField const&)=delete;

	MetaField::type_info const& get_type_info_impl() const noexcept override;

public:
/** @name Constructors and destructor */ /// @{
	/** Default constructor. */
	Int32MetaField();
	/**
		Constructor with name and value.
		@param name Name.
		@param value Value.
	*/
	Int32MetaField(String name, int32_t value) noexcept
		: MetaField{std::move(name)}
		, value{value}
	{}
	/** Move constructor. */
	Int32MetaField(Int32MetaField&&);
	/** Destructor. */
	~Int32MetaField() noexcept override;
/// @}

/** @name Operators */ /// @{
	/** Move assignment operator. */
	Int32MetaField& operator=(Int32MetaField&&);
/// @}
};

/**
	Int64 MetaField.
*/
class Int64MetaField final
	: public MetaField {
public:
/** @name Properties */ /// @{
	/** Value. */
	int64_t value{0};
/// @}

private:
	Int64MetaField(Int64MetaField const&)=delete;
	Int64MetaField& operator=(Int64MetaField const&)=delete;

	MetaField::type_info const& get_type_info_impl() const noexcept override;

public:
/** @name Constructors and destructor */ /// @{
	/** Default constructor. */
	Int64MetaField();
	/**
		Constructor with name and value.
		@param name Name.
		@param value Value.
	*/
	Int64MetaField(String name, int64_t value) noexcept
		: MetaField{std::move(name)}
		, value{value}
	{}
	/** Move constructor. */
	Int64MetaField(Int64MetaField&&);
	/** Destructor. */
	~Int64MetaField() noexcept override;
/// @}

/** @name Operators */ /// @{
	/** Move assignment operator. */
	Int64MetaField& operator=(Int64MetaField&&);
/// @}
};

/**
	Bool MetaField.
*/
class BoolMetaField final
	: public MetaField {
public:
/** @name Properties */ /// @{
	/** Value. */
	bool value{false};
/// @}

private:
	BoolMetaField(BoolMetaField const&)=delete;
	BoolMetaField& operator=(BoolMetaField const&)=delete;

	MetaField::type_info const& get_type_info_impl() const noexcept override;

public:
/** @name Constructors and destructor */ /// @{
	/** Default constructor. */
	BoolMetaField();
	/**
		Constructor with name and value.
		@param name Name.
		@param value Value.
	*/
	BoolMetaField(String name, bool value) noexcept
		: MetaField{std::move(name)}
		, value{value}
	{}
	/** Move constructor. */
	BoolMetaField(BoolMetaField&&);
	/** Destructor. */
	~BoolMetaField() noexcept override;
/// @}

/** @name Operators */ /// @{
	/** Move assignment operator. */
	BoolMetaField& operator=(BoolMetaField&&);
/// @}
};

/**
	Metadata.
*/
struct Metadata final {
public:
	/** MetaField vector. */
	typedef aux::vector<std::unique_ptr<MetaField> > field_vector_type;

/** @name Properties */ /// @{
	/** Fields. */
	field_vector_type fields{};
/// @}

private:
	Metadata(Metadata const&)=delete;
	Metadata& operator=(Metadata const&)=delete;

public:
/** @name Constructors and destructor */ /// @{
	/** Default constructor. */
	Metadata()=default;
	/** Move constructor. */
	Metadata(Metadata&&)=default;
	/** Destructor. */
	~Metadata() noexcept=default;
/// @}

/** @name Operators */ /// @{
	/** Move assignment operator. */
	Metadata& operator=(Metadata&&)=default;
/// @}
};

/** @} */ // end of doc-group metadata

} // namespace Hord

#endif // HORD_METADATA_HPP_
