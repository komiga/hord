/**
@file Cmd/Stage.hpp
@brief Command stage base class.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_CMD_STAGE_HPP_
#define HORD_CMD_STAGE_HPP_

#include <Hord/config.hpp>
#include <Hord/traits.hpp>
#include <Hord/Cmd/Defs.hpp>

#include <murk/DescCompound.hpp>
#include <murk/TieCompound.hpp>

#include <iosfwd>

namespace Hord {

// Forward declarations (external)
namespace System {
class Context;
}

namespace Cmd {

// Forward declarations
class Stage;

template<Cmd::Type, Cmd::StageType, class Data>
class StageImpl;

/**
	@addtogroup cmd
	@{
*/

/**
	@name Macros

	Command implementation helpers.

	@note Due to an arbitrary limitation in C++, the implementation
	definition macros must be placed in the same scope as %StageImpl
	-- namely, @c Hord::Cmd.

	@remarks Yes, this is terrible. But it's better than redefining
	the entire %Stage interface for every command implementation.
	With this we can reduce the amount of refactoring needed if
	something needs to change up the inheritance tree (i.e., %Stage).

	@{
*/

/**
	Generic command stage declaration.

	@note @a cmdt__ and @a staget__ are typecast to Cmd::Type and
	Cmd::StageType, respectively.

	@param stagen__ Identifier for the stage name.
	@param cmdt__ Command type.
	@param staget__ %Stage type.
	@param cti__ Command type info.
	@param sti__ %Stage type info.
	@param mems__ Data members.
*/
#define HORD_CMD_STAGE_DECL_G(stagen__, cmdt__, staget__, cti__, sti__, mems__)\
	struct stagen__ final {												\
		using impl = ::Hord::Cmd::StageImpl<							\
			static_cast<::Hord::Cmd::Type>(cmdt__),						\
			static_cast<::Hord::Cmd::StageType>(staget__),				\
			stagen__													\
		>;																\
		mems__															\
	};																	\
	template<> ::Hord::Cmd::type_info const&							\
	stagen__::impl::get_command_type_info_impl() const noexcept {		\
		return cti__;													\
	}																	\
	template<> ::Hord::Cmd::Stage::type_info const&						\
	stagen__::impl::get_stage_type_info_impl() const noexcept {			\
		return sti__;													\
	}

/** @cond INTERNAL */
// Forgive me
#define HORD_CMD_STAGE__JOIN_CT__(cmdn__) \
	HORD_CMD_STAGE__JOIN_CT_I__(cmdn__)
#define HORD_CMD_STAGE__JOIN_CT_I__(cmdn__) \
	s_type_info_ ## cmdn__

#define HORD_CMD_STAGE__JOIN_ST__(cmdn__, stagen__) \
	HORD_CMD_STAGE__JOIN_ST_I__(cmdn__, stagen__)
#define HORD_CMD_STAGE__JOIN_ST_I__(cmdn__, stagen__) \
	s_type_info_ ## cmdn__ ## _ ## stagen__
/** @endcond */ // INTERNAL

/**
	Standard command stage declaration.

	This requires @c HORD_CMD_TYPE__ to be defined to the command
	type for the stages. This also requires accessible static
	@c Cmd::type_info and @c Cmd::Stage::type_info objects with the
	names:

	- @c s_type_info_ ## HORD_CMD_TYPE__
	- @c s_type_info_ ## HORD_CMD_TYPE__ ## _ ## stagen__

	@param stagen__ Identifier for the stage name.
	@param mems__ Data members.
*/
#define HORD_CMD_STAGE_DECL(stagen__, mems__)					\
	HORD_CMD_STAGE_DECL_G(										\
		stagen__,												\
		::Hord::Cmd::Type::HORD_CMD_TYPE__,						\
		::Hord::Cmd::StageType::stagen__,						\
		HORD_CMD_STAGE__JOIN_CT__(HORD_CMD_TYPE__),				\
		HORD_CMD_STAGE__JOIN_ST__(HORD_CMD_TYPE__, stagen__),	\
		mems__													\
	)

