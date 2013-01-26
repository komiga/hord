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
enum class ErrorCode : unsigned;
enum class FieldType : uint8_t;
enum class StandardMetaFieldTypes : MetaFieldType;
enum class StandardRuleTypes : RuleType;
enum class StorageState : unsigned;

/**
	@addtogroup error
	@{
*/

/**
	Error codes.
	@sa Error
*/
enum class ErrorCode : unsigned {
/** @name General */ /// @{
	/**
		Unknown/unspecified.
	*/
	unknown,
/// @}

/** @name Driver */ /// @{
	/**
		Attempted to placehold a Hive with an empty root path.
	*/
	driver_hive_root_empty,
	/**
		Attempted to placehold a Hive with a root path that is shared with another placeheld Hive.
	*/
	driver_hive_root_shared,
	/**
		Attempted to access Hive that is locked.
	*/
	driver_hive_locked,
/// @}

/** @name Serialization */ /// @{
	/**
		Object is not in proper state.
		- When deserializing, this is due to any state other than @c StorageState::placeholder.
		- When serializing, this is due to any state other than @c StorageState::modified (will not be thrown if the serialization was triggered on a Hive).
	*/
	serialization_improper_state,
	/**
		Cannot access data.
		When object data cannot be accessed.
	*/
	serialization_access,
/// @}
};

/** @} */ // end of doc-group error

/**
	@addtogroup data
	@{
*/

/**
	Field type.
	@note The bitflag layout is only for Rule::type_info to restrict use; Fields must use a single value.
	@sa Field
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
	@sa MetaField, Metadata
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
	@addtogroup node
	@{
*/
/**
	@addtogroup rule
	@{
*/

/**
	Standard Rule types.
	@note Values in @c [0,8] are reserved for standard types (@c 0 is invalid). Userspace may specify further types, and unrecognized types should be ignored by a serializer.
	@sa Rule
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

/** @} */ // end of doc-group rule
/** @} */ // end of doc-group node

/**
	@addtogroup driver
	@{
*/
/**
	@addtogroup serialization
	@{
*/

/**
	Object storage state.
*/
enum class StorageState : unsigned {
	/**
		Null/invalid object.
		Object has no identifying information, a state in which neither serialization nor deserialization can be performed.
		@sa OBJECT_NULL
	*/
	null,
	/**
		Placeholder.
		Object has identifying information, a state in which deserialization can be performed (but not serialization).
	*/
	placeholder,
	/**
		Matches external storage.
		Specifically, after successful serialization or deserialization.
	*/
	original,
	/**
		Modified.
		Client-side modifications not yet serialized.
	*/
	modified,
};

/** @} */ // end of doc-group driver
/** @} */ // end of doc-group serialization

} // namespace Hord

#endif // HORD_COMMON_ENUMS_HPP_
