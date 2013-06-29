/**
@file Rule/State.hpp
@brief Rule State class.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
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

protected:
/** @name Constructors and destructor */ /// @{
	/** Default constructor. */
	State() noexcept;
	/** Move constructor. */
	State(State&&) noexcept;
public:
	/** Destructor. */
	virtual
	~State() noexcept = 0;
/// @}

protected:
/** @name Operators */ /// @{
	/** Move assignment operator. */
	State& operator=(State&&) noexcept;
/// @}
};

/** @} */ // end of doc-group rule
/** @} */ // end of doc-group object

} // namespace Rule
} // namespace Hord

#endif // HORD_RULE_STATE_HPP_
