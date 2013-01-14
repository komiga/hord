/**
@file Rule.hpp
@brief Rule classes.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license; see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_RULE_HPP_
#define HORD_RULE_HPP_

#include "./config.hpp"
#include "./common_types.hpp"
#include "./common_enums.hpp"
#include "./fwd.hpp"

#include <memory>

// TODO: Rule classes

namespace Hord {

// Forward declarations
struct RuleState;
class Rule;

/**
	@addtogroup rule
	@{
*/

/**
	Base Rule state.
*/
struct RuleState {
private:
	RuleState(RuleState const&)=delete;
	RuleState& operator=(RuleState const&)=delete;
	RuleState& operator=(RuleState&&)=delete;

public:
/** @name Constructors and destructor */ /// @{
	/** Constructor. */
	RuleState()=default;
	/** Move constructor. */
	RuleState(RuleState&&)=delete;
	/** Destructor. */
	inline virtual ~RuleState()=0;
/// @}
};
/** @cond INTERNAL */
inline RuleState::~RuleState()=default;
/** @endcond */ // INTERNAL

/**
	Base rule.
*/
class Rule {
public:
	/**
		Type info.
	*/
	struct type_info final {
		/**
			Rule type.
			@sa #StandardRuleTypes
		*/
		RuleType const type;
		/**
			Permitted FieldTypes.
			@note This should be a nonzero combination of FieldTypes.
			@sa #FieldType
		*/
		uint8_t const permitted_types;
	};

private:
	Hive& m_owner; // Runtime
	RuleID m_id;

	Rule()=delete;
	Rule(Rule const&)=delete;
	Rule& operator=(Rule const&)=delete;
	Rule& operator=(Rule&&)=delete;

public:
/** @name Constructors and destructor */ /// @{
	/**
		Constructor with owner and ID.
		@param owner Owner.
		@param id ID.
	*/
	Rule(Hive& owner, RuleID const id)
		: m_owner{owner}
		, m_id{id}
	{}
	/** Move constructor. */
	Rule(Rule&&)=default;
	/** Destructor. */
	inline virtual ~Rule()=0;
/// @}

/** @name Properties */ /// @{
	/**
		Get ID.
		@returns Current ID.
	*/
	RuleID get_id() const noexcept { return m_id; }

	/**
		Get type info.
		@returns The rule's type info.
	*/
	type_info const& get_type_info() const noexcept { return get_type_info_impl(); }
/// @}

private:
	/**
		get_type_info() implementation.
		@returns The rule's type info.
	*/
	virtual type_info const& get_type_info_impl() const noexcept=0;
};
/** @cond INTERNAL */
inline Rule::~Rule()=default;
/** @endcond */ // INTERNAL

/** @} */ // end of doc-group rule

} // namespace Hord

#endif // HORD_RULE_HPP_
