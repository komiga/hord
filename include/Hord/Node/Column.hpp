/**
@file Node/Column.hpp
@brief Column class.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_NODE_COLUMN_HPP_
#define HORD_NODE_COLUMN_HPP_

#include <Hord/config.hpp>
#include <Hord/aux.hpp>
#include <Hord/String.hpp>
#include <Hord/serialization.hpp>
#include <Hord/Rule/Defs.hpp>

namespace Hord {
namespace Node {

// Forward declarations
struct Column;

/**
	@addtogroup object
	@{
*/
/**
	@addtogroup node
	@{
*/
/**
	@addtogroup column
	@{
*/

/**
	Node layout column.
*/
struct Column final {
public:
	/** Rule::ID vector. */
	using rule_id_vector_type = aux::vector<Rule::ID>;

/** @name Properties */ /// @{
	/** Rule type. */
	Rule::Type type{static_cast<Rule::Type>(Rule::StandardTypes::None)};

	/** Title. */
	String title{};

	/** Attached rules. */
	rule_id_vector_type rules{};
/// @}

private:
	Column(Column const&) = delete;
	Column& operator=(Column const&) = delete;

public:
/** @name Constructor and destructor */ /// @{
	/** Default constructor. */
	Column() = default;
	/** Move constructor. */
	Column(Column&&) = default;
	/** Destructor. */
	~Column() noexcept = default;
/// @}

/** @name Operators */ /// @{
	/** Move assignment operator. */
	Column& operator=(Column&&) = default;
/// @}

/** @name Serialization */ /// @{
	/**
		Read from input serializer.

		@note State will be retained if an exception is thrown.

		@throws SerializerError{..}
		If a serialization operation failed.

		@param ser Input serializer.
	*/
	ser_result_type
	read(
		ser_tag_read,
		InputSerializer& ser
	);

	/**
		Write to output serializer.

		@note State will be retained if an exception is thrown.

		@throws SerializerError{..}
		If a serialization operation failed.

		@param ser Output serializer.
	*/
	ser_result_type
	write(
		ser_tag_write,
		OutputSerializer& ser
	) const;
/// @}
};

/** @} */ // end of doc-group column
/** @} */ // end of doc-group node
/** @} */ // end of doc-group object

} // namespace Node
} // namespace Hord

#endif // HORD_NODE_COLUMN_HPP_
