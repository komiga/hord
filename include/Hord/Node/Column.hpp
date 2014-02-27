/**
@file Node/Column.hpp
@brief Column class.

@author Tim Howard
@copyright 2013-2014 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_NODE_COLUMN_HPP_
#define HORD_NODE_COLUMN_HPP_

#include <Hord/config.hpp>
#include <Hord/aux.hpp>
#include <Hord/String.hpp>
#include <Hord/Rule/Defs.hpp>

#include <iosfwd>

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
		Deserialize from stream.

		@note State will be retained if an exception is thrown.

		@throws Error{ErrorCode::serialization_io_failed}
		If an input operation failed.

		@param stream Input stream.
	*/
	void
	deserialize(
		std::istream& stream
	);

	/**
		Serialize to stream.

		@note State will be retained if an exception is thrown.

		@throws Error{ErrorCode::serialization_io_failed}
		If an input operation failed.

		@param stream Output stream.
	*/
	void
	serialize(
		std::ostream& stream
	) const;
/// @}
};

/** @} */ // end of doc-group column
/** @} */ // end of doc-group node
/** @} */ // end of doc-group object

} // namespace Node
} // namespace Hord

#endif // HORD_NODE_COLUMN_HPP_
