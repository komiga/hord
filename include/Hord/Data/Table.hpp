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

#include <duct/debug.hpp>

namespace Hord {
namespace Data {

// Forward declarations
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
	struct Chunk {
		std::uint8_t* data{nullptr};
		std::uint8_t* head{nullptr};
		std::uint8_t* tail{nullptr};
		unsigned size{0};
		unsigned num_records{0};

		unsigned
		offset_head() const noexcept {
			return head - data;
		}
		unsigned
		offset_tail() const noexcept {
			return tail - data;
		}

		unsigned
		space_head() const noexcept {
			return offset_head();
		}
		unsigned
		space_tail() const noexcept {
			return size - offset_tail();
		}

		unsigned
		space_used() const noexcept {
			return tail - head;
		}

		unsigned
		space() const noexcept {
			return space_used() - size;
		}
	};

	friend struct Iterator;
	struct Iterator {
		Data::Table* table;
		unsigned index;
		unsigned chunk_index;
		unsigned inner_index;
		unsigned data_offset;

		bool
		operator==(
			Iterator const& rhs
		) const noexcept {
			return table == rhs.table && index == rhs.index;
		}

		bool
		operator!=(
			Iterator const& rhs
		) const noexcept {
			return !this->operator==(rhs);
		}

		bool
		operator<(
			Iterator const& rhs
		) const noexcept {
			return index < rhs.index;
		}

		/**
			Advance.
		*/
		Iterator&
		operator++() noexcept;

		/**
			Advance by count.
		*/
		Iterator&
		operator+=(
			unsigned count
		) noexcept;

		/**
			Whether the iterator can advance.
		*/
		bool
		can_advance() const noexcept {
			return table && index < table->m_num_records;
		}

		/**
			Insert a record at iterator.

			@sa Data::Table::insert()
		*/
		void
		insert(
			unsigned const num_fields,
			Data::ValueRef* const fields
		) noexcept {
			table->insert(*this, num_fields, fields);
		}

		/**
			Remove record at iterator.
		*/
		void
		remove() noexcept {
			table->remove(*this);
		}

		/**
			Set field value at iterator.
		*/
		void
		set_field(
			unsigned const column_index,
			Data::ValueRef value
		) {
			table->set_field(*this, column_index, value);
		}

		/**
			Get field value at iterator.
		*/
		Data::ValueRef
		get_field(
			unsigned const column_index
		) const noexcept {
			return table->get_field(*this, column_index);
		}
	};

	/**
		Chunk vector type.
	*/
	using chunk_vector_type = aux::vector<Chunk>;

	/**
		Column vector type.
	*/
	using column_vector_type = aux::vector<Data::Type>;

private:
	unsigned m_num_records{0};
	Data::SchemaHashValue m_hash{HASH_EMPTY};
	column_vector_type m_columns{};
	chunk_vector_type m_chunks{};

	Table(Table const&) = delete;
	Table& operator=(Table const&) = delete;

private:
	void free_chunks();

public:
/** @name Special member functions */ /// @{
	/** Destructor. */
	~Table() noexcept;

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
		Get number of records.
	*/
	unsigned
	num_records() const noexcept {
		return m_num_records;
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
	// TODO: Replace this function with a table schema mutation interface
	/**
		Configure the table with a schema.

		@returns @c true if the schema differed.
	*/
	bool
	configure(
		Data::TableSchema const& schema
	);

	/**
		Get column type by index.
	*/
	Data::Type
	column(
		unsigned const index
	) noexcept {
		return index < m_columns.size() ? m_columns[index] : Data::Type{};
	}
/// @}

/** @name Iteration */ /// @{
	/**
		Get beginning iterator.
	*/
	Data::Table::Iterator
	begin() {
		if (m_chunks.empty()) {
			return {this, m_num_records, 0, 0, 0};
		} else {
			auto const& chunk = m_chunks.front();
			return {this, 0, 0, 0, chunk.offset_head()};
		}
	}

	/**
		Get ending iterator.
	*/
	Data::Table::Iterator
	end() {
		if (m_chunks.empty()) {
			return {this, m_num_records, 0, 0, 0};
		} else {
			auto const& chunk = m_chunks.back();
			return {
				this,
				m_num_records,
				static_cast<unsigned>(m_chunks.size() - 1),
				chunk.num_records,
				chunk.offset_tail()
			};
		}
	}

	/**
		Get iterator at index.
	*/
	Data::Table::Iterator
	iterator_at(
		unsigned const index
	) {
		auto it = begin();
		it += min_ce(index, m_num_records);
		return it;
	}
/// @}

/** @name Modification */ /// @{
	/**
		Remove all records.
	*/
	void clear() noexcept;

	/**
		Insert a record.

		@note Fields may be morphed to other types.
	*/
	void
	insert(
		Data::Table::Iterator& it,
		unsigned num_fields,
		Data::ValueRef* const fields
	) noexcept;

	/**
		Push a record to the end of the table.

		@sa insert()
	*/
	void
	push_back(
		unsigned num_fields,
		Data::ValueRef* const fields
	) noexcept {
		auto it = end();
		insert(it, num_fields, fields);
	}

	/**
		Remove a record.
	*/
	void
	remove(
		Data::Table::Iterator& it
	) noexcept;

	/**
		Set field value.
	*/
	void
	set_field(
		Data::Table::Iterator& it,
		unsigned column_index,
		Data::ValueRef value
	);

	/**
		Get field value.
	*/
	Data::ValueRef
	get_field(
		Data::Table::Iterator const& it,
		unsigned const column_index
	) const noexcept;
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
