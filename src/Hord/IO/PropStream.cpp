
#include <Hord/IO/PropStream.hpp>

#include <cassert>
#include <istream>
#include <ostream>

namespace Hord {
namespace IO {

// class InputPropStream implementation

InputPropStream::InputPropStream(
	IO::Datastore& datastore,
	IO::PropInfo info
)
	: base(datastore, std::move(info))
	, m_stream(nullptr)
{}

InputPropStream::InputPropStream(InputPropStream&&) noexcept = default;
InputPropStream::~InputPropStream() noexcept = default;

std::istream&
InputPropStream::get_stream() {
	assert(nullptr != m_stream);
	return *m_stream;
}

void
InputPropStream::acquire() {
	assert(nullptr == m_stream);
	m_stream = &(m_datastore.acquire_input_stream(m_info));
}

void
InputPropStream::release() {
	if (nullptr != m_stream) {
		m_datastore.release_input_stream(m_info);
		m_stream = nullptr;
	}
}


// class OutputPropStream implementation

OutputPropStream::OutputPropStream(
	IO::Datastore& datastore,
	IO::PropInfo info
)
	: base(datastore, std::move(info))
	, m_stream(nullptr)
{}

OutputPropStream::OutputPropStream(OutputPropStream&&) noexcept = default;
OutputPropStream::~OutputPropStream() noexcept = default;

std::ostream&
OutputPropStream::get_stream() {
	assert(nullptr != m_stream);
	return *m_stream;
}

void
OutputPropStream::acquire() {
	assert(nullptr == m_stream);
	m_stream = &(m_datastore.acquire_output_stream(m_info));
}

void
OutputPropStream::release() {
	if (nullptr != m_stream) {
		m_datastore.release_output_stream(m_info);
		m_stream = nullptr;
	}
}

} // namespace IO
} // namespace Hord
