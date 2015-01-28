/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <Hord/String.hpp>
#include <Hord/serialization.hpp>
#include <Hord/Object/Defs.hpp>
#include <Hord/Schema/Unit.hpp>
#include <Hord/IO/Defs.hpp>
#include <Hord/IO/PropStream.hpp>

#include <cassert>

#include <Hord/detail/gr_ceformat.hpp>

namespace Hord {
namespace Schema {

// class Unit implementation

#define HORD_SCOPE_CLASS Schema::Unit

Unit::~Unit() noexcept = default;

Unit::Unit(Unit&&) = default;
Unit& Unit::operator=(Unit&&) = default;

Unit::Unit(
	Object::TypeInfo const& tinfo,
	Schema::ID const id,
	Object::ID const parent
) noexcept
	: base(
		tinfo,
		static_cast<Object::ID>(id),
		parent
	)
{}

// serialization

#define HORD_SCOPE_FUNC deserialize_prop_primary
void
Unit::deserialize_prop_primary(
	IO::InputPropStream& prop_stream,
	InputSerializer& /*ser*/
) {
	m_init_metadata.deserialize(prop_stream);
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC serialize_prop_primary
void
Unit::serialize_prop_primary(
	IO::OutputPropStream& prop_stream,
	OutputSerializer& /*ser*/
) const {
	m_init_metadata.serialize(prop_stream);
}
#undef HORD_SCOPE_FUNC


// - impl

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
	switch (prop_stream.type()) {
	case IO::PropType::identity:
		// No additional identity properties
		break;

	case IO::PropType::primary:
		deserialize_prop_primary(prop_stream, ser);
		break;

	case IO::PropType::auxiliary:
		deserialize_prop_auxiliary(prop_stream, ser);
		break;

	default:
		// Object::Unit should protect us from this
		assert(false);
	}
} catch (SerializerError& serr) {
	HORD_THROW_SER_PROP(
		s_err_read_failed,
		serr,
		id(),
		IO::get_prop_type_name(prop_stream.type())
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
	switch (prop_stream.type()) {
	case IO::PropType::identity:
		// No additional identity properties
		break;

	case IO::PropType::primary:
		serialize_prop_primary(prop_stream, ser);
		break;

	case IO::PropType::auxiliary:
		serialize_prop_auxiliary(prop_stream, ser);
		break;

	default:
		// Object::Unit should protect us from this
		assert(false);
	}
} catch (SerializerError& serr) {
	HORD_THROW_SER_PROP(
		s_err_write_failed,
		serr,
		id(),
		IO::get_prop_type_name(prop_stream.type())
	);
}
#undef HORD_SCOPE_FUNC

#undef HORD_SCOPE_CLASS

} // namespace Schema
} // namespace Hord
