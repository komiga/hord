
#include <Hord/Object/Defs.hpp>
#include <Hord/Hive/Defs.hpp>
#include <Hord/Hive/UnitBasic.hpp>
#include <Hord/Anchor/Defs.hpp>
#include <Hord/Anchor/UnitBasic.hpp>
#include <Hord/Node/Defs.hpp>
#include <Hord/Node/UnitBasic.hpp>
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

// NB: See note about stdlib defect in System/IDGenerator.cpp

Driver::~Driver() noexcept = default;

Driver::Driver(Driver&&) = default;

Driver::Driver(
	bool const register_standard_object_types
) /*noexcept*/
	: m_id_generator()
	, m_object_types()
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
	// TODO: Register standard rule types
	if (register_standard_object_types) {
		register_object_type(Hive::UnitBasic::info);
		register_object_type(Anchor::UnitBasic::info);
		register_object_type(Node::UnitBasic::info);
	}
}

#define HORD_SCOPE_FUNC register_object_type
void
Driver::register_object_type(
	Object::type_info const& type_info
) {
	if (m_object_types.cend() != m_object_types.find(type_info.type)) {
		HORD_THROW_FQN(
			ErrorCode::driver_object_type_shared,
			"type has already been registered"
		);
	}
	m_object_types.emplace(
		type_info.type,
		type_info
	);
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC get_object_type_info
Object::type_info const*
Driver::get_object_type_info(
	Object::Type const type
) const noexcept {
	auto const it = m_object_types.find(type);
	if (m_object_types.cend() != it) {
		return &it->second;
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
	Hive::Type const hive_type,
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
	auto const* const hive_tinfo = get_object_type_info(hive_type);
	if (nullptr == hive_tinfo) {
		HORD_THROW_FQN(
			ErrorCode::driver_hive_type_not_found,
			"failed to construct hive"
		);
	}
	Hive::ID const id = m_id_generator.generate_unique(m_hives);
	auto hive_ptr = hive_tinfo->construct(id, Object::ID_NULL);
	if (nullptr == datastore_ptr) {
		HORD_THROW_FQN(
			ErrorCode::driver_hive_construct_failed,
			"failed to construct hive"
		);
	}

	auto const& result_pair = m_hives.emplace(
		id,
		datastore_hive_pair{
			duct::cc_unique_ptr<IO::Datastore>{datastore_ptr},
			std::move(hive_ptr)
		}
	);
	return result_pair.first->second;
}
#undef HORD_SCOPE_FUNC

#undef HORD_SCOPE_CLASS

} // namespace System
} // namespace Hord
