
#include <Hord/String.hpp>
#include <Hord/serialization.hpp>
#include <Hord/Object/Defs.hpp>
#include <Hord/Node/Unit.hpp>
#include <Hord/IO/Defs.hpp>
#include <Hord/IO/PropStream.hpp>

#include <cassert>

#include <Hord/detail/gr_ceformat.hpp>

namespace Hord {
namespace Node {

// class Unit implementation

#define HORD_SCOPE_CLASS Node::Unit

Unit::~Unit() noexcept = default;

Unit::Unit(Unit&&) = default;
Unit& Unit::operator=(Unit&&) = default;

Unit::Unit(
	Object::type_info const& tinfo,
	Node::ID const id,
	Object::ID const parent
) noexcept
	: base(
		tinfo,
		static_cast<Object::ID>(id),
		parent
	)
{}

void
Unit::set_layout_ref(
	Node::ID const node_id
) noexcept {
	m_layout_ref = node_id;
	/* TODO */
}

// serialization

#define HORD_SCOPE_FUNC deserialize_prop_primary
void
Unit::deserialize_prop_primary(
	IO::InputPropStream& /*prop_stream*/,
	InputSerializer& /*ser*/
) {
	/* TODO */
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC serialize_prop_primary
void
Unit::serialize_prop_primary(
	IO::OutputPropStream& /*prop_stream*/,
	OutputSerializer& /*ser*/
) const {
	/* TODO */
}
#undef HORD_SCOPE_FUNC


// - auxiliary

#define HORD_SCOPE_FUNC deserialize_prop_auxiliary
void
Unit::deserialize_prop_auxiliary(
	IO::InputPropStream& /*prop_stream*/,
	InputSerializer& ser
) {
	Node::ID des_layout_ref{Node::ID_NULL};
	ser(des_layout_ref);
	/* TODO */
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC serialize_prop_auxiliary
void
Unit::serialize_prop_auxiliary(
	IO::OutputPropStream& /*prop_stream*/,
	OutputSerializer& ser
) const {
	ser(m_layout_ref);
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
	switch (prop_stream.get_type()) {
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
		get_id(),
		IO::get_prop_type_name(prop_stream.get_type())
	);
}
#undef HORD_SCOPE_FUNC

#undef HORD_SCOPE_CLASS

} // namespace Node
} // namespace Hord
