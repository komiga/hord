
#include <Hord/String.hpp>
#include <Hord/Object/Defs.hpp>
#include <Hord/Node/UnitBasic.hpp>

#include <Hord/detail/gr_ceformat.hpp>

namespace Hord {
namespace Node {

// class UnitBasic implementation

#define HORD_SCOPE_CLASS Node::UnitBasic

Object::UPtr
UnitBasic::construct(
	Object::ID const id,
	Object::ID const parent
) noexcept {
	return Object::UPtr{
		new(std::nothrow) Node::UnitBasic(id, parent)
	};
}

Object::type_info const&
UnitBasic::get_type_info_impl() const noexcept {
	return Node::UnitBasic::info;
}

UnitBasic::~UnitBasic() noexcept = default;

UnitBasic::UnitBasic(UnitBasic&&) = default;
UnitBasic& UnitBasic::operator=(UnitBasic&&) = default;

UnitBasic::UnitBasic(
	Node::ID const id,
	Object::ID const parent
) noexcept
	: base(
		Node::UnitBasic::info,
		static_cast<Object::ID>(id),
		parent
	)
{}

#undef HORD_SCOPE_CLASS

} // namespace Node
} // namespace Hord
