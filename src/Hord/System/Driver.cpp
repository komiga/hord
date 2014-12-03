
#include <Hord/Object/Defs.hpp>
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
	, m_datastores()
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

#define HORD_SCOPE_FUNC placehold_datastore
namespace {
HORD_DEF_FMT_FQN(
	s_err_root_shared,
	"cannot placehold datastore with non-unique root path `%s`"
);
} // anonymous namespace

IO::Datastore&
Driver::placehold_datastore(
	IO::Datastore::type_info const& type_info,
	String root_path
) {
	if (root_path.empty()) {
		HORD_THROW_FQN(
			ErrorCode::driver_datastore_root_empty,
			"cannot placehold datastore with empty root path"
		);
	} else if (find_datastore(root_path)) {
		HORD_THROW_FMT(
			ErrorCode::driver_datastore_root_shared,
			s_err_root_shared,
			root_path
		);
	}

	auto datastore_ptr = type_info.construct(std::move(root_path));
	if (nullptr == datastore_ptr) {
		HORD_THROW_FQN(
			ErrorCode::driver_datastore_construct_failed,
			"failed to construct datastore"
		);
	}
	auto const result_pair = m_datastores.emplace(
		datastore_ptr->get_id(),
		std::move(datastore_ptr)
	);
	return *result_pair.first->second;
}
#undef HORD_SCOPE_FUNC

#undef HORD_SCOPE_CLASS

} // namespace System
} // namespace Hord
