/**
@file Cmd/Unit.hpp
@brief Command unit.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_CMD_UNIT_HPP_
#define HORD_CMD_UNIT_HPP_

#include <Hord/config.hpp>
#include <Hord/traits.hpp>
#include <Hord/serialization.hpp>
#include <Hord/Cmd/Defs.hpp>

#include <type_traits>
#include <utility>

namespace Hord {

// Forward declarations (external)
namespace System {
class Context;
}

namespace Cmd {

// Forward declarations
class Unit;

/**
	@addtogroup cmd
	@{
*/

/**
	@name Command definition macros

	TODO

	@{
*/

/**
	Open @c make_stage() function definition.

	%Stage type switch body should follow macro (without braces).
	See Cmd::type_info for visible parameters.
*/
#define HORD_CMD_MAKE_STAGE_OPEN()				\
	static ::Hord::Cmd::StageUPtr				\
	make_stage(									\
		::Hord::Cmd::StageType const type		\
	) {											\
		switch (type) {

/**
	Make stage type switch in @c make_stage() definition.

	@param stagen_ Identifier of the stage name.
*/
#define HORD_CMD_MAKE_STAGE_CASE(stagen_)		\
		case ::Hord::Cmd::StageType::stagen_:	\
			return ::Hord::Cmd::StageUPtr{new stagen_::impl()};

/**
	Close @c make_stage() function definition.

	This requires @c HORD_CMD_TYPE_ to be defined to the command
	type for the stages.
*/
#define HORD_CMD_MAKE_STAGE_CLOSE()										\
		default:														\
			DUCT_GR_THROW_IMPL_(										\
				::Hord::ErrorCode::cmd_construct_stage_type_invalid,	\
				DUCT_GR_MSG(											\
					DUCT_GR_SCOPE_CLASS_STR "::make_stage",				\
					"stage type not implemented for command"			\
					" " HORD_STRINGIFY(HORD_CMD_TYPE_)					\
				)														\
			);															\
		}																\
	}

/**
	Define command type info.

	@param cmdn_ Command class identifier.
	@param ... Type flags.
*/
#define HORD_CMD_DEF_TYPE_INFO(cmdn_, ...)				\
	::Hord::Cmd::type_info const						\
	HORD_CMD_JOIN_CT_(HORD_CMD_TYPE_){					\
		::Hord::Cmd::Type::HORD_CMD_TYPE_,				\
		{__VA_ARGS__},									\
		{cmdn_::make},									\
		{make_stage}									\
	};

/**
	Open command definition.

	Command class body should follow macro (without braces).

	@param classn_ Class name.
*/
#define HORD_CMD_DEF_OPEN(classn_)									\
	class classn_ : public ::Hord::Cmd::Unit {						\
	private:														\
		::Hord::Cmd::type_info const&								\
		get_type_info_impl() const noexcept override {				\
			return HORD_CMD_JOIN_CT_(HORD_CMD_TYPE_);				\
		}															\
		::Hord::Cmd::UnitUPtr										\
		clone_impl() override {										\
			return ::Hord::Cmd::UnitUPtr{new classn_(*this)};		\
		}															\
	private:														\
		classn_& operator=(classn_ const&) = delete;				\
	public:															\
		~classn_() override = default;								\
		classn_() = default;										\
		classn_(classn_&&) = default;								\
		classn_& operator=(classn_&&) = default;					\
		classn_(classn_ const&) = default;							\
		static ::Hord::Cmd::UnitUPtr								\
		make() {													\
			return ::Hord::Cmd::UnitUPtr{new classn_()};			\
		}

/**
	Close command definition.
*/
#define HORD_CMD_DEF_CLOSE() \
	};

/** @} */ // end of name-group Command definition macros

/**
	Command base class.
*/
class Unit {
public:
	/**
		Ensure traits for deriving classes.

		@tparam D Deriving class.
	*/
	template<
		class D
	>
	struct ensure_traits :
		traits::require_t<
			D,
			tw::capture_post<std::is_base_of, Cmd::Unit>::type,
			tw::is_fully_moveable,
			std::is_copy_constructible,
			std::is_nothrow_destructible
		>,
		traits::disallow_t<
			D,
			std::is_copy_assignable
		>
	{};

private:
	Cmd::ID m_id{};
	Cmd::Status m_status{Cmd::Status::waiting};
	Cmd::StageUPtr m_initiator{nullptr};

