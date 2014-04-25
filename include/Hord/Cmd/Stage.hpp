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
#include <Hord/serialization.hpp>
#include <Hord/Cmd/Defs.hpp>
#include <Hord/Cmd/Unit.hpp>

#include <type_traits>
#include <iosfwd>

namespace Hord {

// Forward declarations (external)
namespace System {
class Context;
}

namespace Cmd {

// Forward declarations
class Stage;

template<Cmd::Type const, Cmd::StageType const, class, class>
class StageImpl;

/**
	@addtogroup cmd
	@{
*/

/**
	@name Stage definition macros

	TODO[check] Unless a macro takes a @a stagen_ parameter, it requires
	@c HORD_CMD_STAGE_TYPE_ to be defined to the stage name.
	HORD_CMD_CONSTRUCT_CLOSE() and HORD_CMD_STAGE_DATA_CLOSE() also
	require @c HORD_CMD_TYPE_.

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
	Generic close stage definition.

	@note @a cmdt_ and @a staget_ are typecast to Cmd::Type and
	Cmd::StageType, respectively.

	@param cmdn_ Identifier of the command.
	@param stagen_ Identifier of the stage.
	@param cmdt_ Command type.
	@param staget_ %Stage type.
	@param cti_ Command type info.
	@param sti_ %Stage type info.
*/
#define HORD_CMD_STAGE_CLOSE_G(cmdn_, stagen_, cmdt_, staget_, cti_, sti_)	\
		using impl = ::Hord::Cmd::StageImpl<								\
			static_cast<::Hord::Cmd::Type>(cmdt_),							\
			static_cast<::Hord::Cmd::StageType>(staget_),					\
			cmdn_,															\
			stagen_															\
		>;																	\
	};																		\
	template<> ::Hord::Cmd::type_info const&								\
	stagen_::impl::get_command_type_info_impl() const noexcept {			\
		return cti_;														\
	}																		\
	template<> ::Hord::Cmd::Stage::type_info const&							\
	stagen_::impl::get_stage_type_info_impl() const noexcept {				\
		return sti_;														\
	}

/**
	Define stage type information.

	@note This is encapsulated in an anonymous namespace.
*/
#define HORD_CMD_STAGE_DEF_TYPE_INFO()									\
	namespace {															\
	static ::Hord::Cmd::Stage::type_info const							\
	HORD_CMD_JOIN_ST_(HORD_CMD_TYPE_, HORD_CMD_STAGE_TYPE_){			\
		::Hord::Cmd::StageType:: HORD_CMD_STAGE_TYPE_					\
	};																	\
	}

/**
	Open stage definition.

	Stage data body should follow macro (without braces).

	@note This calls HORD_CMD_STAGE_DEF_TYPE_INFO().
*/
#define HORD_CMD_STAGE_DATA_OPEN()				\
	HORD_CMD_STAGE_DEF_TYPE_INFO();				\
	struct HORD_CMD_STAGE_TYPE_ final {

/**
	Open stage definition with base class.

	Stage data body should follow macro (without braces).

	@note This calls HORD_CMD_STAGE_DEF_TYPE_INFO().

	@param base_ Base class.
*/
#define HORD_CMD_STAGE_DATA_OPEN_INHERIT(base_)	\
	HORD_CMD_STAGE_DEF_TYPE_INFO();				\
	struct HORD_CMD_STAGE_TYPE_ final : public base_ {

/**
	Standard close stage definition.

	This requires @c HORD_CMD_TYPE_ to be defined to the command
	type for the stages and @c HORD_SCOPE_CLASS to be defined to the
	command class identifier. This also requires accessible static
	@c Cmd::type_info and @c Cmd::Stage::type_info objects with the
	names:

	- <code>s_type_info_ ## HORD_CMD_TYPE_</code>
	- <code>s_type_info_ ## HORD_CMD_TYPE_ ## _ ## stagen_</code>
*/
#define HORD_CMD_STAGE_DATA_CLOSE()											\
	HORD_CMD_STAGE_CLOSE_G(													\
		HORD_SCOPE_CLASS,													\
		HORD_CMD_STAGE_TYPE_,												\
		::Hord::Cmd::Type::HORD_CMD_TYPE_,									\
		::Hord::Cmd::StageType::HORD_CMD_STAGE_TYPE_,						\
		HORD_CMD_JOIN_CT_(HORD_CMD_TYPE_),									\
		HORD_CMD_JOIN_ST_(HORD_CMD_TYPE_, HORD_CMD_STAGE_TYPE_)				\
	)

/**
	Open stage data serialize() function definition.

	Function body should follow macro (without braces).
	This should be placed within the stage data definition.
*/
#define HORD_CMD_STAGE_DATA_SERIALIZE_OPEN()			\
	template<class Ser>									\
	::Hord::ser_result_type								\
	serialize(											\
		::Hord::ser_tag_serialize,						\
		Ser& ser										\
	) {													\
		auto& self = ::Hord::const_safe<Ser>(*this);	\
		(void)self;

/**
	Open stage data read() function definition.

	Function body should follow macro (without braces).
	This should be placed within the stage data definition.
*/
#define HORD_CMD_STAGE_DATA_READ_OPEN()					\
	::Hord::ser_result_type								\
	read(												\
		::Hord::ser_tag_read,							\
		::Hord::InputSerializer& ser					\
	) {													\
		auto& self = *this;								\
		(void)self;

/**
	Open stage data write() function definition.

	Function body should follow macro (without braces).
	This should be placed within the stage data definition.
*/
#define HORD_CMD_STAGE_DATA_WRITE_OPEN()				\
	::Hord::ser_result_type								\
	write(												\
		::Hord::ser_tag_write,							\
		::Hord::OutputSerializer& ser					\
	) const {											\
		auto& self = *this;								\
		(void)self;

/**
	Close stage data serialize() function definition.
*/
#define HORD_CMD_STAGE_DATA_SERIALIZE_CLOSE() \
	}

/**
	Close stage data read() function definition.
*/
#define HORD_CMD_STAGE_DATA_READ_CLOSE() \
	HORD_CMD_STAGE_DATA_SERIALIZE_CLOSE()

/**
	Close stage data write() function definition.
*/
#define HORD_CMD_STAGE_DATA_WRITE_CLOSE() \
	HORD_CMD_STAGE_DATA_SERIALIZE_CLOSE()

/**
	Define an empty serialization function.
*/
#define HORD_CMD_STAGE_DATA_NO_SERIALIZATION()	\
	HORD_CMD_STAGE_DATA_SERIALIZE_OPEN()		\
		(void)ser;								\
	HORD_CMD_STAGE_DATA_SERIALIZE_CLOSE()

/**
	Define execute_typecast() by stage name.

	Function body should follow macro (with braces).
	This should be placed outside of the stage data definition.
	See StageImpl::execute_typecast() for visible parameters.

	@param stagen_ Identifier of the stage.
*/
#define HORD_CMD_STAGE_DEF_EXECUTE(stagen_)		\
	template<> ::Hord::Cmd::Status				\
	stagen_::impl::execute_typecast(			\
		::Hord::System::Context& context,		\
		stagen_::impl::owner_type& command		\
	)

/** @} */ // end of name-group Stage definition macros

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
		Type info.
	*/
	struct type_info final {
		/**
			%Stage type.
		*/
		Cmd::StageType const type;
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
			std::is_copy_constructible,
			std::is_nothrow_destructible
		>,
		traits::disallow_t<
			D,
			std::is_copy_assignable
		>
	{};

private:
	friend class StageShadow;

	Cmd::ID m_id;

	Stage& operator=(Stage const&) = delete;

protected:
/** @name Implementation */ /// @{
	/**
		get_command_type_info() implementation.
	*/
	virtual Cmd::type_info const&
	get_command_type_info_impl() const noexcept = 0;

	/**
		get_stage_type_info() implementation.
	*/
	virtual Cmd::Stage::type_info const&
	get_stage_type_info_impl() const noexcept = 0;

	/**
		get_data() implementation.
	*/
	virtual void const*
	get_data_impl() const noexcept = 0;

	/**
		read() implementation.

		@throws SerializerError{..}
	*/
	virtual ser_result_type
	read_impl(
		InputSerializer& ser
	) = 0;

	/**
		write() implementation.

		@throws SerializerError{..}
	*/
	virtual ser_result_type
	write_impl(
		OutputSerializer& ser
	) const = 0;

	/**
		clone() implementation.

		@throws std::bad_alloc
	*/
	virtual Cmd::StageUPtr
	clone_impl() = 0;

	/**
		execute() implementation.

		@throws std::bad_alloc
		From stage allocation.
	*/
	virtual Cmd::Status
	execute_impl(
		System::Context& context,
		Cmd::Unit& command
	) = 0;
/// @}

/** @name Special member functions */ /// @{
public:
	/** Destructor. */
	virtual
	~Stage() noexcept = 0;

protected:
	/** Default constructor. */
	Stage() = default;
	/** Move constructor. */
	Stage(Stage&&) = default;
	/** Move assignment operator. */
	Stage& operator=(Stage&&) = default;

	/**
		Copy constructor.

		@note Copy will have no identification.
	*/
	Stage(Stage const&)
		: Stage()
	{}
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
		Get stage data.

		@returns Pointer to stage data, or @c nullptr if none.
	*/
	void const*
	get_data() const noexcept {
		return get_data_impl();
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
		Check if the stage is an initiator.

		@sa Cmd::ID
	*/
	bool
	is_initiator() const noexcept {
		return m_id.is_initiator();
	}
/// @}

public:
/** @name Serialization */ /// @{
	/**
		Read from input serializer.

		@warning State may not be retained if an exception is thrown.

		@throws SerializerError{..}
		If a serialization operation failed.

		@throws Error{ErrorCode::serialization_data_malformed}
		If malformed data was encountered.
	*/
	ser_result_type
	read(
		ser_tag_read,
		InputSerializer& ser
	) {
		ser(m_id);
		read_impl(ser);
	}

	/**
		Write to output serializer.

		@throws SerializerError{..}
		If a serialization operation failed.
	*/
	ser_result_type
	write(
		ser_tag_write,
		OutputSerializer& ser
	) const {
		ser(m_id);
		write_impl(ser);
	}
/// @}

/** @name Operations */ /// @{
	/**
		Duplicate the stage.

		@note The duplicate stage will have no identification.

		@throws std::bad_alloc
		If an allocation fails.
	*/
	Cmd::StageUPtr
	clone() {
		return clone_impl();
	}

	/**
		Execute the stage.

		@note This will call Cmd::Unit::set_status() with the return
		value of execute_impl().

		@warning This function should not be used directly.
		See System::Context.

		@pre @code get_id().is_identified() @endcode

		@throws std::bad_alloc
		If an allocation fails.

		@returns Status of the command after execution.
	*/
	Cmd::Status
	execute(
		System::Context& context,
		Cmd::Unit& command
	) {
		// TODO: preconditions
		Cmd::Status const status = execute_impl(context, command);
		command.set_status(status);
		return status;
	}
/// @}
};
inline Stage::~Stage() noexcept = default;

/**
	Generic command stage implementation.

	@note @a Data must satisfy input and output serialization
	operations for Cacophony.

	@tparam command_type Command type.
	@tparam stage_type %Stage type.
	@tparam Owner Command class for the stage.
	@tparam Data Data class for the stage.
*/
template<
	Cmd::Type const command_type,
	Cmd::StageType const stage_type,
	class Owner,
	class Data
>
class StageImpl final
	: public Cmd::Stage
{
private:
	using owner_type = Owner;
	using data_type = Data;

	static_assert(
		Cacophony::is_input_serializable<InputSerializer, data_type>::value &&
		Cacophony::is_output_serializable<OutputSerializer, data_type>::value,
		"Data type must satisfy input and output serialization for"
		" Cacophony"
	);

	data_type m_data{};

	StageImpl& operator=(StageImpl const&) = delete;

public:
/** @name Special member functions */ /// @{
	/** Destructor. */
	~StageImpl() noexcept override = default;

	/** Default constructor. */
	StageImpl() = default;
	/** Copy constructor. */
	StageImpl(StageImpl const&) = default;
	/** Move constructor. */
	StageImpl(StageImpl&&) = default;
	/** Move assignment operator. */
	StageImpl& operator=(StageImpl&&) = default;

	/**
		Data constructor.

		@param data %Stage data.
	*/
	explicit
	StageImpl(
		data_type&& data
	)
		: m_data(std::move(data))
	{}
/// @}

private:
// Implementation
	Cmd::type_info const&
	get_command_type_info_impl() const noexcept override;

	Cmd::Stage::type_info const&
	get_stage_type_info_impl() const noexcept override;

	void const*
	get_data_impl() const noexcept override {
		return &m_data;
	}

	void
	read_impl(
		InputSerializer& ser
	) override {
		ser(m_data);
	}

	void
	write_impl(
		OutputSerializer& ser
	) const override {
		ser(m_data);
	}

	Cmd::StageUPtr
	clone_impl() override {
		return Cmd::StageUPtr{new StageImpl(*this)};
	}

	Cmd::Status
	execute_impl(
		System::Context& context,
		Cmd::Unit& command
	) override {
		return execute_typecast(
			context,
			static_cast<owner_type&>(command)
		);
	}

	Cmd::Status
	execute_typecast(
		System::Context& context,
		owner_type& command
	);
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
