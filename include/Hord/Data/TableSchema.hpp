/**
@file
@brief TableSchema class.
@ingroup data

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#pragma once

#include <Hord/config.hpp>
#include <Hord/aux.hpp>
#include <Hord/String.hpp>
#include <Hord/utility.hpp>
#include <Hord/serialization.hpp>
#include <Hord/Data/Defs.hpp>

#include <duct/debug.hpp>

namespace Hord {
namespace Data {

// Forward declarations
class TableSchema;

/**
	@addtogroup data
	@{
*/

/**
	Table schema.
*/
class TableSchema {
public:
	/**
		Column schema.
	*/
	struct Column {
		/** Name. */
		String name;

		/** Type. */
		Data::Type type;

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
				Cacophony::make_string_cfg<std::uint8_t>(self.name),
				self.type
			);
		}
	};

	/**
		Column vector type.
	*/
	using column_vector_type = aux::vector<Column>;

private:
	Data::SchemaHashValue m_hash{HASH_EMPTY};
	column_vector_type m_columns{};

	TableSchema(TableSchema const&) = delete;
	TableSchema& operator=(TableSchema const&) = delete;

public:
/** @name Special member functions */ /// @{
	/** Destructor. */
	~TableSchema() noexcept = default;

	/** Default constructor. */
	TableSchema() = default;
	/** Move constructor. */
	TableSchema(TableSchema&&) = default;
	/** Move assignment operator. */
	TableSchema& operator=(TableSchema&&) = default;

	/**
		Construct with column initializer list.
	*/
	TableSchema(
		std::initializer_list<Data::TableSchema::Column> const ilist
	) noexcept;
/// @}

/** @name Properties */ /// @{
	/**
		Get schema hash.
	*/
	Data::SchemaHashValue
	get_hash() const noexcept {
		return m_hash;
	}

	/**
		Get columns (mutable).
	*/
	column_vector_type&
	get_columns() noexcept {
		return m_columns;
	}

	/**
		Get columns.
	*/
	column_vector_type const&
	get_columns() const noexcept {
		return m_columns;
	}
/// @}

/** @name Layout */ /// @{
	/**
		Update schema hash.

		@returns @c true if the hash changed.
	*/
	bool
	update() noexcept;

	/**
		Insert a column.

		@throws Error{ErrorCode::table_schema_column_name_shared}
		If a column with @a name already exists.
	*/
	void
	insert(
		unsigned const index,
		String name,
		Data::Type const type
	);

	/**
		Remove a column.
	*/
	void
	remove(
		unsigned const index
	) noexcept;
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
	);

	/**
		Write to output serializer.

		@throws SerializerError{..}
		If a serialization operation failed.
	*/
	ser_result_type
	write(
		ser_tag_write,
		OutputSerializer& ser
	) const;
/// @}
};

/** @} */ // end of doc-group data

} // namespace Data
} // namespace Hord
