
#include <Hord/Datastore.hpp>

// TODO: Base exceptions

namespace Hord {

// class Datastore implementation

#define HORD_SCOPE_CLASS_IDENT__ Datastore

Datastore::Datastore(Hive const& hive) noexcept
	: m_hive{hive}
{}

Datastore::Datastore(Datastore&&)=default;
Datastore::~Datastore()=default;
Datastore& Datastore::operator=(Datastore&&)=default;

#define HORD_SCOPE_FUNC_IDENT__ open
void open() {

}
#undef HORD_SCOPE_FUNC_IDENT__

#define HORD_SCOPE_FUNC_IDENT__ close
void close() {

}
#undef HORD_SCOPE_FUNC_IDENT__

#define HORD_SCOPE_FUNC_IDENT__ acquire_input_stream
std::istream& Datastore::acquire_input_stream(
	PropInfo const& prop_info
) {
	return acquire_input_stream_impl(prop_info);
}
#undef HORD_SCOPE_FUNC_IDENT__

#define HORD_SCOPE_FUNC_IDENT__ acquire_output_stream
std::ostream& Datastore::acquire_output_stream(
	PropInfo const& prop_info
) {
	return acquire_output_stream_impl(prop_info);
}
#undef HORD_SCOPE_FUNC_IDENT__

#define HORD_SCOPE_FUNC_IDENT__ release_input_stream
void Datastore::release_input_stream(
	PropInfo const& prop_info
) {
	return release_input_stream_impl(prop_info);
}
#undef HORD_SCOPE_FUNC_IDENT__

#define HORD_SCOPE_FUNC_IDENT__ release_output_stream
void Datastore::release_output_stream(
	PropInfo const& prop_info
) {
	return release_output_stream_impl(prop_info);
}
#undef HORD_SCOPE_FUNC_IDENT__

#undef HORD_SCOPE_CLASS_IDENT__

} // namespace Hord
