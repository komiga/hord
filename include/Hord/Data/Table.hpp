/**
@file
@brief Table class.
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
#include <Hord/Data/ValueStore.hpp>

#include <duct/debug.hpp>

namespace Hord {
namespace Data {

// Forward declarations
struct Column;
class Table;

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

/**
	Data table.
*/
class Table {
public:
	/**
		Column vector type.
	*/
	using column_vector_type = aux::vector<Data::ValueStore>;

private:
	unsigned m_num_rows{0};
	Data::SchemaHashValue m_hash{HASH_EMPTY};
	column_vector_type m_columns{};

	Table(Table const&) = delete;
	Table& operator=(Table const&) = delete;

public:
/** @name Special member functions */ /// @{
	/** Destructor. */
	~Table() noexcept = default;

	/** Default constructor. */
	Table() = default;
	/** Move constructor. */
	Table(Table&&) = default;
	/** Move assignment operator. */
	Table& operator=(Table&&) = default;

	/**
		Construct with schema.
	*/
	Table(
		Data::TableSchema const& schema
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

	/**
		Get number of rows.
	*/
	unsigned
	num_rows() const noexcept {
		return m_num_rows;
	}

	/**
		Get number of columns.
	*/
	unsigned
	num_columns() const noexcept {
		return m_columns.size();
	}
/// @}

/** @name Layout */ /// @{
	/**
		Get column by index (mutable).
	*/
	Data::ValueStore&
	column(
		unsigned const index
	) noexcept {
		DUCT_ASSERTE(index < m_columns.size());
		return m_columns[index];
	}

	/**
		Get column by index.
	*/
	Data::ValueStore const&
	column(
		unsigned const index
	) const noexcept {
		DUCT_ASSERTE(index < m_columns.size());
		return m_columns[index];
	}

	/**
		Configure the table with a schema.

		@returns @c true if the schema differed.
	*/
	bool
	configure(
		Data::TableSchema const& schema
	);
/// @}

/** @name Modification */ /// @{
	/**
		Push values to the end of the table.
	*/
	void
	push_back(
		std::initializer_list<Data::ValueRef> const values
	) noexcept;

	/**
		Remove row.
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
