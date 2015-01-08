/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <Hord/IO/Datastore.hpp>
#include <Hord/IO/PropStream.hpp>

#include <cassert>
#include <istream>
#include <ostream>

namespace Hord {
namespace IO {

// class InputPropStream implementation

InputPropStream::~InputPropStream() noexcept = default;
InputPropStream::InputPropStream(InputPropStream&&) noexcept = default;

InputPropStream::InputPropStream(
	IO::Datastore& datastore,
	IO::PropInfo info
)
	: base(datastore, std::move(info))
	, m_stream(nullptr)
{}

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
InputPropStream::acquire_weak(
	std::istream& stream
) noexcept {
	assert(nullptr == m_stream);
	m_stream = &stream;
	m_weak = true;
}

void
InputPropStream::release() {
	if (nullptr != m_stream) {
		m_stream = nullptr;
		m_datastore.release_input_stream(m_info);
	}
}


// class OutputPropStream implementation

OutputPropStream::~OutputPropStream() noexcept = default;
OutputPropStream::OutputPropStream(OutputPropStream&&) noexcept = default;

OutputPropStream::OutputPropStream(
	IO::Datastore& datastore,
	IO::PropInfo info
)
	: base(datastore, std::move(info))
	, m_stream(nullptr)
{}

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
OutputPropStream::acquire_weak(
	std::ostream& stream
) noexcept {
	assert(nullptr == m_stream);
	m_stream = &stream;
	m_weak = true;
}

void
OutputPropStream::release() {
	if (nullptr != m_stream) {
		m_stream = nullptr;
		m_datastore.release_output_stream(m_info);
	}
}

} // namespace IO
} // namespace Hord
