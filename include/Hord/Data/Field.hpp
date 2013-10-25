/**
@file Data/Field.hpp
@brief Field struct.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_DATA_FIELD_HPP_
#define HORD_DATA_FIELD_HPP_

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
	/** Type. */
	Data::FieldType type{Data::FieldType::Text};

	/** Value. */
	struct {
		String str{}; /**< String value. */
		union {
			int64_t num{0}; /**< Integer value. */
			bool bin; /**< Boolean value. */
		};
	} value{};

	/** Rule state. */
	duct::cc_unique_ptr<Rule::State> state{nullptr}; // Runtime
/// @}

private:
	Field(Field const& other) = delete;
	Field& operator=(Field const&) = delete;

public:
/** @name Constructors and destructor */ /// @{
	/** Default constructor. */
	Field() = default;
	/** Move constructor. */
	Field(Field&&) = default;
	/** Destructor. */
	~Field() noexcept = default;
/// @}

/** @name Operators */ /// @{
	/** Move assignment operator. */
	Field& operator=(Field&&) = default;
/// @}
};

/** @} */ // end of doc-group data

} // namespace Data
} // namespace Hord

#endif // HORD_DATA_FIELD_HPP_
