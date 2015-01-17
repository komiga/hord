/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief TableSchema class.
@ingroup data
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
	/** @name Properties */ /// @{
		/**
			Index.

			This is only used when configuring a table.
			See @c Data::Table::configure().
		*/
		unsigned index{~0u};

		/** Type. */
		Data::Type type{};

		/** Name. */
		String name{};
	/// @}

	/** @name Special member functions */ /// @{
		/** Destructor. */
		~Column() noexcept = default;

		/** Default constructor. */
		Column() = default;
		/** Move constructor. */
		Column(Column&&) = default;
		/** Copy constructor. */
		Column(Column const&) = default;
		/** Move assignment operator. */
		Column& operator=(Column&&) = default;
		/** Copy assignment operator. */
		Column& operator=(Column const&) = default;

		/**
			Construct with name and type.
		*/
		Column(
			String name,
			Data::Type type
		) noexcept
			: type(type)
			, name(std::move(name))
		{}

		/**
			Construct with index, name, and type.
		*/
		Column(
			unsigned index,
			String name,
			Data::Type type
		) noexcept
			: index(index)
			, type(type)
			, name(std::move(name))
		{}
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
		}
	/// @}
	};

	/**
		Column vector type.
	*/
	using column_vector_type = aux::vector<Column>;

private:
	HashValue m_hash{HASH_EMPTY};
	column_vector_type m_columns{};

public:
/** @name Special member functions */ /// @{
	/** Destructor. */
	~TableSchema() noexcept = default;

	/** Default constructor. */
	TableSchema() = default;
	/** Move constructor. */
	TableSchema(TableSchema&&) = default;
	/** Copy constructor. */
	TableSchema(TableSchema const&) = default;
	/** Move assignment operator. */
	TableSchema& operator=(TableSchema&&) = default;
	/** Copy assignment operator. */
	TableSchema& operator=(TableSchema const&) = default;

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
	HashValue
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
		Get number of columns.
	*/
	unsigned
	num_columns() const noexcept {
		return m_columns.size();
	}
/// @}

/** @name Layout */ /// @{
	/**
		Get a column by index.
	*/
	Data::TableSchema::Column const&
	column(
		unsigned const index
	) const {
		return m_columns.at(index);
	}

	/**
		Update schema hash.

		@returns @c true if the hash changed.
	*/
	bool
	update() noexcept;

	/**
		Update schema hash.

		@returns @c true if a type or the number of columns changed.
	*/
	bool
	assign(
		Data::TableSchema const& schema
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
