
#include <Hord/common_enums.hpp>
#include <Hord/Error.hpp>
#include <Hord/Driver.hpp>

#include <type_traits>
#include <algorithm>
#include <utility>
#include <chrono>

namespace Hord {

// class Driver implementation

#define HORD_SCOPE_CLASS_IDENT__ Driver

Driver::Driver(
	IDGenerator& id_generator
) noexcept
	: m_id_generator{id_generator}
{
	static_assert(
		std::is_same<int64_t, std::chrono::steady_clock::rep>::value,
		"steady_clock representation is not 64 bits!"
	);
	// Initialize generator
	m_id_generator.seed(
		std::chrono::steady_clock::now().time_since_epoch().count()
	);
	// TODO: Register standard rule types.
}

Driver::Driver(Driver&&)=default;
Driver::~Driver() noexcept=default;

#define HORD_SCOPE_FUNC_IDENT__ register_rule_type
void
Driver::register_rule_type(
	Rule::type_info const& type_info
) {
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
Hive const&
Driver::placehold_hive(
	Datastore::type_info const& type_info,
	String root_path
) {
	if (root_path.empty()) {
		HORD_THROW_ERROR_SCOPED_FQN(
			ErrorCode::driver_hive_root_empty,
			"cannot placehold hive with empty root path"
		);
	} else if (
		m_datastores.cend()
		!=std::find_if(m_datastores.cbegin(), m_datastores.cend(),
			[&root_path](datastore_map_type::value_type const& pair) -> bool {
				return 0==root_path.compare(pair.second->get_root_path());
			}
		)
	) {
		HORD_THROW_ERROR_SCOPED_FQN(
			ErrorCode::driver_hive_root_shared,
			"cannot placehold hive with non-unique root path"
		);
	}
	// Phew. Now let's try to construct and insert this guy
	ObjectID const id=m_id_generator.generate_unique(m_datastores);
	Datastore* const datastore_ptr
		=type_info.construct(std::move(root_path), id);
	if (nullptr==datastore_ptr) {
		HORD_THROW_ERROR_SCOPED_FQN(
			ErrorCode::driver_hive_root_shared,
			"cannot placehold hive with non-unique root path"
		);
	}
	auto result_pair=m_datastores.emplace(
		id,
		std::move(std::unique_ptr<Datastore>{
			datastore_ptr
		})
	);
	m_hive_order.emplace_back(id);
	return result_pair.first->second->get_hive();
}
#undef HORD_SCOPE_FUNC_IDENT__

#undef HORD_SCOPE_CLASS_IDENT__

} // namespace Hord
