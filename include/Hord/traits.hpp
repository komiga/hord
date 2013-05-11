/**
@file traits.hpp
@brief Type traits and helpers.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_TRAITS_HPP_
#define HORD_TRAITS_HPP_

#include "./config.hpp"

#include <type_traits>

namespace Hord {
namespace traits {

// Forward declarations

/**
	@addtogroup traits
	@{
*/

template<typename... P>
struct integral_and;

template<typename I>
struct integral_and<I> final
	: public std::integral_constant<bool,
		I::value
	>
{};

template<typename I, typename... P>
struct integral_and<I, P...> final
	: public std::integral_constant<bool,
		I::value &&
		integral_and<P...>::value
	>
{};

/**
	Require traits.

	@tparam P The traits to require.
*/
template<typename... P>
struct require {
	static_assert(
		true==integral_and<P...>::value,
		"required traits are not satisfied"
	);
};

/**
	Disallow traits.

	@tparam P The traits to disallow.
*/
template<typename... P>
struct disallow {
	static_assert(
		false==integral_and<P...>::value,
		"disallowed traits are present"
	);
};

/**
	Whether a type is either copy constructible or copy assignable.

	@tparam T Type to test.
*/
template<typename T>
struct is_copy_constructible_or_assignable final
	: public std::integral_constant<bool,
		std::is_copy_constructible<T>::value ||
		std::is_copy_assignable<T>::value
	>
{};

/**
	Whether a type is either move constructible or move assignable.

	@tparam T Type to test.
*/
template<typename T>
struct is_move_constructible_or_assignable final
	: public std::integral_constant<bool,
		std::is_move_constructible<T>::value ||
		std::is_move_assignable<T>::value
	>
{};

/** @} */ // end of doc-group traits

} // namespace traits
} // namespace Hord

#endif // HORD_TRAITS_HPP_
