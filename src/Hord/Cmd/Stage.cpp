
#include <Hord/detail/gr_ceformat.hpp>
#include <Hord/Cmd/Defs.hpp>
#include <Hord/Cmd/Stage.hpp>

#include <murk/Desc.hpp>
#include <murk/TieCompound.hpp>
#include <murk/serialize.hpp>

#include <iostream>

namespace Hord {
namespace Cmd {

// Forward declarations
class StageShadow;

// class Stage implementation

murk::Desc const
Stage::s_comp_base[2]{
	{murk::DescType::uint32},
	{murk::DescType::terminate}
};

static_assert(
	std::is_same<
		Cmd::ID,
		uint32_t
	>::value, ""
);

// serialization

void
Stage::deserialize_complex(
	std::istream&
) {}

void
Stage::serialize_complex(
	std::ostream&
) const {}

#define HORD_STAGE_MURK_MSG__(func__)								\
	"failed to " func__ " base at desc=(%#08p, %s);"				\
	" murk error:\n"												\
	"  >%s"
//

#define HORD_STAGE_THROW_MURK_ERROR__(err__, ex__)					\
	HORD_THROW_FMT(													\
		ErrorCode::serialization_io_failed,							\
		err__,														\
		&(ex__.get_tie().get_desc()),								\
		murk::get_desc_name(ex__.get_tie().get_desc().get_type()),	\
		ex__.what()													\
	)
//

#define HORD_SCOPE_FUNC deserialize
namespace {
HORD_DEF_FMT_FQN(
	s_err_deserialize_murk,
	HORD_STAGE_MURK_MSG__("deserialize")
);
} // anonymous namespace

void
Stage::deserialize(
	std::istream& stream
) {
	murk::TieCompound tcomp{
		get_stage_type_info().comp
	};
	auto& binder = tcomp.bind_begin(murk::BindMutable);
		binder(&m_id.serial);
		bind_base(binder);
	binder.bind_end();

	try {
		murk::deserialize(
			stream, tcomp, murk::Endian::LITTLE
		);
	} catch (murk::SerializeError& murk_err) {
		HORD_STAGE_THROW_MURK_ERROR__(
			s_err_deserialize_murk,
			murk_err
		);
	}

	deserialize_complex(stream);
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC serialize
namespace {
HORD_DEF_FMT_FQN(
	s_err_serialize_murk,
	HORD_STAGE_MURK_MSG__("serialize")
);
} // anonymous namespace

void
Stage::serialize(
	std::ostream& stream
) const {
	murk::TieCompound tcomp{
		get_stage_type_info().comp
	};
	auto& binder = tcomp.bind_begin(murk::BindImmutable);
		binder(&m_id.serial);
		bind_base_const(binder);
	binder.bind_end();

	try {
		murk::serialize(
			stream, tcomp, murk::Endian::LITTLE
		);
	} catch (murk::SerializeError& murk_err) {
		HORD_STAGE_THROW_MURK_ERROR__(
			s_err_serialize_murk,
			murk_err
		);
	}

	serialize_complex(stream);
}
#undef HORD_SCOPE_FUNC


// class StageShadow implementation

class StageShadow final
	: public Cmd::Stage
{
private:
	Stage* const m_stage;

	StageShadow() = delete;
	StageShadow(StageShadow const&) = delete;
	StageShadow& operator=(StageShadow const&) = delete;

public:
	explicit
	StageShadow(
		Cmd::Stage& stage
	)
		: m_stage(&stage)
	{}

	StageShadow(StageShadow&&) = default;
	~StageShadow() noexcept override = default;

private:
	Cmd::type_info const&
	get_command_type_info_impl() const noexcept override {
		return m_stage->get_command_type_info();
	}

	Cmd::Stage::type_info const&
	get_stage_type_info_impl() const noexcept override {
		return m_stage->get_stage_type_info();
	}

	void
	bind_base(
		murk::TieBinder& binder
	) noexcept override {
		m_stage->bind_base(binder);
	}

	void
	bind_base_const(
		murk::TieBinder& binder
	) const noexcept override {
		m_stage->bind_base_const(binder);
	}

	Cmd::Status
	execute_impl(
		System::Context& context,
		Cmd::Stage& initiator
	) override {
		return m_stage->execute_impl(context, initiator);
	}
};

Cmd::StageUPtr
make_stage_shadow(
	Stage& stage
) noexcept {
	return Cmd::StageUPtr{new StageShadow(
		stage
	)};
}

} // namespace Cmd
} // namespace Hord
