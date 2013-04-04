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

/**
	Require a trait.

	@tparam Trait The trait to require.
*/
template<typename Trait>
struct require_trait final {
	static_assert(
		true==Trait::value,
		"required trait is not satisfied"
	);
};

/**
	Disallow a trait.

	@tparam Trait The trait to disallow.
*/
template<typename Trait>
struct disallow_trait final {
	static_assert(
		false==Trait::value,
		"disallowed trait is present"
	);
};

/**
	Whether a type is either copy constructible or copy assignable.

	@remarks OR of @c std::is_copy_constructible
	and @c std::is_copy_assignable.
	@tparam T Type to test.
*/
template<typename T>
struct is_copy_constructible_or_assignable final
	: public std::integral_constant<bool,
		// FIXME: libstdc++ 4.6.3 does not have
		// is_assignable nor the is_copy traits.
		std::is_constructible<T, T const&>::value
		//std::is_copy_constructible<T>::value ||
		//std::is_copy_assignable<T>::value
	>
{};

/** @} */ // end of doc-group traits

} // namespace traits
} // namespace Hord

#endif // HORD_TRAITS_HPP_
