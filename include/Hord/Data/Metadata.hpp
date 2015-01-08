/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Metadata class.
@ingroup metadata
*/

#pragma once

#include <Hord/config.hpp>
#include <Hord/aux.hpp>
#include <Hord/String.hpp>
#include <Hord/serialization.hpp>
#include <Hord/Data/Defs.hpp>
#include <Hord/Data/Table.hpp>
#include <Hord/IO/PropStream.hpp>

namespace Hord {
namespace Data {

// Forward declarations
class Metadata;

/**
	@addtogroup metadata
	@{
*/

/**
	Metadata.

	@sa IO::PropType::metadata
*/
class Metadata final {
public:
	/**
		Table schema.
	*/
	static Data::TableSchema const s_schema;

	/**
		Column indices.
	*/
	enum : unsigned {
		/** Name column. */
		COL_NAME,
		/** Value column. */
		COL_VALUE,
	};

private:
	Data::Table m_table{s_schema};

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

/** @name Properties */ /// @{
	/**
		Get table (mutable).
	*/
	Data::Table&
	table() noexcept {
		return m_table;
	}

	/**
		Get table.
	*/
	Data::Table const&
	table() const noexcept {
		return m_table;
	}

	/**
		Get the number of fields.
	*/
	unsigned
	num_fields() const noexcept {
		return m_table.num_records();
	}
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

} // namespace Data
} // namespace Hord
