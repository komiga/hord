
#include <Hord/Cmd/Stage.hpp>

namespace Hord {
namespace Cmd {

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

} // namespace Cmd
} // namespace Hord
