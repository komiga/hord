
#include <Hord/Cmd/Defs.hpp>
#include <Hord/Cmd/Stage.hpp>

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
	bind_impl(
		murk::TieBinder& binder
	) noexcept override {
		m_stage->bind_impl(binder);
	}

	void
	bind_const_impl(
		murk::TieBinder& binder
	) const noexcept override {
		m_stage->bind_const_impl(binder);
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
