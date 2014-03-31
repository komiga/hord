
#include <Hord/Rule/Defs.hpp>
#include <Hord/Cmd/Defs.hpp>
#include <Hord/Cmd/type_info.hpp>
#include <Hord/System/Driver.hpp>

#include <duct/cc_unique_ptr.hpp>

#include <type_traits>
#include <algorithm>
#include <utility>
#include <chrono>

#include <Hord/detail/gr_ceformat.hpp>

namespace Hord {
namespace System {

// class Driver implementation

#define HORD_SCOPE_CLASS System::Driver

namespace {
static Cmd::type_info const*
s_std_cmd_table_array[]{
	// Group: Generic
	&Cmd::s_type_info_GenericTerminate,

	// Group: Node
	&Cmd::s_type_info_NodeCreate
};
static Cmd::type_info_table const
s_std_cmd_table{
	s_std_cmd_table_array,
	Cmd::Type::STANDARD_BASE,
	Cmd::Type::STANDARD_END
};
}// anonymous namespace

static_assert(
	static_cast<unsigned>(Cmd::Type::STANDARD_COUNT_DEFINED)
	== std::extent<decltype(s_std_cmd_table_array)>::value,
	"standard command table is incomplete"
);

// NB: See note about stdlib defect in System/IDGenerator.cpp

Driver::Driver() /*noexcept*/
	: m_id_generator()
	, m_rule_types()
	, m_command_tables({&s_std_cmd_table})
	, m_hives()
{
	static_assert(
		8u == sizeof(std::chrono::steady_clock::rep),
		"steady_clock representation is not 64 bits!"
	);

	// Initialize generator
	m_id_generator.seed(
		std::chrono::steady_clock::now().time_since_epoch().count()
	);
	// TODO: Register standard rule types.
}

Driver::Driver(Driver&&) = default;
Driver::~Driver() noexcept = default;

#define HORD_SCOPE_FUNC register_rule_type
void
Driver::register_rule_type(
	Rule::type_info const& type_info
) {
	if (
		static_cast<Rule::Type>(Rule::StandardTypes::ReservedLast)
		>= type_info.type
	) {
		HORD_THROW_FQN(
			ErrorCode::driver_rule_type_reserved,
			"type is within range reserved for standard rules"
		);
	} else if (0u == type_info.permitted_types) {
		HORD_THROW_FQN(
			ErrorCode::driver_rule_type_zero_permitted_types,
			"permitted_types property must be a nonzero combination"
			" of FieldTypes"
		);
	} else if (m_rule_types.cend() != m_rule_types.find(type_info.type)) {
		HORD_THROW_FQN(
			ErrorCode::driver_rule_type_shared,
			"type has already been registered"
		);
	}
	m_rule_types.emplace(
		type_info.type,
		type_info
	);
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC get_rule_type_info
Rule::type_info const*
Driver::get_rule_type_info(
	Rule::Type const type
) const noexcept {
	auto const it = m_rule_types.find(type);
	if (m_rule_types.cend() != it) {
		return &it->second;
	}
	return nullptr;
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC register_command_type_table
void
Driver::register_command_type_table(
	Cmd::type_info_table const& table
) {
	if (table.range_begin > table.range_end) {
		HORD_THROW_FQN(
			ErrorCode::driver_command_table_range_invalid,
			"type range is invalid: range_begin > range_end"
		);
	} else if (table.range_begin == table.range_end) {
		HORD_THROW_FQN(
			ErrorCode::driver_command_table_range_invalid,
			"type range is invalid: size() == 0"
		);
	} else if (table.range_begin < Cmd::Type::STANDARD_BASE) {
		HORD_THROW_FQN(
			ErrorCode::driver_command_table_range_invalid,
			"type range is invalid: intersects the range reserved for"
			" stage types"
		);
	} else if (table.range_begin < Cmd::Type::USERSPACE_BASE) {
		HORD_THROW_FQN(
			ErrorCode::driver_command_table_range_invalid,
			"type range is invalid: intersects the range reserved for"
			" standard command types"
		);
	}
	for (auto const& registered_table : m_command_tables) {
		if (table.intersects(*registered_table)) {
			HORD_THROW_FQN(
				ErrorCode::driver_command_table_range_shared,
				"type range intersects with type range of previously-"
				"registered table"
			);
		}
	}
	m_command_tables.emplace_back(&table);
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC get_command_type_info
Cmd::type_info const*
Driver::get_command_type_info(
	Cmd::Type const type
) const noexcept {
	for (auto const& table : m_command_tables) {
		if (table->contains(type)) {
			return table->array[
				static_cast<std::size_t>(type) -
				static_cast<std::size_t>(table->range_begin)
			];
		}
	}
	return nullptr;
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC placehold_hive
namespace {
HORD_DEF_FMT_FQN(
	s_err_root_shared,
	"cannot placehold hive with non-unique root path `%s`"
);
} // anonymous namespace

System::Driver::datastore_hive_pair&
Driver::placehold_hive(
	IO::Datastore::type_info const& type_info,
	String root_path
) {
	if (root_path.empty()) {
		HORD_THROW_FQN(
			ErrorCode::driver_hive_root_empty,
			"cannot placehold hive with empty root path"
		);
	} else if (
		// FIXME: This is rank.
		m_hives.cend()
		!= std::find_if(m_hives.cbegin(), m_hives.cend(),
			[&root_path](hive_map_type::value_type const& pair) -> bool {
				return 0 == root_path.compare(
					pair.second.datastore->get_root_path()
				);
			}
		)
	) {
		HORD_THROW_FMT(
			ErrorCode::driver_hive_root_shared,
			s_err_root_shared,
			root_path
		);
	}

	// Phew. Now let's try to construct and insert this guy
	IO::Datastore* const
		datastore_ptr = type_info.construct(std::move(root_path));
	if (nullptr == datastore_ptr) {
		HORD_THROW_FQN(
			ErrorCode::driver_datastore_construct_failed,
			"failed to construct datastore"
		);
	}

	Object::ID const id = m_id_generator.generate_unique(m_hives);
	auto const& result_pair = m_hives.emplace(
		id,
		datastore_hive_pair{
			duct::cc_unique_ptr<IO::Datastore>{datastore_ptr},
			Hive::Unit{id}
		}
	);
	return result_pair.first->second;
}
#undef HORD_SCOPE_FUNC

#undef HORD_SCOPE_CLASS

} // namespace System
} // namespace Hord
