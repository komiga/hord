
#include <Hord/PropStream.hpp>
#include <Hord/Datastore.hpp>

#include <cassert>
#include <istream>
#include <ostream>

namespace Hord {

// class InputPropStream implementation

InputPropStream::InputPropStream(
	Datastore& datastore,
	PropInfo info
)
	: m_datastore{datastore}
	, m_info(std::move(info))
{}

InputPropStream::InputPropStream(
	InputPropStream&&
) noexcept=default;

InputPropStream::~InputPropStream() noexcept=default;

std::istream& InputPropStream::get_stream() {
	assert(nullptr!=m_stream);
	return *m_stream;
}

void InputPropStream::acquire() {
	assert(nullptr==m_stream);
	m_stream=&(m_datastore.acquire_input_stream(m_info));
}

void InputPropStream::release() {
	if (nullptr!=m_stream) {
		m_datastore.release_input_stream(m_info);
		m_stream=nullptr;
	}
}

// class OutputPropStream implementation

OutputPropStream::OutputPropStream(
	Datastore& datastore,
	PropInfo info
)
	: m_datastore{datastore}
	, m_info(std::move(info))
{}

OutputPropStream::OutputPropStream(
	OutputPropStream&&
) noexcept=default;

OutputPropStream::~OutputPropStream() noexcept=default;

std::ostream& OutputPropStream::get_stream() {
	assert(nullptr!=m_stream);
	return *m_stream;
}

void OutputPropStream::acquire() {
	assert(nullptr==m_stream);
	m_stream=&(m_datastore.acquire_output_stream(m_info));
}

void OutputPropStream::release() {
	if (nullptr!=m_stream) {
		m_datastore.release_output_stream(m_info);
		m_stream=nullptr;
	}
}

} // namespace Hord
