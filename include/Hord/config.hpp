/**
@file config.hpp
@brief Configuration.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_CONFIG_HPP_
#define HORD_CONFIG_HPP_

#include <cstddef>
#include <cstdint>

/**
	@addtogroup config
	@{
*/

/**
	Allocator for auxiliary specializations.
*/
#define HORD_AUX_ALLOCATOR std::allocator

/** @cond INTERNAL */
#define DUCT_CONFIG_ALLOCATOR HORD_AUX_ALLOCATOR
#define MURK_AUX_ALLOCATOR HORD_AUX_ALLOCATOR

#define HORD_STRINGIFY_INNER__(x) #x
/** @endcond */

/**
	Stringify an identifier.
*/
#define HORD_STRINGIFY(x) \
	HORD_STRINGIFY_INNER__(x)

/**
	@name Error reporting
	These macros are for error reporting.
	A class implementation file should
	@code #define HORD_SCOPE_CLASS_IDENT__ ClassName @endcode
	and
	@code #undef HORD_SCOPE_CLASS_IDENT__ @endcode
	around its implementation space. Throwing functions should
	likewise define and undefine @c HORD_SCOPE_FUNC_IDENT__ within
	the body. All of these macros require these definitions.

	@note <Hord/String.hpp> and <Hord/Error.hpp> are required to use
	these.

	@note All throw macros except for #HORD_THROW_ERROR_S encapsulate
	the final message in #HORD_STR_LIT (that is, @a m__ needn't be
	#HORD_STR_LIT-ized).

	@remarks I quite despise this method, but there is no @c __fqn__.
	Luckily, a nice a side-effect of this method is that it cuts down
	on both implementation complexity and dynamic allocation -- both
	good in my book, even if the cost is paid in 2 gnarly
	preprocessing directives per throwing function.
	@{
*/

#ifdef DOXYGEN_CONSISTS_SOLELY_OF_UNICORNS_AND_CONFETTI
/**
	Current class identifier.

	This is defined surrounding class implementations.
*/
#define HORD_SCOPE_CLASS_IDENT__

/**
	Current function identifier.

	This is defined surrounding function implementations.
*/
#define HORD_SCOPE_FUNC_IDENT__
#endif

/**
	Returns the string literal of @c HORD_SCOPE_CLASS_IDENT__.
*/
#define HORD_SCOPE_CLASS \
	HORD_STRINGIFY(HORD_SCOPE_CLASS_IDENT__)

/**
	Returns the string literal of @c HORD_SCOPE_FUNC_IDENT__.
*/
#define HORD_SCOPE_FUNC \
	HORD_STRINGIFY(HORD_SCOPE_FUNC_IDENT__)

/**
	Returns the fully-qualified name of the current function.
*/
#define HORD_SCOPE_FQN \
	HORD_SCOPE_CLASS "::" HORD_SCOPE_FUNC

/** @cond INTERNAL */
#define HORD_MSG_SCOPED_IMPL__(s__, m__) \
		HORD_STR_LIT(s__ ": " m__)
/** @endcond */

/**
	Build message string literal with class scope.

	@param m__ Message.
*/
#define HORD_MSG_SCOPED_CLASS(m__) \
	HORD_MSG_SCOPED_IMPL__(HORD_SCOPE_CLASS, m__)

/**
	Build message string literal with function scope.

	@param m__ Message.
*/
#define HORD_MSG_SCOPED_FUNC(m__) \
	HORD_MSG_SCOPED_IMPL__(HORD_SCOPE_FUNC, m__)

/**
	Build message string literal with fully-qualified scope.

	@param m__ Message.
*/
#define HORD_MSG_SCOPED_FQN(m__) \
	HORD_MSG_SCOPED_IMPL__(HORD_SCOPE_FQN, m__)

/** @cond INTERNAL */
#define HORD_THROW_ERROR_IMPL__(e__, m__) \
	throw std::move(::Hord::Error{	\
		e__,						\
		::Hord::String{m__}			\
	})
/** @endcond */

/**
	Throw error with message.

	@param e__ ErrorCode.
	@param m__ Message (string literal).
*/
#define HORD_THROW_ERROR(e__, m__) \
	HORD_THROW_ERROR_IMPL__(e__, HORD_STR_LIT(m__))

/**
	Throw error with message.

	@param e__ ErrorCode.
	@param m__ Message (String).
*/
#define HORD_THROW_ERROR_S(e__, m__) \
	HORD_THROW_ERROR_IMPL__(e__, m__)

/**
	Throw error with class scope.

	@param e__ ErrorCode.
	@param m__ Message (string literal).

	@sa HORD_THROW_ERROR_SCOPED_FUNC,
		HORD_THROW_ERROR_SCOPED_FQN
*/
#define HORD_THROW_ERROR_SCOPED_CLASS(e__, m__) \
	HORD_THROW_ERROR_IMPL__(	\
		e__,						\
		HORD_MSG_SCOPED_CLASS(m__)	\
	)

/**
	Throw error with function scope.

	@param e__ ErrorCode.
	@param m__ Message (string literal).

	@sa HORD_THROW_ERROR_SCOPED_CLASS,
		HORD_THROW_ERROR_SCOPED_FQN
*/
#define HORD_THROW_ERROR_SCOPED_FUNC(e__, m__) \
	HORD_THROW_ERROR_IMPL__(	\
		e__,						\
		HORD_MSG_SCOPED_FUNC(m__)	\
	)

/**
	Throw error with fully-qualified scope.

	@param e__ ErrorCode.
	@param m__ Message (string literal).

	@sa HORD_THROW_ERROR_SCOPED_CLASS,
		HORD_THROW_ERROR_SCOPED_FUNC
*/
#define HORD_THROW_ERROR_SCOPED_FQN(e__, m__) \
	HORD_THROW_ERROR_IMPL__(	\
		e__,						\
		HORD_MSG_SCOPED_FQN(m__)	\
	)

/** @} */ // end of name-group Error reporting

namespace Hord {
} // namespace Hord

/** @} */ // end of doc-group config

#endif // HORD_CONFIG_HPP_
