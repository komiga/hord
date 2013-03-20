
#include <Hord/PropStream.hpp>

#include <cassert>
#include <istream>
#include <ostream>

namespace Hord {

// class InputPropStream implementation

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

InputPropStream::InputPropStream(
	Datastore& datastore,
	PropInfo info
)
	: m_datastore{datastore}
	, m_info(std::move(info))
{
	this->acquire();
}

InputPropStream::InputPropStream(
	InputPropStream&&
) noexcept=default;

InputPropStream::~InputPropStream() {
	this->release();
}

std::istream& InputPropStream::get_stream() {
	assert(nullptr!=m_stream);
	return *m_stream;
}

// class OutputPropStream implementation

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

OutputPropStream::OutputPropStream(
	Datastore& datastore,
	PropInfo info
)
	: m_datastore{datastore}
	, m_info(std::move(info))
{
	this->acquire();
}

OutputPropStream::OutputPropStream(
	OutputPropStream&&
) noexcept=default;

OutputPropStream::~OutputPropStream() {
	this->release();
}

std::ostream& OutputPropStream::get_stream() {
	assert(nullptr!=m_stream);
	return *m_stream;
}

} // namespace Hord
