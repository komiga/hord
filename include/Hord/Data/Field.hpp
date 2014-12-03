/**
@file
@brief Field struct.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#pragma once

#include <Hord/config.hpp>
#include <Hord/String.hpp>
#include <Hord/Data/Defs.hpp>
#include <Hord/Rule/State.hpp>

#include <duct/cc_unique_ptr.hpp>

namespace Hord {
namespace Data {

// Forward declarations
struct Field;

/**
	@addtogroup data
	@{
*/

/**
	Data field.
*/
struct Field final {
public:
/** @name Properties */ /// @{
	/** Value. */
	Data::FieldValue value;

	/** Rule state. */
	duct::cc_unique_ptr<Rule::State> state{nullptr}; // Runtime
/// @}

private:
	Field& operator=(Field const&) = delete;

public:
/** @name Special member functions */ /// @{
	/** Destructor. */
	~Field() noexcept = default;

	/** Default constructor. */
	Field() = default;
	/** Move constructor. */
	Field(Field&&) = default;
	/** Move assignment operator. */
	Field& operator=(Field&&) = default;

	/**
		Copy constructor.

		@note Field will have no rule state.
	*/
	Field(
		Field const& other
	) noexcept
		: value(other.value)
		, state(nullptr)
	{}
/// @}
};

/** @} */ // end of doc-group data

} // namespace Data
} // namespace Hord
