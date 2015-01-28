
//#include <Hord/config.hpp>
//#include <Hord/aux.hpp>
#include <Hord/traits.hpp>
//#include <Hord/String.hpp>
#include <Hord/utility.hpp>
//#include <Hord/ErrorCode.hpp>
#include <Hord/Error.hpp>
#include <Hord/Log.hpp>
#include <Hord/Data/Defs.hpp>
#include <Hord/Data/Metadata.hpp>
#include <Hord/Data/Table.hpp>
//#include <Hord/Object/Unit.hpp>
#include <Hord/Object/Ops.hpp>
#include <Hord/Rule/Unit.hpp>
#include <Hord/Table/UnitBasic.hpp>
//#include <Hord/IO/Datastore.hpp>
//#include <Hord/System/IDGenerator.hpp>
#include <Hord/System/Driver.hpp>

#include <utility>
#include <iostream>
#include <iomanip>

#include "../common/dummies.hpp"

using Hord::enum_cast;

signed
main() {
	// group driver
	Hord::System::Driver driver{true};

	// group error
	Hord::Error const err_test{Hord::ErrorCode::unknown, "oh no!"};

	// group metadata
	Hord::Data::Metadata metadata{};
	Hord::Data::ValueRef vr{"abacaba"};

	// group data
	Hord::Data::TableSchema table_schema{};
	Hord::Data::Table table{table_schema};

	// group node
	//Hord::Rule::State rule_state{};
	//Hord::Rule::Unit rule{};
	auto node = Hord::Table::UnitBasic::info.construct(
		Hord::Table::ID{42},
		Hord::Object::ID_NULL
	);

	// Registering rule types
	Hord::Object::TypeInfo const
	ti_standard{
		"Hord:test:ti_standard",
		Hord::Rule::Type{Hord::Rule::UnitType::null},
		{false, false},
		dummy_rule_type_construct
	},
	ti_zero_permitted{
		"Hord:test:ti_zero_permitted",
		Hord::Rule::Type{Hord::Rule::UnitType::Composition},
		{false, false},
		dummy_rule_type_construct
	},
	ti_valid{
		"Hord:test:ti_valid",
		Hord::Rule::Type{Hord::Rule::UnitType::Composition},
		{false, false},
		dummy_rule_type_construct
	};

	try {
		driver.register_object_type(ti_standard);
	} catch (Hord::Error const& err) {
		Hord::Log::report_error(err);
	}
	try {
		driver.register_object_type(ti_zero_permitted);
	} catch (Hord::Error const& err) {
		Hord::Log::report_error(err);
	}
	try {
		driver.register_object_type(ti_valid);
		std::cout << "second register:" << std::endl;
		driver.register_object_type(ti_valid);
	} catch (Hord::Error const& err) {
		Hord::Log::report_error(err);
	}

	// Placeholding datastores
	try {
		driver.placehold_datastore(
			DummyDatastore::s_type_info,
			Hord::String{}
		);
	} catch (Hord::Error const& err) {
		Hord::Log::report_error(err);
	}

	std::cout
		<< "first datastore id: "
		<< driver.placehold_datastore(DummyDatastore::s_type_info, "./bork").id()
	<< std::endl;
	try {
		driver.placehold_datastore(DummyDatastore::s_type_info, "./bork");
	} catch (Hord::Error const& err) {
		Hord::Log::report_error(err);
	}
	return 0;
}
