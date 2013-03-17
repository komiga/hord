
#include <Hord/Serializer.hpp>

namespace Hord {

// class Serializer implementation

Serializer::Serializer() noexcept=default;
Serializer::Serializer(Serializer&&) noexcept=default;
Serializer::~Serializer() noexcept=default;

Serializer& Serializer::operator=(Serializer&&) noexcept=default;

} // namespace Hord
