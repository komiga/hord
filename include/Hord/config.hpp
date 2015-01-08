/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Configuration.
@ingroup config
*/

#pragma once

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

#define CEFORMAT_AUX_ALLOCATOR HORD_AUX_ALLOCATOR
//#define CEFORMAT_CONFIG_STRING_TYPE
//#define CEFORMAT_CONFIG_OSTRINGSTREAM_TYPE

#define HORD_DATA_ENDIAN duct::Endian::little

/** @endcond */

#include <duct/config.hpp>

/**
	Stringify an identifier.
*/
#define HORD_STRINGIFY(x) \
	DUCT_STRINGIFY(x)

/** @} */ // end of doc-group config
