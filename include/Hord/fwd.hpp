/**
@file fwd.hpp
@brief Forward declarations.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license; see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_FWD_HPP_
#define HORD_FWD_HPP_

#include "./config.hpp"
#include "./common_types.hpp"

namespace Hord {

// Forward declarations
enum class FieldType : uint8_t;
enum class StandardMetaFieldTypes : MetaFieldType;
enum class StandardRuleTypes : RuleType;

struct MetaField;
struct StringMetaField;
struct Int32MetaField;
struct Int64MetaField;
struct BoolMetaField;
struct Metadata;

struct Field;
struct Record;

struct RuleState;
class Rule;
struct Column;
class Node;

class Hive;

} // namespace Hord

#endif // HORD_FWD_HPP_
