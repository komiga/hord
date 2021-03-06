/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Type traits and helpers.
@ingroup traits
*/

#pragma once

#include <Hord/config.hpp>

#include <trait_wrangler/traits.hpp>
#include <trait_wrangler/test.hpp>

namespace Hord {

namespace tw = ::trait_wrangler;

namespace traits {

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

} // namespace traits
} // namespace Hord
