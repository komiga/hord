
#include <Hord/Error.hpp>
#include <Hord/Object/Unit.hpp>
#include <Hord/IO/PropStream.hpp>

#include <murk/Desc.hpp>
#include <murk/TieCompound.hpp>
#include <murk/serialize.hpp>

#include <ceformat/print.hpp>

namespace Hord {
namespace Object {

// class Unit implementation

#define HORD_SCOPE_CLASS_IDENT__ Object::Unit

Unit::Unit(
	bool const supplies_primary,
	bool const supplies_auxiliary,
	Object::ID const id,
	Object::ID const parent
) noexcept
	: m_id(id)
	, m_prop_states(
		supplies_primary,
		supplies_auxiliary
	)
	, m_parent(parent)
{}

Unit::Unit(Unit&&) = default;
Unit::~Unit() noexcept = default;

Unit& Unit::operator=(Unit&&) = default;

// properties

void
Unit::set_slug(
	String slug
) noexcept {
	m_slug.assign(std::move(slug));
	if (64u < m_slug.size()) {
		m_slug.resize(64u);
	}
	// TODO: Truncate invalid unit sequence (if any) after resize
}

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

#define HORD_UNIT_ERR_MSG_UNSUPPLIED__ \
	"prop %08x -> %s is not supplied for type %s"

#define HORD_UNIT_CHECK_UNSUPPLIED__(err__)						\
	if (!m_prop_states.is_supplied(type)) {						\
		HORD_THROW_ERROR_F(										\
			ErrorCode::serialization_prop_unsupplied,			\
			err__,												\
			m_id,												\
			IO::get_prop_type_name(type),						\
			Object::get_type_name(get_type())					\
		);														\
	}
//

namespace {
namespace prop_comp {
	static murk::DescCompound const
	identity{
		{murk::DescType::uint32}, // parent
		{murk::StringDesc{0u}}, // slug
		{murk::DescType::terminate}
	};

	static murk::DescCompound const
	scratch{
		{murk::StringDesc{0u}}, // data
		{murk::DescType::terminate}
	};
}

HORD_FMT_UNSCOPED(
	s_err_prop_murk,
	"%s: failed to %s prop at desc=(%#08p, %s);"
	" murk error:\n"
	"  >%s"
);
} // anonymous namespace

#define HORD_SCOPE_FUNC_IDENT__ deserialize_base_prop
void
deserialize_base_prop(
	IO::InputPropStream& prop_stream,
	murk::TieCompound& tcomp
) {
	try {
		murk::deserialize(
			prop_stream.get_stream(), tcomp, murk::Endian::LITTLE
		);
	} catch (murk::SerializeError& murk_err) {
		HORD_UNIT_THROW_MURK_ERROR__(
			s_err_prop_murk,
			HORD_SCOPE_FQN,
			"deserialize",
			murk_err
		);
	}
}
#undef HORD_SCOPE_FUNC_IDENT__

#define HORD_SCOPE_FUNC_IDENT__ serialize_base_prop
void
serialize_base_prop(
	IO::OutputPropStream& prop_stream,
	murk::TieCompound const& tcomp
) {
	try {
		murk::serialize(
			prop_stream.get_stream(), tcomp, murk::Endian::LITTLE
		);
	} catch (murk::SerializeError& murk_err) {
		HORD_UNIT_THROW_MURK_ERROR__(
			s_err_prop_murk,
			HORD_SCOPE_FQN,
			"serialize",
			murk_err
		);
	}
}
#undef HORD_SCOPE_FUNC_IDENT__

#define HORD_SCOPE_FUNC_IDENT__ deserialize
namespace {
HORD_FMT_SCOPED_FQN(
	s_err_deserialize_unsupplied,
	HORD_UNIT_ERR_MSG_UNSUPPLIED__
);
} // anonymous namespace

void
Unit::deserialize(
	IO::InputPropStream& prop_stream
) {
	IO::PropType const type = prop_stream.get_type();
	HORD_UNIT_CHECK_UNSUPPLIED__(s_err_deserialize_unsupplied);

	switch (type) {
	case IO::PropType::identity: {
		murk::TieCompound tcomp{prop_comp::identity};
		tcomp
		.bind_begin(murk::BindMutable)
			(&m_parent)
			(&m_slug)
		.bind_end();
		deserialize_base_prop(prop_stream, tcomp);
	} break;

	case IO::PropType::metadata:
		m_metadata.deserialize(prop_stream);
		break;

	case IO::PropType::scratch: {
		murk::TieCompound tcomp{prop_comp::scratch};
		tcomp
		.bind_begin(murk::BindMutable)
			(&m_scratch_data)
		.bind_end();
		deserialize_base_prop(prop_stream, tcomp);
	} break;

	default:
		deserialize_impl(prop_stream);
		break;
	}

	m_prop_states.assign(type, IO::PropState::original);
}
#undef HORD_SCOPE_FUNC_IDENT__

#define HORD_SCOPE_FUNC_IDENT__ serialize
namespace {
HORD_FMT_SCOPED_FQN(
	s_err_serialize_unsupplied,
	HORD_UNIT_ERR_MSG_UNSUPPLIED__
);
} // anonymous namespace

void
Unit::serialize(
	IO::OutputPropStream& prop_stream
) {
	IO::PropType const type = prop_stream.get_type();
	HORD_UNIT_CHECK_UNSUPPLIED__(s_err_serialize_unsupplied);

	switch (type) {
	case IO::PropType::identity: {
		murk::TieCompound tcomp{prop_comp::identity};
		tcomp
		.bind_begin(murk::BindImmutable)
			(&m_parent)
			(&m_slug)
		.bind_end();
		serialize_base_prop(prop_stream, tcomp);
	} break;

	case IO::PropType::metadata:
		m_metadata.serialize(prop_stream);
		break;

	case IO::PropType::scratch: {
		murk::TieCompound tcomp{prop_comp::scratch};
		tcomp
		.bind_begin(murk::BindImmutable)
			(&m_scratch_data)
		.bind_end();
		serialize_base_prop(prop_stream, tcomp);
	} break;

	default:
		serialize_impl(prop_stream);
		break;
	}

	m_prop_states.assign(type, IO::PropState::original);
}
#undef HORD_SCOPE_FUNC_IDENT__

#undef HORD_SCOPE_CLASS_IDENT__

} // namespace Object
} // namespace Hord
