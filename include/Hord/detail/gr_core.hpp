/**
@file detail/gr_core.hpp
@brief GR core configuration.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_DETAIL_GR_CORE_HPP_
#define HORD_DETAIL_GR_CORE_HPP_

#include <Hord/config.hpp>
#include <Hord/ErrorCode.hpp>
#include <Hord/String.hpp>
#include <Hord/Error.hpp>

#ifndef HORD_DETAIL_GR_CORE_NS_GUARD_
#define HORD_DETAIL_GR_CORE_NS_GUARD_

namespace Hord {
namespace detail {
namespace GR {
	using Error = ::Hord::Error;
}}}

#endif // HORD_DETAIL_GR_CORE_NS_GUARD_

#define DUCT_GR_ROOT_NAMESPACE \
	::Hord::detail::GR

#define DUCT_GR_STR_LIT(s_) \
	HORD_STR_LIT(s_)

#define DUCT_GR_MSG_IMPL_(s_, m_) \
	"{" s_ "} " m_

#define DUCT_GR_SCOPE_PREFIX \
	Hord::

#define DUCT_GR_SCOPE_CLASS \
	HORD_SCOPE_CLASS

#define DUCT_GR_SCOPE_FUNC \
	HORD_SCOPE_FUNC

#define HORD_SCOPE_FQN_STR_LIT \
	DUCT_GR_SCOPE_FQN_STR_LIT

#include <duct/GR/common.hpp>
#include <duct/GR/throw.hpp>

#define HORD_THROW(ec_, m_) \
	DUCT_GR_THROW(ec_, m_)

#define HORD_THROW_CLASS(ec_, m_) \
	DUCT_GR_THROW_CLASS(ec_, m_)

#define HORD_THROW_FUNC(ec_, m_) \
	DUCT_GR_THROW_FUNC(ec_, m_)

#define HORD_THROW_FQN(ec_, m_) \
	DUCT_GR_THROW_FQN(ec_, m_)

#endif // HORD_DETAIL_GR_CORE_HPP_
