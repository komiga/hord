/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Rule State class.
@ingroup rule
*/

#pragma once

#include <Hord/config.hpp>

namespace Hord {
namespace Rule {

// Forward declarations
class State;

/**
	@addtogroup rule
	@{
*/

/**
	Base rule state.
*/
class State {
private:
	State(State const&) = delete;
	State& operator=(State const&) = delete;

/** @name Special member functions */ /// @{
public:
	/** Destructor. */
	virtual
	~State() noexcept = 0;

protected:
	/** Default constructor. */
	State() noexcept;
	/** Move constructor. */
	State(State&&) noexcept;
	/** Move assignment operator. */
	State& operator=(State&&) noexcept;
/// @}
};

/** @} */ // end of doc-group rule

} // namespace Rule
} // namespace Hord
