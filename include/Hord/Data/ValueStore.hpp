/**
@file
@brief ValueStore class.
@ingroup data

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#pragma once

#include <Hord/config.hpp>
#include <Hord/aux.hpp>
#include <Hord/utility.hpp>
#include <Hord/serialization.hpp>
#include <Hord/Data/Defs.hpp>

#include <duct/debug.hpp>

namespace Hord {
namespace Data {

// Forward declarations
class ValueStore;

/**
	@addtogroup data
	@{
*/

/**
	Value store.
*/
class ValueStore {
public:
	struct Chunk {
		std::uint8_t* data{nullptr};
		std::uint8_t* head{nullptr};
		std::uint8_t* tail{nullptr};
		unsigned size{0};
		unsigned num_values{0};

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
		ValueStore* value_store;
		unsigned index;
		unsigned chunk_index;
		unsigned inner_index;
		unsigned value_offset;

		bool
		operator==(
			Iterator const& rhs
		) const noexcept {
			return value_store == rhs.value_store && index == rhs.index;
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
			return value_store && index < value_store->m_num_values;
		}

		/**
			Remove value.
		*/
		void
		insert(
			Data::ValueRef value
		) {
			value_store->set_value(*this, value, true);
		}

		/**
			Remove value.
		*/
		void
		remove() {
			value_store->remove(*this);
		}

		/**
			Set value.
		*/
		void
		set_value(
			Data::ValueRef value
		) {
			value_store->set_value(*this, value, false);
		}

		/**
			Get value.
		*/
		Data::ValueRef
		get_value() {
			return value_store->get_value(*this);
		}
	};

	using chunk_vector_type = aux::vector<Chunk>;

private:
	Data::Type m_type{};
	unsigned m_num_values{0u};
	chunk_vector_type m_chunks{};

	ValueStore(ValueStore const&) = delete;
	ValueStore& operator=(ValueStore const&) = delete;

public:
/** @name Special member functions */ /// @{
	/** Destructor. */
	~ValueStore() noexcept;

	/** Default constructor. */
	ValueStore() = default;
	/** Move constructor. */
	ValueStore(ValueStore&&) = default;
	/** Move assignment operator. */
	ValueStore& operator=(ValueStore&&) = default;

	/**
		Constructor with type.
	*/
	explicit
	ValueStore(
		Data::Type const type
	) noexcept;
/// @}

/** @name Properties */ /// @{
	/**
		Get type.
	*/
	Data::Type
	type() const noexcept {
		return m_type;
	}

	/**
		Set type.

		@returns @c true if the type changed.
	*/
	bool
	set_type(
		Data::Type const type
	) noexcept;
/// @}

/** @name Values */ /// @{
	/**
		Get beginning iterator.
	*/
	Data::ValueStore::Iterator
	begin() {
		if (m_chunks.empty()) {
			return {this, m_num_values, 0, 0, 0};
		} else {
			auto const& chunk = m_chunks.front();
			return {this, 0, 0, 0, chunk.offset_head()};
		}
	}

	/**
		Get ending iterator.
	*/
	Data::ValueStore::Iterator
	end() {
		if (m_chunks.empty()) {
			return {this, m_num_values, 0, 0, 0};
		} else {
			auto const& chunk = m_chunks.back();
			return {
				this,
				m_num_values,
				static_cast<unsigned>(m_chunks.size() - 1),
				chunk.num_values,
				chunk.offset_tail()
			};
		}
	}

	/**
		Get iterator at index.
	*/
	Data::ValueStore::Iterator
	iterator_at(
		unsigned const index
	) {
		auto it = begin();
		it += min_ce(index, m_num_values);
		return it;
	}

	/**
		Zero all values.
	*/
	void
	zero() noexcept;

	/**
		Empty all chunks.
	*/
	void
	clear() noexcept;

	/**
		Free all chunks.
	*/
	void
	free() noexcept;

	/**
		Resize.
	*/
	void
	resize(
		unsigned const size
	) noexcept;

private:
	void
	set_value(
		Data::ValueStore::Iterator& it,
		Data::ValueRef& value,
		bool const insert
	);

public:
	/**
		Insert value.
	*/
	void
	insert(
		Data::ValueStore::Iterator& it,
		Data::ValueRef value
	) {
		set_value(it, value, true);
	}

	/**
		Remove value.
	*/
	void
	remove(
		Data::ValueStore::Iterator& it
	);

	/**
		Set value.
	*/
	void
	set_value(
		Data::ValueStore::Iterator& it,
		Data::ValueRef value
	) {
		set_value(it, value, false);
	}

	/**
		Get value.
	*/
	Data::ValueRef
	get_value(
		Data::ValueStore::Iterator const& it
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
