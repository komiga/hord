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
#include "./Object.hpp"

// TODO: Standard Rule classes

namespace Hord {

// Forward declarations
class RuleState;
class Rule;

/**
	@addtogroup object
	@{
*/
/**
	@addtogroup rule
	@{
*/

/**
	Base Rule state.
*/
class RuleState {
private:
	RuleState(RuleState const&)=delete;
	RuleState& operator=(RuleState const&)=delete;

protected:
/** @name Constructors and destructor */ /// @{
	/** Default constructor. */
	RuleState() noexcept;
	/** Move constructor. */
	RuleState(RuleState&&) noexcept;
public:
	/** Destructor. */
	virtual ~RuleState() noexcept=0;
/// @}

protected:
/** @name Operators */ /// @{
	/** Move assignment operator. */
	RuleState& operator=(RuleState&&) noexcept;
/// @}
};

/**
	Base rule.
*/
class Rule
	: public Object {
public:
	/**
		Type info.
	*/
	struct type_info final {
		/**
			%Rule type.

			@sa StandardRuleTypes
		*/
		RuleType const type;
		/**
			Permitted FieldTypes.

			@note This should be a nonzero combination of FieldTypes.
			@sa FieldType
		*/
		uint8_t const permitted_types;
		// TODO: Documentation on construct()
		/**
			Construct a rule of this type.
		*/
		Rule* (&construct)(HiveID const owner, RuleID const id);
	};

	using Object::ensure_traits;

private:
	Rule()=delete;
	Rule(Rule const&)=delete;
	Rule& operator=(Rule const&)=delete;

	Object::type_info const& get_type_info_impl() const noexcept override;

protected:
/** @name Implementation */ /// @{
	/**
		get_rule_type_info() implementation.
	*/
	virtual type_info const& get_rule_type_info_impl() const noexcept=0;
/// @}

protected:
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
	Rule(HiveID const owner, RuleID const id) noexcept
		: Object{
			(OBJECT_NULL==id)
				? StorageState::null
				: StorageState::placeholder
			, ObjectID{owner}
			, ObjectID{id}
		}
	{}
	/** Move constructor. */
	Rule(Rule&&);
public:
	/** Destructor. */
	virtual ~Rule() noexcept override=0;
/// @}

protected:
/** @name Operators */ /// @{
	/** Move assignment operator. */
	Rule& operator=(Rule&&);
/// @}

public:
/** @name Properties */ /// @{
	/**
		Get type info.
		@returns The rule's type info.
	*/
	type_info const& get_rule_type_info() const noexcept
		{ return get_rule_type_info_impl(); }
/// @}
};

/** @} */ // end of doc-group rule
/** @} */ // end of doc-group object

} // namespace Hord

#endif // HORD_RULE_HPP_
