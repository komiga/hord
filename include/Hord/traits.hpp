/**
@file
@brief Type traits and helpers.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#pragma once

#include <Hord/config.hpp>

#include <trait_wrangler/traits.hpp>
#include <trait_wrangler/test.hpp>

namespace Hord {

namespace tw = ::trait_wrangler;

namespace traits {

/**
	@addtogroup etc
	@{
*/
/**
	@addtogroup traits
	@{
*/

/**
	See @c trait_wrangler::require_t.
*/
template<
	class T,
	template<class> class... TraitTpl
>
struct require_t
	: public tw::require_t<T, TraitTpl...>
{};

/**
	See @c trait_wrangler::disallow_t.
*/
template<
	class T,
	template<class> class... TraitTpl
>
struct disallow_t
	: public tw::disallow_t<T, TraitTpl...>
{};

/** @} */ // end of doc-group traits
/** @} */ // end of doc-group etc

} // namespace traits
} // namespace Hord
