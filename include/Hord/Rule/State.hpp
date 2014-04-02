/**
@file Rule/State.hpp
@brief Rule State class.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_RULE_STATE_HPP_
#define HORD_RULE_STATE_HPP_

#include <Hord/config.hpp>

namespace Hord {
namespace Rule {

// Forward declarations
class State;

/**
	@addtogroup object
	@{
*/
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
/** @} */ // end of doc-group object

} // namespace Rule
} // namespace Hord

#endif // HORD_RULE_STATE_HPP_