/**
	Define execute_impl() by name.

	Function body should follow macro. See Stage::execute_impl()
	for visible parameters.

	@param stagen__ Identifier for the stage name.
*/
#define HORD_CMD_STAGE_DEF_EXECUTE(stagen__)	\
	template<> ::Hord::Cmd::Status				\
	stagen__::impl::execute_impl(				\
		::Hord::System::Context& context,		\
		::Hord::Cmd::Stage& initiator			\
	)

/**
	Define sub-bind with specified constness.

	@param stagen__ Identifier for the stage name.
	@param mems__ Sequence for the bind.
	@param subn__ Sub-name of bind.
	@param constness__ Constness of bind.
*/
#define HORD_CMD_STAGE_DEF_BIND_S(stagen__, mems__, subn__, constness__) \
	template<> void								\
	stagen__::impl::bind_base ## subn__(	\
		::murk::TieBinder& binder				\
	) constness__ noexcept {					\
		binder mems__;							\
	}											\

/**
	Define bind_base() and bind_base_const() individually by stage
	name.

	@remarks See Stage::bind_base() for visible parameters.

	@param stagen__ Identifier for the stage name.
	@param mems__ Sequence for bind_base().
	@param mems_const__ Sequence for bind_base_const().
*/
#define HORD_CMD_STAGE_DEF_BIND_EACH(stagen__, mems__, mems_const__)\
	HORD_CMD_STAGE_DEF_BIND_S(stagen__, mems__      ,,)				\
	HORD_CMD_STAGE_DEF_BIND_S(stagen__, mems_const__, _const, const)

/**
	Define bind_base() and bind_base_const() by stage name.

	@remarks See Stage::bind_base() for visible parameters.

	@param stagen__ Identifier for the stage name.
	@param mems__ Sequence of adjacent parentheses-enclosed pointers
	to data members to be bound.
*/
#define HORD_CMD_STAGE_DEF_BIND(stagen__, mems__) \
	HORD_CMD_STAGE_DEF_BIND_EACH(stagen__, mems__, mems__)

/**
	Define deserialize_complex() by name.

	Function body should follow macro.
	See Stage::deserialize_complex() for visible parameters.

	@param stagen__ Identifier for the stage name.
*/
#define HORD_CMD_STAGE_DEF_DESERIALIZE_COMPLEX(stagen__) \
	template<> void							\
	stagen__::impl::deserialize_complex(	\
		std::istream& stream				\
	)

/**
	Define serialize_complex() by name.

	Function body should follow macro.
	See Stage::serialize_complex() for visible parameters.

	@param stagen__ Identifier for the stage name.
*/
#define HORD_CMD_STAGE_DEF_SERIALIZE_COMPLEX(stagen__) \
	template<> void							\
	stagen__::impl::serialize_complex(		\
		std::ostream& stream				\
	) const

/** @} */ // end of name-group Macros

/**
	@name Utilities
	@{
*/

/**
	Make stage shadow.

	@note Local initiator stages use this to pass a copy of
	themselves to the remote endpoint without incurring extra
	memory costs. The shadow acts as a new distinct stage, but
	internally passes through to the implementation of the
	shadowed stage.

	@par
	@note The result stage has @c !is_identified().

	@warning This does not extend the lifetime of the shadowed
	stage; more generally: the shadowed stage must be active
	within its context (ergo: it must be an active initiator).

	@returns Owning pointer to a shadow of @a stage.
	@param stage %Stage to shadow.
*/
Cmd::StageUPtr
make_stage_shadow(
	Stage& stage
) noexcept;

/** @} */ // end of name-group Utilities

/**
	Command stage base class.
*/
class Stage {
public:
	/**
		Base descriptor compound.
	*/
	static murk::Desc const s_comp_base[2];

