
#include <Hord/Error.hpp>
#include <Hord/Driver.hpp>

#include <algorithm>

namespace Hord {

// class Driver implementation

Hive& Driver::placehold_hive(String root) {
	if (root.empty()) {
		throw Error{ErrorCode::driver_hive_root_empty, HORD_STR_LIT("Driver: Cannot placehold hive with empty root path")};
	} else if (
		m_hives.cend()!=std::find_if(m_hives.cbegin(), m_hives.cend(),
			[&root](Hive const& hive) -> bool { return 0==root.compare(hive.get_root()); })
	) {
		throw Error{ErrorCode::driver_hive_root_shared, HORD_STR_LIT("Driver: Cannot placehold hive with non-unique root path")};
	}
	m_hives.emplace_back(std::move(root));
	return m_hives.back();
}

} // namespace Hord
