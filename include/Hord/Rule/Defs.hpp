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
enum class UnitType : Object::TypeValue;
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
using ID = Object::GenID<Object::BaseType::Rule>;

static constexpr Rule::ID const
/**
	Null ID.
*/
ID_NULL{Object::ID_NULL};

/**
	Rule unit type.

	@sa Rule::Unit
*/
enum class UnitType : Object::TypeValue {
	/**
		Typeless.

		@note This defines the <em>lack</em> of rule/structure.
	*/
	null = 0,

	/**
		Composition of types.
	*/
	Composition,

	/**
		Delimited set of rules.
	*/
	DelimitedSet,

	/**
		Value matcher.
	*/
	Match,

	/**
		Number limiter.
	*/
	Limit,

	/**
		List.
	*/
	List,

	/**
		Date-time.
	*/
	DateTime,

	/**
		Timespan.
	*/
	Timespan
};

} // namespace Rule

/** @cond INTERNAL */
namespace Object {
template<>
struct unit_type_traits<Rule::UnitType>
	: public Object::unit_type_traits_impl<
		Rule::UnitType,
		Object::BaseType::Rule
	>
{};
}
/** @endcond */ // INTERNAL

namespace Rule {

/**
	Rule object type.
*/
using Type = Object::GenType<Rule::UnitType>;

/**
	Rule type info.
*/
struct type_info final
	: public Object::type_info
{
	/**
		Permitted field types.

		@note This should be a nonzero combination of
		Data::FieldTypes.

		@sa Data::FieldType
	*/
	std::uint8_t const permitted_types;

	/**
		Constructor with properties.
	*/
	constexpr
	type_info(
		Object::type_info const& tinfo,
		std::uint8_t const permitted_types
	)
		: Object::type_info(tinfo)
		, permitted_types(permitted_types)
	{}
};

/** @} */ // end of doc-group rule
/** @} */ // end of doc-group object

} // namespace Rule
} // namespace Hord

#endif // HORD_RULE_DEFS_HPP_
