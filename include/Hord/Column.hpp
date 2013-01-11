/**
@file Column.hpp
@brief Column class.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license; see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_COLUMN_HPP_
#define HORD_COLUMN_HPP_

#include "./config.hpp"
#include "./common_types.hpp"
#include "./common_enums.hpp"
#include "./fwd.hpp"
#include "./aux.hpp"
#include "./String.hpp"

namespace Hord {

// Forward declarations
struct Column;

/**
	@addtogroup node
	@{
*/

/**
	Node layout column.
*/
struct Column final {
	/** RuleID vector. */
	typedef aux::vector<RuleID> rule_id_vector_type;

/** @name Properties */ /// @{
	/** Rule type. */
	RuleType m_type{static_cast<RuleType>(StandardRuleTypes::None)};
	/** Title. */
	String m_title{};
	/** Attached rules. */
	rule_id_vector_type m_rules{};
/// @}

private:
	Column& operator=(Column const&)=delete;
	Column& operator=(Column&&)=delete;

public:
/** @name Constructor and destructor */ /// @{
	/** Constructor. */
	Column()=default;
	/** Copy constructor. */
	Column(Column const&)=default;
	/** Move constructor. */
	Column(Column&&)=default;
	/** Destructor. */
	~Column()=default;
/// @}
};

/** @} */ // end of doc-group node

} // namespace Hord

#endif // HORD_COLUMN_HPP_
