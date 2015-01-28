/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <Hord/String.hpp>
#include <Hord/serialization.hpp>
#include <Hord/Data/Table.hpp>
#include <Hord/Object/Defs.hpp>
#include <Hord/Table/Unit.hpp>
#include <Hord/IO/Defs.hpp>
#include <Hord/IO/PropStream.hpp>

#include <duct/debug.hpp>

#include <Hord/detail/gr_ceformat.hpp>

namespace Hord {
namespace Table {

// class Unit implementation

#define HORD_SCOPE_CLASS Table::Unit

Unit::~Unit() noexcept = default;

Unit::Unit(Unit&&) = default;
Unit& Unit::operator=(Unit&&) = default;

Unit::Unit(
	Object::type_info const& tinfo,
	Table::ID const id,
	Object::ID const parent
) noexcept
	: base(
		tinfo,
		static_cast<Object::ID>(id),
		parent
	)
{}

// serialization

#define HORD_SCOPE_FUNC deserialize_impl
namespace {
HORD_DEF_FMT_FQN(
	s_err_read_failed,
	HORD_SER_ERR_MSG_IO_PROP("read")
);
} // anonymous namespace

void
Unit::deserialize_impl(
	IO::InputPropStream& prop_stream
) try {
	auto ser = prop_stream.make_serializer();
	switch (prop_stream.get_type()) {
	case IO::PropType::identity: {
		Schema::ID des_schema_ref{Schema::ID_NULL};
		ser(des_schema_ref);

		// commit
		set_schema_ref(des_schema_ref);
	}	break;

	case IO::PropType::primary: {
		Data::Table des_data;
		ser(des_data);

		// commit
		m_data = std::move(des_data);
	}	break;

	default:
		// Object::Unit should protect us from this
		DUCT_ASSERTE(false);
	}
} catch (SerializerError& serr) {
	HORD_THROW_SER_PROP(
		s_err_read_failed,
		serr,
		get_id(),
		IO::get_prop_type_name(prop_stream.get_type())
	);
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC serialize_impl
namespace {
HORD_DEF_FMT_FQN(
	s_err_write_failed,
	HORD_SER_ERR_MSG_IO_PROP("write")
);
} // anonymous namespace

void
Unit::serialize_impl(
	IO::OutputPropStream& prop_stream
) const try {
	auto ser = prop_stream.make_serializer();
	switch (prop_stream.get_type()) {
	case IO::PropType::identity:
		ser(m_schema_ref);
		break;

	case IO::PropType::primary:
		ser(m_data);
		break;

	default:
		// Object::Unit should protect us from this
		DUCT_ASSERTE(false);
	}
} catch (SerializerError& serr) {
	HORD_THROW_SER_PROP(
		s_err_write_failed,
		serr,
		get_id(),
		IO::get_prop_type_name(prop_stream.get_type())
	);
}
#undef HORD_SCOPE_FUNC

#undef HORD_SCOPE_CLASS

} // namespace Table
} // namespace Hord