	/**
		Type info.
	*/
	struct type_info final {
	/** @name name_group_name */ /// @{
		/**
			%Stage type.
		*/
		Cmd::StageType const type;

		/**
			Descriptor compound for type.

			@note This must begin with a reference
			to @c Stage::s_comp_base.
		*/
		murk::DescCompoundRef const comp;
	/// @}
	};

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
			tw::capture_post<std::is_base_of, Cmd::Stage>::type,
			tw::is_fully_moveable,
			std::is_nothrow_destructible
		>,
		traits::disallow_t<
			D,
			tw::is_copyable
		>
	{};

private:
	friend class StageShadow;

	union {
		Cmd::ID const serial;
		Cmd::IDFields fields;
		struct {
			Cmd::ID const value : 31;
			bool const _ : 1;
		} canonical;
	} m_id{Cmd::NULL_ID};

	Stage(Stage const&) = delete;
	Stage& operator=(Stage const&) = delete;

protected:
/** @name Implementation */ /// @{
	/**
		get_command_type_info() implementation.
	*/
	virtual Cmd::type_info const&
	get_command_type_info_impl() const noexcept = 0;

	/**
		get_stage_type() implementation.
	*/
	virtual Cmd::Stage::type_info const&
	get_stage_type_info_impl() const noexcept = 0;

	/**
		bind() implementation.

		@note deserialize() will bind base properties;
		implementations shall bind only their own base properties.
	*/
	virtual void
	bind_base(
		murk::TieBinder& binder
	) noexcept = 0;

	/**
		bind_const() implementation.

		@note serialize() will bind base properties;
		implementations shall bind only their own base properties.
	*/
	virtual void
	bind_base_const(
		murk::TieBinder& binder
	) const noexcept = 0;

	/**
		Deserialize complex data from stream.

		@note This is optional. The base implementation does nothing.
	*/
	virtual void
	deserialize_complex(
		std::istream& stream
	);

	/**
		Serialize complex data to stream.

		@note This is optional. The base implementation does nothing.
	*/
	virtual void
	serialize_complex(
		std::ostream& stream
	) const;

	/**
		execute() implementation.

		@throws std::bad_alloc
	*/
	virtual Cmd::Status
	execute_impl(
		System::Context& context,
		Cmd::Stage& initiator
	) = 0;
/// @}

/** @name Constructors and destructor */ /// @{
	/** Default constructor. */
	Stage() = default;
	/** Move constructor. */
	Stage(Stage&&) = default;
public:
	/** Destructor. */
	virtual
	~Stage() noexcept = 0;
/// @}

protected:
/** @name Operators */ /// @{
	/** Move assignment operator. */
	Stage& operator=(Stage&&) = default;
/// @}

public:
/** @name Properties */ /// @{
	/**
		Get command type info.
	*/
	Cmd::type_info const&
	get_command_type_info() const noexcept {
		return get_command_type_info_impl();
	}

	/**
		Get command type.
	*/
	Cmd::Type
	get_command_type() const noexcept {
		return get_command_type_info_impl().type;
	}

	/**
		Get stage type info.
	*/
	Cmd::Stage::type_info const&
	get_stage_type_info() const noexcept {
		return get_stage_type_info_impl();
	}

	/**
		Get stage type.
	*/
	Cmd::StageType
	get_stage_type() const noexcept {
		return get_stage_type_info_impl().type;
	}

	/**
		Get %ID.

		@note This returns the canonical %ID value (which
		includes @c flag_host), not the serial form.
	*/
	Cmd::ID
	get_id() const noexcept {
		return m_id.canonical.value;
	}

	/**
		Get %ID fields.

		@remarks @c System::Context uses the %ID for tracking stages
		and results. The returned reference should not be mutated
		elsewhere.
		@{
	*/
	Cmd::IDFields&
	get_id_fields() noexcept {
		return m_id.fields;
	}
	Cmd::IDFields const&
	get_id_fields() const noexcept {
		return m_id.fields;
	}
	/** @} */

