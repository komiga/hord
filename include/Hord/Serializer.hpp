/**
@file Serializer.hpp
@brief Serializer class.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license; see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_SERIALIZER_HPP_
#define HORD_SERIALIZER_HPP_

#include "./config.hpp"
#include "./String.hpp"
#include "./Rule.hpp"
#include "./Node.hpp"
#include "./Hive.hpp"

namespace Hord {

// Forward declarations
class Serializer;

/**
	@addtogroup driver
	@{
*/
/**
	@addtogroup serialization
	@{
*/

/**
	Base object serializer.
*/
class Serializer {
private:
	Serializer(Serializer const&)=delete;
	Serializer& operator=(Serializer const&)=delete;

public:
/** @name Constructors and destructor */ /// @{
	/** Default constructor. */
	Serializer()=default;
	/** Move constructor. */
	Serializer(Serializer&&)=default;
	/** Destructor. */
	inline virtual ~Serializer()=0;
/// @}

/** @name Operators */ /// @{
	/** Move assignment operator. */
	Serializer& operator=(Serializer&&)=default;
/// @}

/** @name Serialization */ /// @{
	/**
		Serialize Hive.
		@throws Error @c ErrorCode::serialization_access: If data for @a hive could not be accessed.
		@param hive Hive to serialize.
	*/
	void serialize_hive(Hive const& hive) { serialize_hive_impl(hive); }
	/**
		Serialize Node.
		@throws Error @c ErrorCode::serialization_improper_state: If @a node does not have @c StorageState::placeholder.
		@throws Error @c ErrorCode::serialization_access: If data for @a node could not be accessed.
		@param node Node to serialize.
	*/
	void serialize_node(Node const& node) { serialize_node_impl(node); }
	/**
		Serialize Rule.
		@throws Error @c ErrorCode::serialization_improper_state: If @a rule does not have @c StorageState::placeholder.
		@throws Error @c ErrorCode::serialization_access: If data for @a rule could not be accessed.
		@param rule Rule to serialize.
	*/
	void serialize_rule(Rule const& rule) { serialize_rule_impl(rule); }
/// @}

/** @name Deserialization */ /// @{
	/**
		Deserialize Hive.
		@throws Error @c ErrorCode::serialization_access: If data for @a hive could not be accessed.
		@param hive Hive to deserialize into.
	*/
	void deserialize_hive(Hive& hive) { deserialize_hive_impl(hive); }
	/**
		Deserialize Node.
		@throws Error @c ErrorCode::serialization_improper_state: If @a node does not have @c StorageState::modified.
		@throws Error @c ErrorCode::serialization_access: If data for @a node could not be accessed.
		@param node Node to deserialize into.
	*/
	void deserialize_node(Node& node) { deserialize_node_impl(node); }
	/**
		Deserialize Rule.
		@throws Error @c ErrorCode::serialization_improper_state: If @a rule does not have @c StorageState::modified.
		@throws Error @c ErrorCode::serialization_access: If data for @a rule could not be accessed.
		@param rule Rule to deserialize into.
	*/
	void deserialize_rule(Rule& rule) { deserialize_rule_impl(rule); }
/// @}

private:
	virtual void serialize_hive_impl(Hive const& hive)=0;
	virtual void serialize_node_impl(Node const& node)=0;
	virtual void serialize_rule_impl(Rule const& rule)=0;
	virtual void deserialize_hive_impl(Hive& hive)=0;
	virtual void deserialize_node_impl(Node& node)=0;
	virtual void deserialize_rule_impl(Rule& rule)=0;
};
inline Serializer::~Serializer()=default;

/** @} */ // end of doc-group serialization
/** @} */ // end of doc-group driver

} // namespace Hord

#endif // HORD_SERIALIZER_HPP_
