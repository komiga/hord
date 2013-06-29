/**
@file common_enums.hpp
@brief Common enums.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_COMMON_ENUMS_HPP_
#define HORD_COMMON_ENUMS_HPP_

#include "./config.hpp"
#include "./common_types.hpp"

namespace Hord {

// Forward declarations
enum class ErrorCode : unsigned;
enum class ObjectType : unsigned;
enum class FieldType : uint8_t;
enum class MetaFieldType : uint8_t;
enum class StandardRuleTypes : RuleType;
enum class StorageState : unsigned;
enum class SerializationFlags : unsigned;
enum class PropType : unsigned;

/**
	@addtogroup error
	@{
*/

// FIXME: Doxygen borks all over itself
// when name-groups are used inside enums.
/**
	%Error codes.
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
		Attempted to register a rule type that is reserved for
		standard rules.
	*/
	driver_rule_type_reserved,
	/**
		Attempted to register a rule with permitted_types property
		equal to @c 0.
	*/
	driver_rule_type_zero_permitted_types,
	/**
		Attempted to register a rule type that has already been
		registered.
	*/
	driver_rule_type_shared,

	/**
		Attempted to placehold a hive with an empty root path.
	*/
	driver_hive_root_empty,
	/**
		Attempted to placehold a hive with a root path that is
		shared with another placeheld hive.
	*/
	driver_hive_root_shared,

	/**
		Failed to construct datastore.

		@sa Datastore::type_info
	*/
	driver_datastore_construct_failed,
	/**
		Attempted to access datastore that is locked.
	*/
	driver_datastore_locked,
/// @}

/** @name Datastore */ /// @{
	/**
		Attempted to open datastore when it was already open.
	*/
	datastore_open_already,
	/**
		Failed to open datastore.

		Implementation-defined reason.
	*/
	datastore_open_failed,

	/**
		Attempted to perform operation on datastore when it was
		closed.
	*/
	datastore_closed,
	/**
		Attempted to perform locking operation while datastore is
		locked.
	*/
	datastore_locked,
	/**
		Attempted to modify property while datastore was open.

		%Datastore properties are immutable while the datastore is
		open.
	*/
	datastore_property_immutable,

	/**
		Attempted to perform operation with an object not in the
		datastore.
	*/
	datastore_object_not_found,

	/**
		Attempted to request prop from object that does not supply
		it.
	*/
	datastore_prop_unsupplied,
	/**
		Attempted to unlock a prop that is not locked.
	*/
	datastore_prop_not_locked,
/// @}

/** @name Serialization */ /// @{
	/**
		%Object is not in proper state.

		- When deserializing, this is due to any state other
		  than @c StorageState::placeholder.
		- When serializing, this is due to any state other
		  than @c StorageState::modified (will not be thrown
		  if the serialization was triggered on a hive).
	*/
	serialization_improper_state,
	/**
		Cannot access data.

		When object data cannot be accessed.
	*/
	serialization_access,
/// @}
/** @cond INTERNAL */
	LAST
/** @endcond */
};

/** @} */ // end of doc-group error

/**
	@addtogroup object
	@{
*/

/**
	@ref object type.
*/
enum class ObjectType : unsigned {
	/** @ref hive. */
	Hive,
	/** @ref rule. */
	Rule,
	/** @ref node. */
	Node
};

/** @} */ // end of doc-group object

/**
	@addtogroup data
	@{
*/

/**
	Field type.

	@note The bitflag layout is only for Rule::type_info to
	restrict use; Fields must use a single value.
	@sa Field
*/
enum class FieldType : uint8_t {
	/** Textual field. */
	Text	= 1 << 0,
	/** Numeric field. */
	Number	= 1 << 1,
	/** Boolean field. */
	Boolean	= 1 << 2
};

/** @} */ // end of doc-group data

/**
	@addtogroup metadata
	@{
*/

/**
	MetaField type.

	@sa MetaField,
		Metadata
*/
enum class MetaFieldType : uint8_t {
	/** StringMetaField. */
	String = 0x01,
	/** Int32MetaField. */
	Int32,
	/** Int64MetaField. */
	Int64,
	/** BoolMetaField. */
	Bool
};

/** @} */ // end of doc-group metadata

/**
	@addtogroup object
	@{
*/
/**
	@addtogroup rule
	@{
*/

/**
	Standard rule types.

	@note Values in @c [0,8] are reserved for standard
	types (@c 0 is invalid). Userspace may specify further types in
	the range @c [9, (2^32)−1].
	@sa Rule
*/
enum class StandardRuleTypes : RuleType {
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
	/** Reserved types (@c 8). */
	Reserved1,
	ReservedFirst=Reserved1,
	ReservedLast=Reserved1
	/// @}
};
static_assert(
	8 == static_cast<RuleType>(StandardRuleTypes::ReservedLast),
	"StandardRuleTypes must reserve up to 8"
);

/** @} */ // end of doc-group rule
/** @} */ // end of doc-group object

/**
	@addtogroup serialization
	@{
*/

/**
	@ref object storage state.
*/
enum class StorageState : unsigned {
	/**
		Null/invalid object.

		%Object has no identifying information, a state in which
		neither serialization nor deserialization can be performed.
		@sa OBJECT_NULL
	*/
	null,
	/**
		Placeholder.

		%Object has identifying information, a state in which
		deserialization can be performed (but not serialization).

		@note An object can have this state when its metadata
		property is deserialized. Most objects only change
		to @c original when their primary data is deserialized.
	*/
	placeholder,
	/**
		Matches external storage.

		Specifically, after successful serialization or
		deserialization.

		@note For hives, this implicitly means that the runtime only
		has identifying information for all children -- not that all
		children are fully deserialized. In a typical configuration,
		children are fully (de)serialized on demand, and always
		placeheld.
	*/
	original,
	/**
		Modified.

		Runtime-side modifications not yet serialized.

		@note %Hives will only have this state when:
		-# one of its stored properties is modified (metadata or
		   slug); or
		-# when a child is added or removed.
		If a child object is modified, it does not affect the hive's
		state.
	*/
	modified,
};

/**
	Serialization flags.

	@note All of these flags apply both to serialization
	and deserialization unless otherwise stated.
*/
enum class SerializationFlags : unsigned {
	/**
		Include identifying information in operation.

		Identifying information includes Object's owner, ID, and slug
		properties. Only the owner and ID properties are required to
		deserialize an object.

		On non-hive objects, this flag will only (de)serialize the
		object's slug (the object's actual ID is supplied by the
		hive's primary data).

		Post-@c placeholder state, this flag is only significant when
		serializing a hive after a child was added or removed.

		@note This flag is implicit.
	*/
	identity = 1 << 0,
	/**
		Include metadata property in operation.
	*/
	metadata = 1 << 1,
	/**
		Include primary data in operation.

		%Hives' primary data is its idset property. A child will only
		be placeheld when its owner's primary data is deserialized.
	*/
	primary = 1 << 2,
	/**
		Shallow deserialization.

		Includes both @c identity and @c metadata.

		@remarks This flag is only used by the driver when
		deserializing placeheld objects.
	*/
	shallow
		= identity
		| metadata
	,
	/**
		All flags.
	*/
	all
		= identity
		| metadata
		| primary
	,
};

/**
	@addtogroup prop
	@{
*/

/**
	Prop type.

	@sa PropInfo,
		Datastore
*/
enum class PropType : unsigned {
	object_metadata,
	object_primary
};

/** @} */ // end of doc-group prop
/** @} */ // end of doc-group serialization

} // namespace Hord

#endif // HORD_COMMON_ENUMS_HPP_
