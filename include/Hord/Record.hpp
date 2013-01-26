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
#include "./fwd.hpp"
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
		String str{}; /**< String value. */
		union {
			int64_t num{0}; /**< Integer value. */
			bool bin; /**< Boolean value. */
		};
	} value{};
	/** Rule state. */
	std::unique_ptr<RuleState> state{nullptr}; // Runtime
/// @}

private:
	Field& operator=(Field const&)=delete;

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

/** @name Operators */ /// @{
	/** Move assignment operator. */
	Field& operator=(Field&&)=default;
/// @}
};

/**
	Data record.
*/
struct Record final {
	/** Field vector. */
	typedef aux::vector<Field> field_vector_type;

/** @name Properties */ /// @{
	/** Field collection. */
	field_vector_type fields{};
/// @}

private:
	Record& operator=(Record const&)=delete;

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

/** @name Operators */ /// @{
	/** Move assignment operator. */
	Record& operator=(Record&&)=default;
/// @}
};

/** @} */ // end of doc-group data

} // namespace Hord

#endif // HORD_RECORD_HPP_
