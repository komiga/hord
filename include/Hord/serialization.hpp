/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Serialization definitions.
@ingroup serialization
*/

#pragma once

#include <Hord/config.hpp>

#include <Cacophony/types.hpp>
#include <Cacophony/traits.hpp>
#include <Cacophony/utility.hpp>
#include <Cacophony/ErrorCode.hpp>
#include <Cacophony/Error.hpp>
#include <Cacophony/BinarySerializer.hpp>
#include <Cacophony/support/binary_blob.hpp>
#include <Cacophony/support/sequence.hpp>
#include <Cacophony/support/enum_cfg.hpp>
#include <Cacophony/support/string_cfg.hpp>
#include <Cacophony/support/vector_cfg.hpp>
#include <Cacophony/support/std_string.hpp>
#include <Cacophony/support/std_vector.hpp>

#include <duct/EndianUtils.hpp>
#include <duct/IO/dynamic_streambuf.hpp>
#include <duct/IO/cacophony_support.hpp>

namespace Hord {

// Forward declarations

/**
	@addtogroup serialization
	@{
*/

/**
	Dynamic @c std::stream buffer.
*/
using DynamicStreamBuf
= duct::IO::basic_dynamic_streambuf<
	char,
	std::char_traits<char>,
	HORD_AUX_ALLOCATOR<char>
>;

// Cacophony imports

using Cacophony::const_safe;

/**
	Serializer error code type.
*/
using SerializerErrorCode = Cacophony::ErrorCode;

/**
	Serializer error type.
*/
using SerializerError = Cacophony::Error;

/**
	Data input serializer.
*/
using InputSerializer = Cacophony::BinaryInputSerializer;

/**
	Data output serializer.
*/
using OutputSerializer = Cacophony::BinaryOutputSerializer;

/**
	Serialization function result type.
*/
using ser_result_type = Cacophony::ser_result_type;

/**
	Serialization function tags for Cacophony.

	These should come first in the serialization function parameter
	list.
	@{
*/
using ser_tag_serialize = Cacophony::tag_serialize;
using ser_tag_read = Cacophony::tag_read;
using ser_tag_write = Cacophony::tag_write;
/** @} */

/**
	Get the name of a serializer error.

	@returns C-string containing the name of @a error_code or
	"INVALID" if somehow @a error_code is not actually a
	SerializerErrorCode.
*/
inline char const*
get_ser_error_name(
	SerializerErrorCode const error_code
) noexcept {
	return Cacophony::get_error_name(error_code);
}

/**
	Make input serializer.
*/
inline InputSerializer
make_input_serializer(
	std::istream& stream
) noexcept {
	return InputSerializer{stream, HORD_DATA_ENDIAN};
}

/**
	Make output serializer.
*/
inline OutputSerializer
make_output_serializer(
	std::ostream& stream
) noexcept {
	return OutputSerializer{stream, HORD_DATA_ENDIAN};
}

/** @cond INTERNAL */
#define HORD_SER_ERR_MSG_IO_GENERIC(skind_)			\
	"failed to " skind_ ":"							\
	" serialization error: \n  {%s} %s"

#define HORD_SER_ERR_MSG_IO_PROP(skind_)			\
	"failed to " skind_ " prop %08x -> %s:"			\
	" serialization error: \n  {%s} %s"

#define HORD_THROW_SER_FMT(efmt_, serr_)			\
	HORD_THROW_FMT(									\
		ErrorCode::serialization_io_failed,			\
		efmt_,										\
		get_ser_error_name(serr_.get_code()),		\
		serr_.get_message()							\
	)
//

#define HORD_THROW_SER_PROP(efmt_, serr_, objid_, pkind_)	\
	HORD_THROW_FMT(											\
		Hord::ErrorCode::serialization_io_failed,			\
		efmt_,												\
		objid_.value(),										\
		pkind_,												\
		Hord::get_ser_error_name(serr_.get_code()),			\
		serr_.get_message()									\
	)
//
/** @endcond */ // INTERNAL

/** @} */ // end of doc-group serialization

} // namespace Hord
