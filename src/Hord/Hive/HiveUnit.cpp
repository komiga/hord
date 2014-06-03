
#include <Hord/serialization.hpp>
#include <Hord/Object/Defs.hpp>
#include <Hord/Hive/Unit.hpp>
#include <Hord/IO/Defs.hpp>
#include <Hord/IO/PropStream.hpp>

#include <Hord/detail/gr_ceformat.hpp>

namespace Hord {
namespace Hive {

// class Unit implementation

#define HORD_SCOPE_CLASS Hive::Unit

Unit::~Unit() noexcept = default;

Unit::Unit(Unit&&) = default;
Unit& Unit::operator=(Unit&&) = default;

Unit::Unit(
	Object::type_info const& tinfo,
	Hive::ID const id
) noexcept
	: base(
		tinfo,
		id,
		Object::ID_NULL
	)
{}

// serialization

#define HORD_SCOPE_FUNC deserialize_impl
void
Unit::deserialize_impl(
	IO::InputPropStream& /*prop_stream*/
) {}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC serialize_impl
void
Unit::serialize_impl(
	IO::OutputPropStream& /*prop_stream*/
) const {}
#undef HORD_SCOPE_FUNC

#undef HORD_SCOPE_CLASS

} // namespace Hive
} // namespace Hord