	Unit& operator=(Unit const&) = delete;

protected:
/** @name Implementation */ /// @{
	/**
		get_type_info() implementation.
	*/
	virtual Cmd::type_info const&
	get_type_info_impl() const noexcept = 0;

	/**
		clone() implementation.

		@throws std::bad_alloc
	*/
	virtual Cmd::UnitUPtr
	clone_impl() = 0;
/// @}

/** @name Special member functions */ /// @{
public:
	/** Destructor. */
	virtual
	~Unit() noexcept = 0;

protected:
	/** Default constructor. */
	Unit() = default;
	/** Move constructor. */
	Unit(Unit&&) = default;
	/** Move assignment operator. */
	Unit& operator=(Unit&&) = default;

	/**
		Copy constructor.

		@note Copy will have no identification and no initiator.
	*/
	Unit(Unit const&)
		: Unit()
	{}

	/**
		Constructor with initiator.
	*/
	Unit(
		StageUPtr initiator
	)
		: m_initiator(std::move(initiator))
	{}
/// @}

public:
/** @name Properties */ /// @{
	/**
		Get command type info.
	*/
	Cmd::type_info const&
	get_type_info() const noexcept {
		return get_type_info_impl();
	}

	/**
		Get command type.
	*/
	Cmd::Type
	get_type() const noexcept {
		return get_type_info_impl().type;
	}

	/**
		Get %ID.

		@remarks @c System::Context uses the %ID for tracking stages
		and results. The returned reference should not be mutated
		elsewhere.
		@{
	*/
	Cmd::ID&
	get_id() noexcept {
		return m_id;
	}
	Cmd::ID const&
	get_id() const noexcept {
		return m_id;
	}
	/** @} */

	/**
		Get canonical %ID value.

		@note This returns the canonical %ID value (which
		includes @c flag_host), not the serial form.
	*/
	Cmd::IDValue
	get_id_canonical() const noexcept {
		return m_id.canonical();
	}

	/**
		Set status.
	*/
	void
	set_status(
		Cmd::Status const status
	) noexcept {
		m_status = status;
	}

	/**
		Get status.
	*/
	Cmd::Status
	get_status() const noexcept {
		return m_status;
	}

	/**
		Check if the base %ID value is non-null.

		@sa Cmd::ID
	*/
	bool
	is_identified() const noexcept {
		return m_id.is_identified();
	}

	/**
		Check if the command is host-initiated.

		@sa Cmd::ID
	*/
	bool
	is_host() const noexcept {
		return m_id.is_host();
	}

	/**
		Check if the command is client-initiated.

		@sa Cmd::ID
	*/
	bool
	is_client() const noexcept {
		return m_id.is_client();
	}

	/**
		Check if the command is active.

		@remarks This uses the initiator value of the command's ID.
		This will yield @c false until a context makes the command
		active.

		@sa Cmd::ID
	*/
	bool
	is_active() const noexcept {
		return m_id.is_initiator();
	}

	/**
		Check if the command has an initiator stage.
	*/
	bool
	has_initiator() const noexcept {
		return static_cast<bool>(m_initiator);
	}

	/**
		Get initiator stage.
	*/
	Cmd::StageUPtr&
	get_initiator() noexcept {
		return m_initiator;
	}

	/**
		Get initiator stage.
	*/
	Cmd::StageUPtr const&
	get_initiator() const noexcept {
		return m_initiator;
	}
/// @}

public:
/** @name Operations */ /// @{
	/**
		Duplicate the command.

		@note The duplicate command will have no identification.

		@throws std::bad_alloc
		If an allocation fails.
	*/
	Cmd::UnitUPtr
	clone() {
		return clone_impl();
	}
/// @}
};
inline Unit::~Unit() noexcept = default;

/** @} */ // end of doc-group cmd

} // namespace Cmd
} // namespace Hord

#endif // HORD_CMD_UNIT_HPP_
