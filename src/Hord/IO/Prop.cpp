/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <Hord/Object/Unit.hpp>
#include <Hord/IO/Prop.hpp>

namespace Hord {
namespace IO {

// class PropInfo implementation

// *sigh* ... Get modules already, C++.

PropInfo::PropInfo(
	Object::Unit const& object,
	IO::PropType const prop_type
) noexcept
	: object_id(object.get_id())
	, object_type(object.get_type())
	, prop_type(prop_type)
{}

} // namespace IO
} // namespace Hord
