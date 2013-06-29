
#include <Hord/Msg/Buffer.hpp>

namespace Hord {
namespace Msg {

// class Buffer implementation

void
Buffer::grow(
	std::size_t const new_size
) {
	if (new_size > get_capacity()) {
		m_buffer.reserve(new_size + 512u);
	}
	m_buffer.resize(new_size);
}

Buffer::Buffer(
	std::size_t const capacity
)
	: m_buffer()
{
	m_buffer.reserve(capacity);
}

Buffer::Buffer(Buffer&&) = default;
Buffer::~Buffer() noexcept = default;

Buffer& Buffer::operator=(Buffer&&) = default;

void
Buffer::push_back(
	MessageID const id,
	std::size_t const size,
	void const* const data
) {
	msg_header const
	header{
		id,
		static_cast<uint32_t>(size)
	};
	auto const offset = get_size();
	grow(offset + sizeof(msg_header) + size);
	auto out_it = m_buffer.data() + offset;
	out_it = std::copy(
		reinterpret_cast<uint8_t const*>(&header),
		reinterpret_cast<uint8_t const*>(&header) + sizeof(msg_header),
		out_it
	);
	std::copy(
		static_cast<uint8_t const* const>(data),
		static_cast<uint8_t const* const>(data) + size,
		out_it
	);
}

} // namespace Msg
} // namespace Hord
