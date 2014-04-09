
#include <Hord/serialization.hpp>
#include <Hord/Cmd/Defs.hpp>
#include <Hord/Cmd/Stage.hpp>

#include <iostream>

namespace Hord {
namespace Cmd {

// Forward declarations
class StageShadow;

// class StageShadow implementation

class StageShadow final
	: public Cmd::Stage
{
private:
	Stage* const m_stage;

	StageShadow() = delete;
	StageShadow& operator=(StageShadow const&) = delete;

public:
	~StageShadow() noexcept override = default;

	StageShadow(StageShadow const&) = default;
	StageShadow(StageShadow&&) = default;

	explicit
	StageShadow(
		Cmd::Stage& stage
	)
		: m_stage(&stage)
	{}

private:
	Cmd::type_info const&
	get_command_type_info_impl() const noexcept override {
		return m_stage->get_command_type_info();
	}

	Cmd::Stage::type_info const&
	get_stage_type_info_impl() const noexcept override {
		return m_stage->get_stage_type_info();
	}

	void const*
	get_data_impl() const noexcept override {
		return m_stage->get_data_impl();
	}

	void
	read_impl(
		InputSerializer& ser
	) override {
		m_stage->read_impl(ser);
	}

	void
	write_impl(
		OutputSerializer& ser
	) const override {
		m_stage->write_impl(ser);
	}

	StageUPtr
	clone_impl() override {
		return StageUPtr{new StageShadow(*this)};
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
