
#include <Hord/utility.hpp>
#include <Hord/String.hpp>
#include <Hord/Error.hpp>
#include <Hord/Rule/Defs.hpp>
#include <Hord/Node/Column.hpp>

#include <murk/Desc.hpp>
#include <murk/TieCompound.hpp>
#include <murk/serialize.hpp>

#include <ceformat/print.hpp>

#include <iostream>

namespace Hord {
namespace Node {

// class Column implementation

// serialization

#define HORD_UNIT_THROW_MURK_ERROR__(err__, fqn__, des_ser__, ex__)	\
	HORD_THROW_ERROR_F(												\
		ErrorCode::serialization_io_failed,							\
		err__,														\
		fqn__,														\
		des_ser__,													\
		&(ex__.get_tie().get_desc()),								\
		murk::get_desc_name(ex__.get_tie().get_desc().get_type()),	\
		ex__.what()													\
	)
//

namespace {
namespace comp {
	static murk::DescCompound const
	properties{
		{murk::DescType::uint32}, // type
		{murk::StringDesc{0u}}, // title
		{murk::VectorDesc{murk::DescType::uint32}}, // rules
		{murk::DescType::terminate}
	};
}

HORD_FMT_UNSCOPED(
	s_err_io_murk,
	"%s: failed to %s at desc=(%#08p, %s);"
	" murk error:\n"
	"  >%s"
);
} // anonymous namespace

#define HORD_SCOPE_FUNC_IDENT__ deserialize
void
Column::deserialize(
	std::istream& stream
) {
	Rule::Type des_type{enum_cast(Rule::StandardTypes::None)};
	String des_title{};
	rule_id_vector_type des_rules{};

	try {
		murk::TieCompound tcomp{comp::properties};
		tcomp
		.bind_begin(murk::BindMutable)
			(&des_type)
			(&des_title)
			(&des_rules)
		.bind_end();
		murk::deserialize(
			stream, tcomp, murk::Endian::LITTLE
		);
	} catch (murk::SerializeError& murk_err) {
		HORD_UNIT_THROW_MURK_ERROR__(
			s_err_io_murk,
			HORD_SCOPE_FQN,
			"deserialize",
			murk_err
		);
	}

	// commit
	this->type = des_type;
	this->title.operator=(std::move(des_title));
	this->rules.operator=(std::move(des_rules));
}
#undef HORD_SCOPE_FUNC_IDENT__

#define HORD_SCOPE_FUNC_IDENT__ serialize
void
Column::serialize(
	std::ostream& stream
) const {
	try {
		murk::TieCompound tcomp{comp::properties};
		tcomp
		.bind_begin(murk::BindImmutable)
			(&this->type)
			(&this->title)
			(&this->rules)
		.bind_end();
		murk::serialize(
			stream, tcomp, murk::Endian::LITTLE
		);
	} catch (murk::SerializeError& murk_err) {
		HORD_UNIT_THROW_MURK_ERROR__(
			s_err_io_murk,
			HORD_SCOPE_FQN,
			"serialize",
			murk_err
		);
	}
}
#undef HORD_SCOPE_FUNC_IDENT__

} // namespace Node
} // namespace Hord
