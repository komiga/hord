/**
@file Data/Record.hpp
@brief Record struct.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_DATA_RECORD_HPP_
#define HORD_DATA_RECORD_HPP_

#include <Hord/config.hpp>
#include <Hord/aux.hpp>
#include <Hord/Data/Field.hpp>

namespace Hord {
namespace Data {

// Forward declarations
struct Record;

/**
	@addtogroup data
	@{
*/

/**
	Data record.
*/
struct Record final {
public:
	/** Field vector. */
	using field_vector_type = aux::vector<Data::Field>;

/** @name Properties */ /// @{
	/** Field collection. */
	field_vector_type fields{};
/// @}

private:
	Record(Record const&) = delete;
	Record& operator=(Record const&) = delete;

public:
/** @name Constructors and destructor */ /// @{
	/** Default constructor. */
	Record() = default;
	/** Move constructor. */
	Record(Record&&) = default;
	/** Destructor. */
	~Record() noexcept = default;
/// @}

/** @name Operators */ /// @{
	/** Move assignment operator. */
	Record& operator=(Record&&) = default;
/// @}
};

/** @} */ // end of doc-group data

} // namespace Data
} // namespace Hord

#endif // HORD_DATA_RECORD_HPP_
