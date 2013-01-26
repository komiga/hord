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

#include <functional>

// TODO: Rule classes

namespace Hord {

// Forward declarations
struct RuleState;
class Rule;
class Hive; // ext

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
	RuleState(RuleState&&)=delete;
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
class Rule {
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
	mutable StorageState m_storage_state{StorageState::null}; // Runtime
	std::reference_wrapper<Hive> m_owner; // Runtime
	RuleID m_id{OBJECT_NULL};

	Rule()=delete;
	Rule(Rule const&)=delete;
	Rule& operator=(Rule const&)=delete;

public:
/** @name Constructors and destructor */ /// @{
	/**
		Constructor with owner and ID.
		@post @code get_storage_state()==(OBJECT_NULL==get_id() ? StorageState::null : StorageState::placeholder) @endcode
		@param owner Owner.
		@param id ID.
	*/
	Rule(Hive& owner, RuleID id)
		: m_storage_state{(OBJECT_NULL==id) ? StorageState::null : StorageState::placeholder}
		, m_owner{owner}
		, m_id{id}
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
		Get storage state.
		@returns Current storage state.
	*/
	StorageState get_storage_state() const noexcept { return m_storage_state; }

	/**
		Get owner.
		@returns Current owner.
	*/
	Hive& get_owner() const noexcept { return m_owner.get(); }

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
	virtual type_info const& get_type_info_impl() const noexcept=0;
};
inline Rule::~Rule()=default;

/** @} */ // end of doc-group rule
/** @} */ // end of doc-group node

} // namespace Hord

#endif // HORD_RULE_HPP_
