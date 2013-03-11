
#include <Hord/Error.hpp>
#include <Hord/Driver.hpp>

#include <algorithm>
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
	auto tp=std::chrono::system_clock::now();
	m_id_generator.seed(
		std::chrono::duration_cast<std::chrono::milliseconds>(
			tp.time_since_epoch()
		).count()
	);
}

Hive& Driver::placehold_hive(String root) {
#define HORD_SCOPE_FUNC_IDENT__ placehold_hive
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
#undef HORD_SCOPE_FUNC_IDENT__
}

#undef HORD_SCOPE_CLASS_IDENT__

} // namespace Hord
