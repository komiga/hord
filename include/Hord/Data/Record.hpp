/**
@file Data/Record.hpp
@brief Record struct.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#pragma once

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
	Record& operator=(Record const&) = delete;

public:
/** @name Special member functions */ /// @{
	/** Destructor. */
	~Record() noexcept = default;

	/** Default constructor. */
	Record() = default;
	/** Copy constructor. */
	Record(Record const&) = default;
	/** Move constructor. */
	Record(Record&&) = default;
	/** Move assignment operator. */
	Record& operator=(Record&&) = default;

	/**
		Construct record with size.
	*/
	Record(
		std::size_t const size
	)
		: fields(size)
	{}
/// @}
};

/** @} */ // end of doc-group data

} // namespace Data
} // namespace Hord
