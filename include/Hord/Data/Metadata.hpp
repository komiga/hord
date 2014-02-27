/**
@file Data/Metadata.hpp
@brief Metadata classes.

@author Tim Howard
@copyright 2013-2014 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_DATA_METADATA_HPP_
#define HORD_DATA_METADATA_HPP_

#include <Hord/config.hpp>
#include <Hord/aux.hpp>
#include <Hord/String.hpp>
#include <Hord/Data/Defs.hpp>
#include <Hord/IO/PropStream.hpp>

#include <murk/DescCompound.hpp>
#include <murk/TieCompound.hpp>

#include <duct/cc_unique_ptr.hpp>

namespace Hord {
namespace Data {

// Forward declarations
class MetaField;
class StringMetaField;
class Int32MetaField;
class Int64MetaField;
class BoolMetaField;
struct Metadata;

/**
	@addtogroup data
	@{
*/
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
		Base descriptor compound.
	*/
	static murk::DescCompound const s_comp_base;

	/**
		Type info.
	*/
	struct type_info final {
		/** Type. */
		MetaFieldType const type;

		/**
			Descriptor compound for type.

			@note This must begin with a reference
			to @c MetaField::s_comp_base.
		*/
		murk::DescCompoundRef const comp;

		/**
			Default-construct field of this type.

			@returns Pointer to field, or @c nullptr if allocation
			failed.
		*/
		MetaField*
		(&construct)() noexcept;
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
	MetaField(MetaField const&) = delete;
	MetaField& operator=(MetaField const&) = delete;

	void
	bind_base(
		murk::TieBinder& binder
	) noexcept;

	void
	bind_const_base(
		murk::TieBinder& binder
	) const noexcept;

protected:
/** @name Implementation */ /// @{
	/**
		get_type_info() implementation.
	*/
	virtual type_info const&
	get_type_info_impl() const noexcept = 0;

	/**
		bind() implementation.

		@note bind() will bind base properties; implementations shall
		bind only their own properties.
		@{
	*/
	virtual void
	bind_impl(
		murk::TieBinder&
	) noexcept = 0;

	virtual void
	bind_const_impl(
		murk::TieBinder&
	) const noexcept = 0;
	/** @} */
/// @}

public:
/** @name Constructors and destructor */ /// @{
	/** Default constructor. */
	MetaField();

	/**
		Constructor with name.

		@param name Name.
	*/
	explicit
	MetaField(
		String name
	) noexcept
		: name(std::move(name))
	{}

	/** Move constructor. */
	MetaField(MetaField&&);
	/** Destructor. */
	virtual
	~MetaField() noexcept = 0;
/// @}

/** @name Operators */ /// @{
	/** Move assignment operator. */
	MetaField& operator=(MetaField&&);
/// @}

/** @name Properties */ /// @{
	/**
		Get type info.
	*/
	type_info const&
	get_type_info() const noexcept {
		return get_type_info_impl();
	}
/// @}

/** @name Serialization */ /// @{
	/**
		Bind field.

		@param binder Tie binder.
		@{
	*/
	void
	bind(
		murk::TieBinder& binder
	) noexcept {
		bind_base(binder);
		bind_impl(binder);
	}

	void
	bind(
		murk::TieBinder& binder
	) const noexcept {
		bind_const_base(binder);
		bind_const_impl(binder);
	}
	/** @} */
/// @}
};

