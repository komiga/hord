/**
@file Record.hpp
@brief Data classes.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license; see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_RECORD_HPP_
#define HORD_RECORD_HPP_

#include "./config.hpp"
#include "./common_enums.hpp"
#include "./aux.hpp"
#include "./String.hpp"

#include <memory>

namespace Hord {

// Forward declarations
struct Field;
struct Record;

/**
	@addtogroup data
	@{
*/

/**
	Data field.
*/
struct Field final {
/** @name Properties */ /// @{
	/** Type. */
	FieldType type{FieldType::Text};
	/** Value. */
	struct {
		String str{};
		union {
			int64_t num{0};
			bool bin;
		};
	} value{};
	/** Rule state. */
	std::unique_ptr<RuleState> state{nullptr};
/// @}

private:
	Field& operator=(Field const&)=delete;
	Field& operator=(Field&&)=delete;

public:
/** @name Constructors and destructor */ /// @{
	/** Constructor with type. */
	Field()=default;
	/**
		Copy constructor.
		@note Copy will have a null state.
	*/
	Field(Field const& other)
		: type{other.type}
		, value{other.value}
		, state{nullptr}
	{}
	/** Move constructor. */
	Field(Field&&)=default;
	/** Destructor. */
	~Field()=default;
/// @}
};

/**
	Data record.
*/
struct Record final {
	/** Field vector. */
	typedef aux::vector<Field> field_vector_type;

/** @name Properties */ /// @{
	field_vector_type fields;
/// @}

private:
	Record& operator=(Record const&)=delete;
	Record& operator=(Record&&)=delete;

public:
/** @name Constructors and destructor */ /// @{
	/** Constructor. */
	Record()=default;
	/** Copy constructor. */
	Record(Record const&)=default;
	/** Move constructor. */
	Record(Record&&)=default;
	/** Destructor. */
	~Record()=default;
/// @}
};

/** @} */ // end of doc-group data

} // namespace Hord

#endif // HORD_RECORD_HPP_
