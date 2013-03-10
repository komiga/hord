/**
@file Rule.hpp
@brief Rule classes.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_RULE_HPP_
#define HORD_RULE_HPP_

#include "./config.hpp"
#include "./common_types.hpp"
#include "./common_enums.hpp"
#include "./String.hpp"
#include "./Object.hpp"

// TODO: Rule classes

namespace Hord {

// Forward declarations
struct RuleState;
class Rule;

/**
	@addtogroup node
	@{
*/
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

public:
/** @name Constructors and destructor */ /// @{
	/** Constructor. */
	RuleState()=default;
	/** Move constructor. */
	RuleState(RuleState&&)=default;
	/** Destructor. */
	inline virtual ~RuleState()=0;
/// @}

/** @name Operators */ /// @{
	/** Move assignment operator. */
	RuleState& operator=(RuleState&&)=default;
/// @}
};
inline RuleState::~RuleState()=default;

/**
	Base rule.
*/
class Rule
	: public Object {
public:
	friend class Driver;
	/**
		Type info.
	*/
	struct type_info final {
		/**
			Rule type.

			@sa StandardRuleTypes
		*/
		RuleType const type;
		/**
			Permitted FieldTypes.

			@note This should be a nonzero combination of FieldTypes.
			@sa FieldType
		*/
		uint8_t const permitted_types;
	};

private:
	Rule()=delete;
	Rule(Rule const&)=delete;
	Rule& operator=(Rule const&)=delete;

public:
/** @name Constructors and destructor */ /// @{
	/**
		Constructor with owner and ID.

		@post
		@code
			get_storage_state()
			==(OBJECT_NULL==get_id()
				? StorageState::null
				: StorageState::placeholder
			)
		@endcode
		@param owner Owner.
		@param id ID.
	*/
	Rule(HiveID owner, RuleID id)
		: Object{
			(OBJECT_NULL==id)
				? StorageState::null
				: StorageState::placeholder
			, ObjectID{owner}
			, ObjectID{id}
		}
	{}
	/** Move constructor. */
	Rule(Rule&&)=default;
	/** Destructor. */
	inline virtual ~Rule()=0;
/// @}

/** @name Operators */ /// @{
	/** Move assignment operator. */
	Rule& operator=(Rule&&)=default;
/// @}

/** @name Properties */ /// @{
	/**
		Get type info.
		@returns The rule's type info.
	*/
	type_info const& get_type_info() const noexcept
		{ return get_type_info_impl(); }
/// @}

private:
	virtual type_info const& get_type_info_impl() const noexcept=0;
};
inline Rule::~Rule()=default;

/** @} */ // end of doc-group rule
/** @} */ // end of doc-group node

} // namespace Hord

#endif // HORD_RULE_HPP_
