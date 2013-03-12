
#include <Hord/common_enums.hpp>
#include <Hord/Error.hpp>
#include <Hord/Hive.hpp>

namespace Hord {

// class Hive implementation

#define HORD_SCOPE_CLASS_IDENT__ Hive

namespace {
static constexpr Object::type_info s_type_info{
	ObjectType::Hive
};
} // anonymous namespace

Object::type_info const& Hive::get_type_info_impl() const noexcept {
	return s_type_info;
}

Hive::~Hive()=default;

void Hive::set_root(String root) {
#define HORD_SCOPE_FUNC_IDENT__ set_root
	if (root.empty()) {
		HORD_THROW_ERROR_SCOPED_FQN(
			ErrorCode::mutate_hive_root_empty,
			"cannot clear hive root path after construction"
		);
	}
	m_root.assign(std::move(root));
#undef HORD_SCOPE_FUNC_IDENT__
}

bool Hive::has_child(
	ObjectID const id
) const noexcept(noexcept(m_idset.find(id))) {
	return m_idset.cend()!=m_idset.find(id);
}

#undef HORD_SCOPE_CLASS_IDENT__

} // namespace Hord
