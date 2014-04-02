/**
@file Rule/Defs.hpp
@brief Rule definitions.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_RULE_DEFS_HPP_
#define HORD_RULE_DEFS_HPP_

#include <Hord/config.hpp>
#include <Hord/Object/Defs.hpp>

namespace Hord {
namespace Rule {

// Forward declarations
class Unit; // external
struct type_info;

/**
	@addtogroup object
	@{
*/
/**
	@addtogroup rule
	@{
*/

/**
	Rule ID.

	@note This type is only to telegraph intent in functions;
	it is entirely equivalent to @c Object::ID.

	@sa Object::ID,
		Hive::ID,
		Node::ID
*/
using ID = Object::ID;

/**
	Rule::ID constants.
*/
enum : Rule::ID {
	/**
		Null object.
	*/
	NULL_ID = static_cast<Rule::ID>(Object::NULL_ID)
};

/**
	Rule type.
*/
using Type = std::uint32_t;

/**
	Standard rule types.

	@note Values in <code>[0, 8]</code> are reserved for standard
	types (@c 0 is invalid). Userspace may specify further types in
	the range <code>[9, (2 ^ 32) - 1]</code>.

	@sa
		Rule::Unit
*/
enum class StandardTypes : Rule::Type {
	/**
		Special non-type.

		@note This type defines the <em>lack</em> of rule/structure.
		%Rules cannot be registered with this type.
	*/
	None = 0,

	/**
		Composition of types.

		@sa CompositionRule
	*/
	Composition,

	/**
		Delimited set of rules.

		@sa DelimitedSetRule
	*/
	DelimitedSet,

	/**
		Value matcher.

		@sa MatchRule
	*/
	Match,

	/**
		Number limiter.

		@sa LimitRule
	*/
	Limit,

	/**
		List.

		@sa ListRule
	*/
	List,

	/**
		Date-time.

		@sa DateTimeRule
	*/
	DateTime,

	/**
		Timespan.

		@sa TimespanRule
	*/
	Timespan,

/// @{
	/**
		Reserved types (@c 8).
	*/
	Reserved1,
	ReservedFirst = Reserved1,
	ReservedLast = Reserved1
/// @}
};

static_assert(
	8 == static_cast<Rule::Type>(StandardTypes::ReservedLast),
	"StandardTypes must reserve up to 8"
);

/**
	Rule type info.
*/
struct type_info final {
	/**
		%Rule type.

		@sa Rule::StandardTypes
	*/
	Type const type;

	/**
		Permitted field types.

		@note This should be a nonzero combination of
		Data::FieldTypes.

		@sa Data::FieldType
	*/
	std::uint8_t const permitted_types;

	/**
		Construct a rule of this type.

		@returns
		- The constructed rule; or
		- @c nullptr if construction failed.
		@param id Object %ID.
		@param parent Parent %ID.
	*/
	Unit*
	(&construct)(
		Rule::ID const id,
		Object::ID const parent
	) noexcept;
};

/** @} */ // end of doc-group rule
/** @} */ // end of doc-group object

} // namespace Rule
} // namespace Hord

#endif // HORD_RULE_DEFS_HPP_
