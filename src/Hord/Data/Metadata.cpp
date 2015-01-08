/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <Hord/utility.hpp>
#include <Hord/String.hpp>
#include <Hord/serialization.hpp>
#include <Hord/Data/Metadata.hpp>
#include <Hord/IO/Defs.hpp>

#include <duct/debug.hpp>

#include <Hord/detail/gr_ceformat.hpp>

namespace Hord {
namespace Data {

// class Metadata implementation

#define HORD_SCOPE_CLASS Metadata

Data::TableSchema const
Metadata::s_schema{
	{"name", {Data::ValueType::string, Data::Size::b8}},
	{"value", {Data::ValueType::dynamic, Data::Size::b8}}
};

#define HORD_SCOPE_FUNC deserialize
namespace {
HORD_DEF_FMT_FQN(
	s_err_read_failed,
	HORD_SER_ERR_MSG_IO_PROP("read")
);
} // anonymous namespace

void
Metadata::deserialize(
	IO::InputPropStream& prop_stream
) try {
	DUCT_ASSERTE(IO::PropType::metadata == prop_stream.get_type());
	auto ser = prop_stream.make_serializer();

	// table
	Data::Table des_table{};
	ser(des_table);
	des_table.configure(Data::Metadata::s_schema);

	// commit
	m_table.operator=(std::move(des_table));
} catch (SerializerError& serr) {
	HORD_THROW_SER_PROP(
		s_err_read_failed,
		serr,
		prop_stream.get_info().object_id,
		"metadata"
	);
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC serialize
namespace {
HORD_DEF_FMT_FQN(
	s_err_write_failed,
	HORD_SER_ERR_MSG_IO_PROP("write")
);
} // anonymous namespace

void
Metadata::serialize(
	IO::OutputPropStream& prop_stream
) const try {
	DUCT_ASSERTE(IO::PropType::metadata == prop_stream.get_type());
	auto ser = prop_stream.make_serializer();

	// table
	ser(m_table);
} catch (SerializerError& serr) {
	HORD_THROW_SER_PROP(
		s_err_write_failed,
		serr,
		prop_stream.get_info().object_id,
		"metadata"
	);
}
#undef HORD_SCOPE_FUNC

#undef HORD_SCOPE_CLASS // Metadata

} // namespace Data
} // namespace Hord
