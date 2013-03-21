
#include <Hord/common_enums.hpp>
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

Hive::Hive()=default;

Hive::Hive(ObjectID const id) noexcept
	: Object{
		OBJECT_NULL==id
			? StorageState::null
			: StorageState::placeholder
		, OBJECT_NULL
		, id
	}
{}

Hive::Hive(Hive&&)=default;
Hive::~Hive() noexcept=default;

Hive& Hive::operator=(Hive&&)=default;

bool Hive::has_child(
	ObjectID const id
) const noexcept(noexcept(m_idset.find(id))) {
	return m_idset.cend()!=m_idset.find(id);
}

#undef HORD_SCOPE_CLASS_IDENT__

} // namespace Hord
