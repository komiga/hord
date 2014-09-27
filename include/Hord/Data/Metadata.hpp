/**
@file Data/Metadata.hpp
@brief Metadata classes.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#pragma once

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
struct MetaField;
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
	Metadata field.
*/
struct MetaField {
public:
/** @name Properties */ /// @{
	/**
		Name.

		@warning This field will be truncated to 255 code units.
	*/
	String name{};

	/** Type. */
	Data::FieldType type{Data::FieldType::Text};

	/** Value. */
	struct {
		String str{}; /**< String value. */
		union {
			std::int64_t num{0}; /**< Integer value. */
			bool bin; /**< Boolean value. */
		};
	} value{};
/// @}

private:
	MetaField& operator=(MetaField const&) = delete;

public:
/** @name Special member functions */ /// @{
	/** Destructor. */
	~MetaField() noexcept = default;

	/** Default constructor. */
	MetaField() = default;
	/** Move constructor. */
	MetaField(MetaField&&) = default;
	/** Move assignment operator. */
	MetaField& operator=(MetaField&&) = default;
	/** Copy constructor. */
	MetaField(MetaField const&) = default;

	/**
		Constructor with name and type (default value).
	*/
	explicit
	MetaField(
		String name,
		Data::FieldType const type
	) noexcept
		: name(std::move(name))
		, type(type)
	{}

	/**
		Constructor with name and text value.
	*/
	explicit
	MetaField(
		String name,
		String value
	) noexcept
		: name(std::move(name))
		, type(Data::FieldType::Text)
	{
		// FIXME: Yuck 1.
		this->value.str = std::move(value);
	}

	/**
		Constructor with name and number value.
	*/
	explicit
	MetaField(
		String name,
		std::int64_t const value
	) noexcept
		: name(std::move(name))
		, type(Data::FieldType::Number)
	{
		// FIXME: Yuck 2.
		this->value.num = value;
	}

	/**
		Constructor with name and boolean value.
	*/
	explicit
	MetaField(
		String name,
		bool const value
	) noexcept
		: name(std::move(name))
		, type(Data::FieldType::Boolean)
	{
		// FIXME: Yuck 3.
		this->value.bin = value;
	}
/// @}

public:
/** @name Properties */ /// @{
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
		Serialize.

		@throws SerializerError{..}
		If a serialization operation failed.
	*/
	template<class Ser>
	ser_result_type
	serialize(
		ser_tag_serialize,
		Ser& ser
	) {
		auto& self = const_safe<Ser>(*this);
		ser(
			self.type,
			Cacophony::make_string_cfg<std::uint8_t>(self.name)
		);
		switch (self.type) {
		case Data::FieldType::Text   : ser(self.value.str); break;
		case Data::FieldType::Number : ser(self.value.num); break;
		case Data::FieldType::Boolean: ser(self.value.bin); break;
		}
	}
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
	= aux::vector<Data::MetaField>;

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
