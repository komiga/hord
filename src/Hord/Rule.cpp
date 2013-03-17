
#include <Hord/Rule.hpp>

namespace Hord {

// class RuleState implementation

RuleState::RuleState() noexcept=default;
RuleState::RuleState(RuleState&&) noexcept=default;
RuleState::~RuleState() noexcept=default;

RuleState& RuleState::operator=(RuleState&&) noexcept=default;

// class Rule implementation

namespace {
static constexpr Object::type_info s_type_info{
	ObjectType::Rule
};
} // anonymous namespace

Object::type_info const& Rule::get_type_info_impl() const noexcept {
	return s_type_info;
}

Rule::Rule(Rule&&)=default;
Rule::~Rule() noexcept=default;

Rule& Rule::operator=(Rule&&)=default;

} // namespace Hord
