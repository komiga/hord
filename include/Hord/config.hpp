/**
@file config.hpp
@brief Configuration.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_CONFIG_HPP_
#define HORD_CONFIG_HPP_

#include <cstdint>

/**
	Allocator for auxiliary specializations.
*/
#define HORD_AUX_ALLOCATOR std::allocator

/** @cond INTERNAL */
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
	around its implementation space. Throwing functions should likewise
	define and undefine @c HORD_SCOPE_FUNC_IDENT__ within the body.
	All of these macros require these definitions.
	@note <Hord/String.hpp> and <Hord/Error.hpp> are required to use these.

	@remark I quite despise this method, but there is no @c __fqn__.
	Luckily, a nice a side-effect of this method is that it cuts down on
	both implementation complexity and dynamic allocation -- both good
	in my book, even if the cost is paid in 2 gnarly preprocessing directives
	per throwing function.
	@{
*/

/**
	@def HORD_SCOPE_CLASS_IDENT__
	Current class identifier.
*/

/**
	@def HORD_SCOPE_FUNC_IDENT__
	Current function identifier.
*/

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
#define HORD_THROW_ERROR_SCOPED_IMPL__(s__, e__, m__) \
	throw ::Hord::Error{				\
		e__,							\
		::Hord::String{HORD_STR_LIT(	\
			s__ ": " m__				\
		)}								\
	}
/** @endcond */

/**
	Throw error with class scope.

	@note The final message is encapsulated in #HORD_STR_LIT
	(that is, @a m__ needn't be #HORD_STR_LIT-ized).
	@param e__ #ErrorCode.
	@param m__ Message.
	@sa HORD_THROW_ERROR_SCOPED_FQN
*/
#define HORD_THROW_ERROR_SCOPED(e__, m__) \
	HORD_THROW_ERROR_SCOPED_IMPL__(HORD_SCOPE_CLASS, e__, m__)

/**
	Throw error with fully-qualified scope.

	@param e__ #ErrorCode.
	@param m__ Message.
	@sa HORD_THROW_ERROR_SCOPED
*/
#define HORD_THROW_ERROR_SCOPED_FQN(e__, m__) \
	HORD_THROW_ERROR_SCOPED_IMPL__(HORD_SCOPE_FQN, e__, m__)

/** @} */ // end of name-group Error reporting

namespace Hord {
} // namespace Hord

#endif // HORD_CONFIG_HPP_
