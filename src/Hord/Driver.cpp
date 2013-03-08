
#include <Hord/Error.hpp>
#include <Hord/Driver.hpp>

#include <algorithm>

namespace Hord {

// class Driver implementation

#define HORD_SCOPE_CLASS_IDENT__ Driver

Hive& Driver::placehold_hive(String root) {
#define HORD_SCOPE_FUNC_IDENT__ placehold_hive
	if (root.empty()) {
		HORD_THROW_ERROR_SCOPED(
			ErrorCode::driver_hive_root_empty,
			"cannot placehold hive with empty root path"
		);
	} else if (
		m_hives.cend()!=std::find_if(m_hives.cbegin(), m_hives.cend(),
			[&root](Hive const& hive) -> bool {
				return 0==root.compare(hive.get_root());
			})
	) {
		HORD_THROW_ERROR_SCOPED_FQN(
			ErrorCode::driver_hive_root_shared,
			"cannot placehold hive with non-unique root path"
		);
	}
	m_hives.emplace_back(std::move(root));
	return m_hives.back();
#undef HORD_SCOPE_FUNC_IDENT__
}

#undef HORD_SCOPE_CLASS_IDENT__

} // namespace Hord
