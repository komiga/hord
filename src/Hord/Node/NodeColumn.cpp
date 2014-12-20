
#include <Hord/utility.hpp>
#include <Hord/String.hpp>
#include <Hord/serialization.hpp>
#include <Hord/Data/Defs.hpp>
#include <Hord/Node/Column.hpp>

namespace Hord {
namespace Node {

// class Column implementation

// serialization

ser_result_type
Column::read(
	ser_tag_read,
	InputSerializer& ser
) {
	Data::FieldType des_type{};
	String des_title{};
	rule_id_vector_type des_rules{};
	ser(
		des_type,
		des_title,
		des_rules
	);

	// commit
	this->type = des_type;
	this->title.operator=(std::move(des_title));
	this->rules.operator=(std::move(des_rules));
}

ser_result_type
Column::write(
	ser_tag_write,
	OutputSerializer& ser
) const {
	ser(
		this->type,
		this->title,
		this->rules
	);
}

} // namespace Node
} // namespace Hord
