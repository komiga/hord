
#include <Hord/serialization.hpp>
#include <Hord/Object/Unit.hpp>
#include <Hord/IO/Defs.hpp>
#include <Hord/IO/PropStream.hpp>

#include <Hord/detail/gr_ceformat.hpp>

namespace Hord {
namespace Object {

// class Unit implementation

#define HORD_SCOPE_CLASS Object::Unit

Unit::~Unit() noexcept = default;

Unit::Unit(Unit&&) = default;
Unit& Unit::operator=(Unit&&) = default;

Unit::Unit(
	type_info const& tinfo,
	Object::ID const id,
	Object::ID const parent
) noexcept
	: m_id(id)
	, m_prop_states(tinfo.props)
	, m_parent(parent)
{}

// properties

void
Unit::set_parent(
	Object::ID const parent
) noexcept {
	bool const differs = parent != m_parent;
	if (differs) {
		m_parent = parent;
	}
	if (differs || !m_prop_states.is_initialized(IO::PropType::identity)) {
		m_prop_states.assign(
			IO::PropType::identity,
			IO::PropState::modified
		);
	}
}

void
Unit::set_slug(
	String slug
) noexcept {
	bool const differs = m_slug != slug;
	if (differs) {
		m_slug.assign(std::move(slug));
		if (0xFF < m_slug.size()) {
			m_slug.resize(0xFF);
			// TODO: Truncate invalid unit sequence (if any) after resize
		}
	}
	if (differs || !m_prop_states.is_initialized(IO::PropType::identity)) {
		m_prop_states.assign(
			IO::PropType::identity,
			IO::PropState::modified
		);
	}
}

void
Unit::set_scratch_space(
	String scratch_space
) noexcept {
	m_scratch_space.assign(std::move(scratch_space));
	m_prop_states.assign(
		IO::PropType::scratch,
		IO::PropState::modified
	);
}

// serialization

#define HORD_UNIT_ERR_MSG_UNSUPPLIED_ \
	"prop %08x -> %s is not supplied for type %s"

#define HORD_UNIT_CHECK_UNSUPPLIED_(err_)						\
	if (!m_prop_states.is_supplied(type)) {						\
		HORD_THROW_FMT(											\
			ErrorCode::serialization_prop_unsupplied,			\
			err_,												\
			get_id(),											\
			IO::get_prop_type_name(type),						\
			Object::get_type_name(get_type())					\
		);														\
	}
//

#define HORD_SCOPE_FUNC deserialize
namespace {
HORD_DEF_FMT_FQN(
	s_err_deserialize_unsupplied,
	HORD_UNIT_ERR_MSG_UNSUPPLIED_
);
HORD_DEF_FMT_FQN(
	s_err_read_failed,
	HORD_SER_ERR_MSG_IO_PROP("read")
);
} // anonymous namespace

void
Unit::deserialize(
	IO::InputPropStream& prop_stream
) try {
	IO::PropType const type = prop_stream.get_type();
	HORD_UNIT_CHECK_UNSUPPLIED_(s_err_deserialize_unsupplied);

	auto ser = prop_stream.make_serializer();
	switch (type) {
	case IO::PropType::identity: {
		Object::ID parent{Object::NULL_ID};
		String slug{};
		ser(
			parent,
			Cacophony::make_string_cfg<std::uint8_t>(slug)
		);

		// commit
		set_parent(parent);
		set_slug(std::move(slug));
	} break;

	case IO::PropType::metadata:
		m_metadata.deserialize(prop_stream);
		break;

	case IO::PropType::scratch: {
		String scratch_space{};
		ser(Cacophony::make_string_cfg<std::uint32_t>(scratch_space));

		// commit
		m_scratch_space.assign(std::move(scratch_space));
	} break;

	default:
		deserialize_impl(prop_stream);
		break;
	}

	m_prop_states.assign(type, IO::PropState::original);
} catch (SerializerError& serr) {
	HORD_THROW_SER_PROP(
		s_err_read_failed,
		serr,
		get_id(),
		IO::get_prop_type_name(prop_stream.get_type())
	);
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC serialize
namespace {
HORD_DEF_FMT_FQN(
	s_err_serialize_unsupplied,
	HORD_UNIT_ERR_MSG_UNSUPPLIED_
);
HORD_DEF_FMT_FQN(
	s_err_serialize_improper_state,
	"prop %08x -> %s cannot be serialized while uninitialized"
);
HORD_DEF_FMT_FQN(
	s_err_write_failed,
	HORD_SER_ERR_MSG_IO_PROP("write")
);
} // anonymous namespace

void
Unit::serialize(
	IO::OutputPropStream& prop_stream
) try {
	IO::PropType const type = prop_stream.get_type();
	HORD_UNIT_CHECK_UNSUPPLIED_(s_err_serialize_unsupplied);
	if (!m_prop_states.is_initialized(type)) {
		HORD_THROW_FMT(
			ErrorCode::serialization_prop_improper_state,
			s_err_serialize_improper_state,
			m_id,
			IO::get_prop_type_name(type)
		);
	}

	auto ser = prop_stream.make_serializer();
	switch (type) {
	case IO::PropType::identity:
		ser(
			m_parent,
			Cacophony::make_string_cfg<std::uint8_t>(m_slug)
		);
		break;

	case IO::PropType::metadata:
		m_metadata.serialize(prop_stream);
		break;

	case IO::PropType::scratch:
		ser(Cacophony::make_string_cfg<std::uint32_t>(m_scratch_space));
		break;

	default:
		serialize_impl(prop_stream);
		break;
	}

	m_prop_states.assign(type, IO::PropState::original);
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

} // namespace Object
} // namespace Hord
