/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <Hord/Rule/State.hpp>

namespace Hord {
namespace Rule {

// class State implementation

State::~State() noexcept = default;

State::State() noexcept = default;
State::State(State&&) noexcept = default;
State& State::operator=(State&&) noexcept = default;

} // namespace Rule
} // namespace Hord
