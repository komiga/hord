/**
@file Msg/Buffer.hpp
@brief Message buffer class.

@author Tim Howard
@copyright 2013-2014 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_MSG_BUFFER_HPP_
#define HORD_MSG_BUFFER_HPP_

#include <Hord/config.hpp>
#include <Hord/aux.hpp>
#include <Hord/Msg/Defs.hpp>

#include <type_traits>

namespace Hord {
namespace Msg {

// Forward declarations
class Buffer;

/**
	@addtogroup messaging
	@{
*/

/**
	Message buffer.

	@note The buffer is structured like so:
	@verbatim
		[msg_1_header] [msg_1_data] ... [msg_n_header] [msg_n_data]
	@endverbatim
*/
class Buffer final {
public:
	/**
		Buffer type.
	*/
	using buffer_type = aux::vector<uint8_t>;

	/**
		Message header.
	*/
	struct msg_header final {
		MessageID id; /**< Message ID. */
		uint32_t size; /**< Message size. */
	};

private:
	buffer_type m_buffer{};

	Buffer() = delete;
	Buffer(Buffer const&) = delete;
	Buffer& operator=(Buffer const&) = delete;

	void
	grow(
		std::size_t const new_size
	);

public:
/** @name Constructors and destructor */ /// @{
	/**
		Constructor with capacity.

		@param capacity Buffer capacity. Consider space used by headers.
	*/
	explicit
	Buffer(
		std::size_t const capacity
	);

	/** Move constructor. */
	Buffer(Buffer&&);
	/** Destructor. */
	~Buffer() noexcept;
/// @}

/** @name Operators */ /// @{
	/** Move assignment operator. */
	Buffer& operator=(Buffer&&);
/// @}

/** @name Properties */ /// @{
	/**
		Get buffer.

		@returns Message buffer.
	*/
	buffer_type const&
	get_buffer() const noexcept {
		return m_buffer;
	}

	/**
		Check if buffer is empty.

		@returns
		- @c true if the buffer is empty;
		- @c false if it is not.
	*/
	bool
	is_empty() const noexcept {
		return m_buffer.empty();
	}

	/**
		Get buffer size.

		@returns Current buffer size.
	*/
	std::size_t
	get_size() const noexcept {
		return m_buffer.size();
	}

	/**
		Get buffer capacity.

		@returns Current buffer capacity.
	*/
	std::size_t
	get_capacity() const noexcept {
		return m_buffer.capacity();
	}
/// @}

/** @name Operations */ /// @{
	/**
		Clear buffer.
	*/
	void
	clear();

	/**
		Push a data-less message.

		@param id Message id.
	*/
	void
	push_back(
		MessageID const id
	) {
		push_back(id, 0u, nullptr);
	}

	/**
		Push a message.

		@note Message data will be copied.

		@param id Message id.
		@param size Message size.
		@param data Message data.
	*/
	void
	push_back(
		MessageID const id,
		std::size_t const size,
		void const* const data
	);

	/**
		Push a message.

		@pre @a MsgT must satisfy PODType.

		@tparam MsgT Message class.
		@param id Message id.
		@param msg Message.
	*/
	template<
		typename MsgT
	>
	void
	push_back(
		MessageID const id,
		MsgT const& msg
	) {
		static_assert(
			std::is_pod<MsgT>::value,
			"MsgT must satisfy PODType"
		);
		this->push_back(
			id,
			sizeof(MsgT),
			static_cast<void const*>(&msg)
		);
	}
/// @}
};

/** @} */ // end of doc-group messaging

} // namespace Msg
} // namespace Hord

#endif // HORD_MSG_BUFFER_HPP_
