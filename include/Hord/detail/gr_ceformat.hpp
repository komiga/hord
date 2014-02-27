/**
@file detail/gr_ceformat.hpp
@brief GR ceformat definitions.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_DETAIL_GR_CEFORMAT_HPP_
#define HORD_DETAIL_GR_CEFORMAT_HPP_

#include <Hord/config.hpp>
#include <Hord/detail/gr_core.hpp>

#include <ceformat/Format.hpp>
#include <ceformat/print.hpp>

#include <duct/GR/ceformat.hpp>

#define HORD_DEF_FMT(ident_, fmt_) \
	DUCT_GR_DEF_CEFMT(ident_, fmt_)

#define HORD_DEF_FMT_CLASS(ident_, fmt_) \
	DUCT_GR_DEF_CEFMT_CLASS(ident_, fmt_)

#define HORD_DEF_FMT_FUNC(ident_, fmt_) \
	DUCT_GR_DEF_CEFMT_FUNC(ident_, fmt_)

#define HORD_DEF_FMT_FQN(ident_, fmt_) \
	DUCT_GR_DEF_CEFMT_FQN(ident_, fmt_)

#define HORD_THROW_FMT(ec_, cefmt_, ...) \
	DUCT_GR_THROW_CEFMT(ec_, cefmt_, __VA_ARGS__)

#endif // HORD_DETAIL_GR_CEFORMAT_HPP_
