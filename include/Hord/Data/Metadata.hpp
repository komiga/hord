/**
@file Data/Metadata.hpp
@brief Metadata classes.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_DATA_METADATA_HPP_
#define HORD_DATA_METADATA_HPP_

#include <Hord/config.hpp>
#include <Hord/aux.hpp>
#include <Hord/String.hpp>
#include <Hord/serialization.hpp>
#include <Hord/Data/Defs.hpp>
#include <Hord/IO/PropStream.hpp>

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

	@note read() and write() will serialize base properties;
	implementations shall serialize only their own properties.
*/
class MetaField {
public:
	/**
		Type info.
	*/
	struct type_info final {
		/** Type. */
		MetaFieldType const type;

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

protected:
/** @name Implementation */ /// @{
	/**
		get_type_info() implementation.
	*/
	virtual type_info const&
	get_type_info_impl() const noexcept = 0;

	/**
		read() implementation.

		@throws SerializerError{..}
	*/
	virtual ser_result_type
	read_impl(
		InputSerializer& ser
	) = 0;

	/**
		write() implementation.

		@throws SerializerError{..}
	*/
	virtual ser_result_type
	write_impl(
		OutputSerializer& ser
	) const = 0;
/// @}

public:
/** @name Special member functions */ /// @{
	/** Destructor. */
	virtual
	~MetaField() noexcept = 0;

	/** Default constructor. */
	MetaField() = default;
	/** Move constructor. */
	MetaField(MetaField&&) = default;
	/** Move assignment operator. */
	MetaField& operator=(MetaField&&) = default;

	/**
		Constructor with name.
	*/
	explicit
	MetaField(
		String name
	) noexcept
		: name(std::move(name))
	{}
/// @}

public:
/** @name Properties */ /// @{
	/**
		Get type info.
	*/
	type_info const&
	get_type_info() const noexcept {
		return get_type_info_impl();
	}

	/**
		Set name.
	*/
	void
	set_name(
		String new_name
	) noexcept {
		this->name.assign(std::move(new_name));
		if (0xFF < this->name.size()) {
			this->name.resize(0xFF);
			// TODO: Truncate invalid unit sequence (if any) after resize
		}
	}
/// @}

/** @name Serialization */ /// @{
	/**
		Read from input serializer.

		@throws SerializerError{..}
		If a serialization operation failed.
	*/
	ser_result_type
	read(
		ser_tag_read,
		InputSerializer& ser
	) {
		ser(Cacophony::make_string_cfg<std::uint8_t>(this->name));
		read_impl(ser);
	}

	/**
		Write to output serializer.

		@throws SerializerError{..}
		If a serialization operation failed.
	*/
	ser_result_type
	write(
		ser_tag_write,
		OutputSerializer& ser
	) const {
		ser(Cacophony::make_string_cfg<std::uint8_t>(this->name));
		write_impl(ser);
	}
/// @}
};
inline MetaField::~MetaField() noexcept = default;

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

	ser_result_type
	read_impl(
		InputSerializer& ser
	) override {
		ser(this->value);
	}

	ser_result_type
	write_impl(
		OutputSerializer& ser
	) const override {
		ser(this->value);
	}

public:
/** @name Special member functions */ /// @{
	/** Destructor. */
	~StringMetaField() noexcept override = default;

	/** Default constructor. */
	StringMetaField() = default;
	/** Move constructor. */
	StringMetaField(StringMetaField&&) = default;
	/** Move assignment operator. */
	StringMetaField& operator=(StringMetaField&&) = default;

	/**
		Constructor with name and value.
	*/
	StringMetaField(
		String name,
		String value
	) noexcept
		: MetaField(std::move(name))
		, value(std::move(value))
	{}
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
	std::int32_t value{0};
/// @}

private:
	Int32MetaField(Int32MetaField const&) = delete;
	Int32MetaField& operator=(Int32MetaField const&) = delete;

	MetaField::type_info const&
	get_type_info_impl() const noexcept override;

	ser_result_type
	read_impl(
		InputSerializer& ser
	) override {
		ser(this->value);
	}

	ser_result_type
	write_impl(
		OutputSerializer& ser
	) const override {
		ser(this->value);
	}

public:
/** @name Special member functions */ /// @{
	/** Destructor. */
	~Int32MetaField() noexcept override = default;

	/** Default constructor. */
	Int32MetaField() = default;
	/** Move constructor. */
	Int32MetaField(Int32MetaField&&) = default;
	/** Move assignment operator. */
	Int32MetaField& operator=(Int32MetaField&&) = default;

	/**
		Constructor with name and value.
	*/
	Int32MetaField(
		String name,
		std::int32_t value
	) noexcept
		: MetaField(std::move(name))
		, value(value)
	{}
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
	std::int64_t value{0};
/// @}

private:
	Int64MetaField(Int64MetaField const&) = delete;
	Int64MetaField& operator=(Int64MetaField const&) = delete;

	MetaField::type_info const&
	get_type_info_impl() const noexcept override;

	ser_result_type
	read_impl(
		InputSerializer& ser
	) override {
		ser(this->value);
	}

	ser_result_type
	write_impl(
		OutputSerializer& ser
	) const override {
		ser(this->value);
	}

public:
/** @name Special member functions */ /// @{
	/** Destructor. */
	~Int64MetaField() noexcept override = default;

	/** Default constructor. */
	Int64MetaField() = default;
	/** Move constructor. */
	Int64MetaField(Int64MetaField&&) = default;
	/** Move assignment operator. */
	Int64MetaField& operator=(Int64MetaField&&) = default;

	/**
		Constructor with name and value.
	*/
	Int64MetaField(
		String name,
		std::int64_t value
	) noexcept
		: MetaField(std::move(name))
		, value(value)
	{}
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

	ser_result_type
	read_impl(
		InputSerializer& ser
	) override {
		ser(this->value);
	}

	ser_result_type
	write_impl(
		OutputSerializer& ser
	) const override {
		ser(this->value);
	}

public:
/** @name Special member functions */ /// @{
	/** Destructor. */
	~BoolMetaField() noexcept override = default;

	/** Default constructor. */
	BoolMetaField() = default;
	/** Move constructor. */
	BoolMetaField(BoolMetaField&&) = default;
	/** Move assignment operator. */
	BoolMetaField& operator=(BoolMetaField&&) = default;

	/**
		Constructor with name and value.
	*/
	BoolMetaField(
		String name,
		bool value
	) noexcept
		: MetaField(std::move(name))
		, value(value)
	{}
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
/** @name Special member functions */ /// @{
	/** Destructor. */
	~Metadata() noexcept = default;

	/** Default constructor. */
	Metadata() = default;
	/** Move constructor. */
	Metadata(Metadata&&) = default;
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
