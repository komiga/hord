/**
@file common_enums.hpp
@brief Common enums.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license; see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_COMMON_ENUMS_HPP_
#define HORD_COMMON_ENUMS_HPP_

#include "./config.hpp"
#include "./common_types.hpp"

namespace Hord {

// Forward declarations
enum class FieldType : uint8_t;
enum class StandardMetaFieldTypes : MetaFieldType;
enum class StandardRuleTypes : RuleType;

/**
	@addtogroup data
	@{
*/

/**
	Field type.
	@note The bitflag layout is only for Rule::type_info to restrict use; Fields must use a single value.
	@sa #Field
*/
enum class FieldType : uint8_t {
	/** Textual field. */
	Text	=1<<0,
	/** Numeric field. */
	Number	=1<<1,
	/** Boolean field. */
	Boolean	=1<<2
};

/** @} */ // end of doc-group data

/**
	@addtogroup metadata
	@{
*/

/**
	Standard MetaField types.
	@note Values in @c [0x00, 0x08] are reserved for standard types (@c 0x00 is invalid). Custom serialization may specify further types, and unrecognized types should be ignored by a serializer.
	@sa #MetaField, #Metadata
*/
enum class StandardMetaFieldTypes : MetaFieldType {
	/** StringMetaField. */
	String=0x01,
	/** Int32MetaField. */
	Int32,
	/** Int64MetaField. */
	Int64,
	/** BoolMetaField. */
	Bool,
	/// @{
	/** Reserved types (@c 0x05 to @c 0x08). */
	Reserved1,
	Reserved2,
	Reserved3,
	Reserved4,
	ReservedFirst=Reserved1,
	ReservedLast=Reserved4
	/// @}
};
static_assert(0x08==static_cast<MetaFieldType>(StandardMetaFieldTypes::ReservedLast), "StandardMetaFieldTypes must reserve up to 0x08");

/** @} */ // end of doc-group metadata

/**
	@addtogroup column
	@{
*/

/**
	Standard Rule types.
	@note Values in @c [0,8] are reserved for standard types (@c 0 is invalid). Userspace may specify further types, and unrecognized types should be ignored by a serializer.
	@sa #Rule
*/
enum class StandardRuleTypes : RuleType {
	/**
		Special non-type.
		@note This type defines the <em>lack</em> of rule/structure. Rules cannot be registered with this type.
	*/
	None=0,
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
	/** Reserved types (@c 8). */
	Reserved1,
	ReservedFirst=Reserved1,
	ReservedLast=Reserved1
	/// @}
};
static_assert(8==static_cast<RuleType>(StandardRuleTypes::ReservedLast), "StandardRuleTypes must reserve up to 8");

/** @} */ // end of doc-group column

} // namespace Hord

#endif // HORD_COMMON_ENUMS_HPP_
