/**
@file MessageBuffer.hpp
@brief MessageBuffer class.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_MESSAGEBUFFER_HPP_
#define HORD_MESSAGEBUFFER_HPP_

#include "./config.hpp"
#include "./common_types.hpp"
#include "./aux.hpp"

#include <type_traits>

namespace Hord {

// Forward declarations
class MessageBuffer;

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
class MessageBuffer final {
public:
	/**
		Buffer type.
	*/
	using buffer_type=aux::vector<uint8_t>;

	/**
		Message header.
	*/
	struct msg_header final {
		MessageID id; /**< Message ID. */
		uint32_t size; /**< Message size. */
	};

private:
	buffer_type m_buffer{};

	MessageBuffer()=delete;
	MessageBuffer(MessageBuffer const&)=delete;
	MessageBuffer& operator=(MessageBuffer const&)=delete;

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
	MessageBuffer(
		std::size_t const capacity
	);
	/** Move constructor. */
	MessageBuffer(MessageBuffer&&);
	/** Destructor. */
	~MessageBuffer() noexcept;
/// @}

/** @name Operators */ /// @{
	/** Move assignment operator. */
	MessageBuffer& operator=(MessageBuffer&&);
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
		push_back(id, 0, nullptr);
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
	template<typename MsgT>
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

} // namespace Hord

#endif // HORD_MESSAGEBUFFER_HPP_
