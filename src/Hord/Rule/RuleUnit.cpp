/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <Hord/Rule/Unit.hpp>

namespace Hord {
namespace Rule {

// class Unit implementation

#define HORD_SCOPE_CLASS Rule::Unit

Unit::~Unit() noexcept = default;

Unit::Unit(Unit&&) = default;
Unit& Unit::operator=(Unit&&) = default;

Unit::Unit(
	Object::type_info const& tinfo,
	Rule::ID const id,
	Object::ID const parent
) noexcept
	: base(
		tinfo,
		id,
		parent
	)
{}

#undef HORD_SCOPE_CLASS

} // namespace Rule
} // namespace Hord