/**
	String MetaField.
*/
class StringMetaField final
	: public MetaField
{
public:
/** @name Properties */ /// @{
	/** Value. */
	String value{};
/// @}

private:
	StringMetaField(StringMetaField const&) = delete;
	StringMetaField& operator=(StringMetaField const&) = delete;

	MetaField::type_info const&
	get_type_info_impl() const noexcept override;

	void
	bind_impl(
		murk::TieBinder&
	) noexcept override;

	void
	bind_const_impl(
		murk::TieBinder&
	) const noexcept override;

public:
/** @name Constructors and destructor */ /// @{
	/** Default constructor. */
	StringMetaField();

	/**
		Constructor with name and value.

		@param name Name.
		@param value Value.
	*/
	StringMetaField(
		String name,
		String value
	) noexcept
		: MetaField(std::move(name))
		, value(std::move(value))
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
	: public MetaField
{
public:
/** @name Properties */ /// @{
	/** Value. */
	int32_t value{0};
/// @}

private:
	Int32MetaField(Int32MetaField const&) = delete;
	Int32MetaField& operator=(Int32MetaField const&) = delete;

	MetaField::type_info const&
	get_type_info_impl() const noexcept override;

	void
	bind_impl(
		murk::TieBinder&
	) noexcept override;

	void
	bind_const_impl(
		murk::TieBinder&
	) const noexcept override;

public:
/** @name Constructors and destructor */ /// @{
	/** Default constructor. */
	Int32MetaField();

	/**
		Constructor with name and value.

		@param name Name.
		@param value Value.
	*/
	Int32MetaField(
		String name,
		int32_t value
	) noexcept
		: MetaField(std::move(name))
		, value(value)
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
	: public MetaField
{
public:
/** @name Properties */ /// @{
	/** Value. */
	int64_t value{0};
/// @}

private:
	Int64MetaField(Int64MetaField const&) = delete;
	Int64MetaField& operator=(Int64MetaField const&) = delete;

	MetaField::type_info const&
	get_type_info_impl() const noexcept override;

	void
	bind_impl(
		murk::TieBinder&
	) noexcept override;

	void
	bind_const_impl(
		murk::TieBinder&
	) const noexcept override;

public:
/** @name Constructors and destructor */ /// @{
	/** Default constructor. */
	Int64MetaField();

	/**
		Constructor with name and value.

		@param name Name.
		@param value Value.
	*/
	Int64MetaField(
		String name,
		int64_t value
	) noexcept
		: MetaField(std::move(name))
		, value(value)
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
	: public MetaField
{
public:
/** @name Properties */ /// @{
	/** Value. */
	bool value{false};
/// @}

private:
	BoolMetaField(BoolMetaField const&) = delete;
	BoolMetaField& operator=(BoolMetaField const&) = delete;

	MetaField::type_info const&
	get_type_info_impl() const noexcept override;

	void
	bind_impl(
		murk::TieBinder&
	) noexcept override;

	void
	bind_const_impl(
		murk::TieBinder&
	) const noexcept override;

public:
/** @name Constructors and destructor */ /// @{
	/** Default constructor. */
	BoolMetaField();

	/**
		Constructor with name and value.

		@param name Name.
		@param value Value.
	*/
	BoolMetaField(
		String name,
		bool value
	) noexcept
		: MetaField(std::move(name))
		, value(value)
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

	@sa IO::PropType::metadata
*/
struct Metadata final {
public:
	/** MetaField vector. */
	using field_vector_type
	= aux::vector<
		duct::cc_unique_ptr<MetaField>
	>;

/** @name Properties */ /// @{
	/** Fields. */
	field_vector_type fields{};
/// @}

private:
	Metadata(Metadata const&) = delete;
	Metadata& operator=(Metadata const&) = delete;

public:
/** @name Constructors and destructor */ /// @{
	/** Default constructor. */
	Metadata() = default;
	/** Move constructor. */
	Metadata(Metadata&&) = default;
	/** Destructor. */
	~Metadata() noexcept = default;
/// @}

/** @name Operators */ /// @{
	/** Move assignment operator. */
	Metadata& operator=(Metadata&&) = default;
/// @}

/** @name Serialization */ /// @{
	/**
		Deserialize from prop stream.

		@note State will be retained if an exception is thrown.

		@pre @code
			IO::PropType::metadata == prop_stream.get_type()
		@endcode

		@throws Error{ErrorCode::serialization_io_failed}
		If an input operation failed.

		@throws Error{ErrorCode::serialization_data_malformed}
		If an invalid field type is encountered in the prop stream.

		@param prop_stream %Prop stream.
	*/
	void
	deserialize(
		IO::InputPropStream& prop_stream
	);

	/**
		Serialize to prop stream.

		@note State will be retained if an exception is thrown.

		@pre @code
			IO::PropType::metadata == prop_stream.get_type()
		@endcode

		@throws Error{ErrorCode::serialization_io_failed}
		If an output operation failed.

		@param prop_stream %Prop stream.
	*/
	void
	serialize(
		IO::OutputPropStream& prop_stream
	) const;
/// @}
};

/** @} */ // end of doc-group metadata
/** @} */ // end of doc-group data

} // namespace Data
} // namespace Hord

#endif // HORD_DATA_METADATA_HPP_