	/**
		Check if the base %ID value is non-null.

		@sa Cmd::IDFields
	*/
	bool
	is_identified() const noexcept {
		// NB: Avoid sticky flag_host by checking the base value
		return Cmd::NULL_ID != m_id.fields.base;
	}

	/**
		Check if the stage belongs to a host-initiated command.

		@sa Cmd::IDFields
	*/
	bool
	is_host() const noexcept {
		return m_id.fields.flag_host;
	}

	/**
		Check if the stage belongs to a local-initiated command.

		@sa Cmd::IDFields
	*/
	bool
	is_local() const noexcept {
		return !m_id.fields.flag_host;
	}

	/**
		Check if the stage is an initiator.

		@sa Cmd::IDFields
	*/
	bool
	is_initiator() const noexcept {
		return m_id.fields.flag_initiator;
	}
/// @}

/** @name Serialization */ /// @{
	/**
		Deserialize from stream.

		@warning State may not be retained if an exception is thrown.

		@throws Error{ErrorCode::serialization_io_failed}
		If an input operation failed.

		@throws Error{ErrorCode::serialization_data_malformed}
		If malformed data was encountered.

		@param stream Stream.
	*/
	void
	deserialize(
		std::istream& stream
	);

	/**
		Serialize to stream.

		@throws Error{ErrorCode::serialization_io_failed}
		If an output operation failed.

		@param stream Stream.
	*/
	void
	serialize(
		std::ostream& stream
	) const;
/// @}

/** @name Operations */ /// @{
	/**
		Execute the stage.

		@warning This function should not be used directly.
		See System::Context.

		@note @a initiator may be equal to @c this.

		@pre @code get_id() != Cmd::NULL_ID @endcode

		@throws std::bad_alloc
		If an allocation fails.

		@returns Status of the command after execution.
		@param context %Context.
		@param initiator Initiating stage of the command.
	*/
	Cmd::Status
	execute(
		System::Context& context,
		Cmd::Stage& initiator
	) {
		// TODO: preconditions
		return execute_impl(context, initiator);
	}
/// @}
};
inline Stage::~Stage() noexcept = default;

/**
	Generic command stage implementation.

	@tparam command_type Command type.
	@tparam stage_type %Stage type.
	@tparam Data Data class for the stage.
*/
template<
	Cmd::Type command_type,
	Cmd::StageType stage_type,
	class Data
>
class StageImpl final
	: public Cmd::Stage
{
private:
	Data m_data;

	StageImpl(StageImpl const&) = delete;
	StageImpl& operator=(StageImpl const&) = delete;

public:
/** @name Constructors and destructor */ /// @{
	/**
		Data constructor.

		@param data %Stage data.
	*/
	explicit
	StageImpl(
		Data&& data
	)
		: m_data(std::move(data))
	{}

	/** Default constructor. */
	StageImpl() = default;
	/** Move constructor. */
	StageImpl(StageImpl&&) = default;
	/** Destructor. */
	~StageImpl() noexcept override = default;
/// @}

private:
	Cmd::type_info const&
	get_command_type_info_impl() const noexcept override;

	Cmd::Stage::type_info const&
	get_stage_type_info_impl() const noexcept override;

	void
	bind_base(
		murk::TieBinder& binder
	) noexcept override;

	void
	bind_base_const(
		murk::TieBinder& binder
	) const noexcept override;

	void
	deserialize_complex(
		std::istream& /*stream*/
	) override {}

	void
	serialize_complex(
		std::ostream& /*stream*/
	) const override {}

	Cmd::Status
	execute_impl(
		System::Context& context,
		Cmd::Stage& initiator
	) override;
};

/** @} */ // end of doc-group cmd

} // namespace Cmd

// Traits
template struct traits::require_t<
	Cmd::Stage,
	std::is_abstract
>;

} // namespace Hord

#endif // HORD_CMD_STAGE_HPP_
