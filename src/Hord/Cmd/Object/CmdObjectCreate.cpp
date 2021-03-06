/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <Hord/utility.hpp>
#include <Hord/Log.hpp>
#include <Hord/IO/Defs.hpp>
#include <Hord/IO/Prop.hpp>
#include <Hord/IO/Datastore.hpp>
#include <Hord/Object/Ops.hpp>
#include <Hord/Schema/Defs.hpp>
#include <Hord/Schema/UnitTable.hpp>
#include <Hord/Table/Defs.hpp>
#include <Hord/Table/Unit.hpp>
#include <Hord/Cmd/Object.hpp>
#include <Hord/System/Driver.hpp>
#include <Hord/System/Context.hpp>

#include <exception>

#include <Hord/detail/gr_core.hpp>

namespace Hord {
namespace Cmd {
namespace Object {

#define HORD_SCOPE_CLASS Cmd::Object::Create

#define HORD_SCOPE_FUNC init_table // pseudo
static void
init_table(
	Hord::Table::Unit& table,
	Hord::Schema::ID schema_id,
	Hord::Schema::Unit const* schema_obj,
	bool schema_ref
) {
	if (schema_ref) {
		table.set_schema_ref(schema_id);
	}
	if (schema_obj && schema_obj->type() == Hord::Schema::Type{Hord::Schema::UnitType::Table}) {
		table.data().replace_schema(
			static_cast<Hord::Schema::UnitTable const*>(schema_obj)->data_schema()
		);
	}
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC commit_impl
HORD_CMD_IMPL_COMMIT_DEF(HORD_SCOPE_CLASS) {
	if (bad()) {
		m_object_id = Hord::Object::ID_NULL;
	}
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC exec // pseudo
HORD_SCOPE_CLASS::exec_result_type
HORD_SCOPE_CLASS::operator()(
	Hord::Object::Type type,
	Hord::Object::ID parent_id,
	Hord::Schema::ID schema_id,
	String const& slug,
	bool schema_ref
) noexcept try {
	auto& driver = this->driver();
	auto& datastore = this->datastore();

	// Validate
	auto const* parent_obj = datastore.find_ptr(parent_id);
	auto const* schema_obj = datastore.find_ptr(schema_id);
	if (!parent_id.is_null() && !parent_obj) {
		return commit_error("parent not found");
	} else if (!schema_id.is_null() && !schema_obj) {
		return commit_error("schema not found");
	} else if (!schema_id.is_null() && Hord::Object::BaseType::Schema != schema_obj->base_type()) {
		return commit_error("given schema ID is not a schema");
	} else if (slug.empty()) {
		return commit_error("slug empty");
	} else if (slug.size() > Hord::Object::SLUG_MAX_SIZE) {
		return commit_error("slug too long");
	} else {
		auto const& parent_children = parent_obj ? parent_obj->children() : datastore.root_objects();
		for (auto child_id : parent_children) {
			auto child_obj = datastore.find_ptr(child_id);
			if (child_obj && slug == child_obj->slug()) {
				return commit_error("slug already exists");
			}
		}
	}

	// Create
	auto const* tinfo = driver.find_object_type_info(type);
	if (nullptr == tinfo) {
		return commit_error("unknown unit type");
	}
	// NB: Leak ErrorCode::datastore_closed
	m_object_id = datastore.generate_id(driver.id_generator());
	auto obj = tinfo->construct(m_object_id, Hord::Object::ID_NULL);
	if (nullptr == obj) {
		return commit_error("allocation failed");
	}
	auto emplace_pair = datastore.objects().emplace(m_object_id, std::move(obj));
	if (!emplace_pair.second) {
		return commit_error("id already exists");
	}
	datastore.create_object(m_object_id, *tinfo, IO::Linkage::resident);

	// Base object properties
	auto& object = *emplace_pair.first->second;
	auto schema_obj_typed = static_cast<Hord::Schema::Unit const*>(schema_obj);
	Hord::Object::set_parent(object, datastore, parent_id);
	object.set_slug(slug);
	if (schema_obj_typed) {
		object.metadata().table().assign(schema_obj_typed->init_metadata().table());
	}

	// Base type-specific properties
	switch (type.base()) {
	case Hord::Object::BaseType::null:
	case Hord::Object::BaseType::Schema:
	case Hord::Object::BaseType::Anchor:
	case Hord::Object::BaseType::Rule:
		// Do nothing
		break;

	case Hord::Object::BaseType::Table:
		init_table(
			static_cast<Hord::Table::Unit&>(object),
			schema_id,
			schema_obj_typed,
			schema_ref
		);
		break;
	}
	object.prop_states().assign_all(IO::PropState::modified);
	if (object.parent().is_null()) {
		datastore.root_objects().emplace(m_object_id);
	}

	return commit();
} catch (Error const& err) {
	notify_exception_current();
	if (ErrorCode::datastore_object_already_exists == err.code()) {
		return commit_error("id already exists");
	}
	return commit_error("unknown error");
} catch (...) {
	notify_exception_current();
	return commit_error("unknown error");
}
#undef HORD_SCOPE_FUNC

#undef HORD_SCOPE_CLASS

} // namespace Object
} // namespace Cmd
} // namespace Hord
