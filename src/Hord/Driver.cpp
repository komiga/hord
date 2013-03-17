
#include <Hord/common_enums.hpp>
#include <Hord/Error.hpp>
#include <Hord/Driver.hpp>

#include <algorithm>
#include <utility>
#include <chrono>

namespace Hord {

// class Driver implementation

#define HORD_SCOPE_CLASS_IDENT__ Driver

Driver::Driver(Serializer& serializer, IDGenerator& id_generator) noexcept
	: m_serializer{serializer}
	, m_id_generator{id_generator}
{
	// Initialize generator
	// FIXME: libstdc++ 4.6.3 does not supply steady_clock.
	auto const tp=std::chrono::system_clock::now();
	m_id_generator.seed(
		std::chrono::duration_cast<std::chrono::milliseconds>(
			tp.time_since_epoch()
		).count()
	);
	// TODO: Register standard rule types.
}

Driver::Driver(Driver&&)=default;
Driver::~Driver() noexcept=default;

#define HORD_SCOPE_FUNC_IDENT__ register_rule_type
void Driver::register_rule_type(Rule::type_info const& type_info) {
	if (
		static_cast<RuleType>(StandardRuleTypes::ReservedLast)
		>=type_info.type
	) {
		HORD_THROW_ERROR_SCOPED_FQN(
			ErrorCode::driver_rule_type_reserved,
			"type is within range reserved for standard rules"
		);
	} else if (0u==type_info.permitted_types) {
		HORD_THROW_ERROR_SCOPED_FQN(
			ErrorCode::driver_rule_type_zero_permitted_types,
			"permitted_types property must be a nonzero combination"
			" of FieldTypes"
		);
	} else if (m_rule_types.cend()!=m_rule_types.find(type_info.type)) {
		HORD_THROW_ERROR_SCOPED_FQN(
			ErrorCode::driver_rule_type_shared,
			"type has already been registered"
		);
	}
	m_rule_types.insert(std::move(std::make_pair(
		type_info.type,
		type_info
	)));
}
#undef HORD_SCOPE_FUNC_IDENT__

#define HORD_SCOPE_FUNC_IDENT__ placehold_hive
Hive& Driver::placehold_hive(String root) {
	if (root.empty()) {
		HORD_THROW_ERROR_SCOPED_FQN(
			ErrorCode::driver_hive_root_empty,
			"cannot placehold hive with empty root path"
		);
	} else if (
		m_hives.cend()!=std::find_if(m_hives.cbegin(), m_hives.cend(),
			[&root](hive_map_type::value_type const& pair) -> bool {
				return 0==root.compare(pair.second.get_root());
			})
	) {
		HORD_THROW_ERROR_SCOPED_FQN(
			ErrorCode::driver_hive_root_shared,
			"cannot placehold hive with non-unique root path"
		);
	}
	// FIXME: libstdc++ 4.6.3 cannot has the emplace!
	// By Jaal's gnarled neck, does 4.6.3 have anything?
	ObjectID const id=m_id_generator.generate_unique(m_hives);
	m_hive_order.emplace_back(id);
	auto result_pair=m_hives.insert(std::move(std::make_pair(
		id,
		Hive{id, std::move(root)}
	)));
	return result_pair.first->second;
}
#undef HORD_SCOPE_FUNC_IDENT__

#undef HORD_SCOPE_CLASS_IDENT__

} // namespace Hord
