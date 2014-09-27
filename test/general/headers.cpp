
//#include <Hord/config.hpp>
//#include <Hord/aux.hpp>
#include <Hord/traits.hpp>
//#include <Hord/String.hpp>
#include <Hord/utility.hpp>
//#include <Hord/ErrorCode.hpp>
#include <Hord/Error.hpp>
#include <Hord/Log.hpp>
//#include <Hord/Data/Metadata.hpp>
//#include <Hord/Data/Field.hpp>
//#include <Hord/Data/Record.hpp>
//#include <Hord/Object/Unit.hpp>
#include <Hord/Object/Ops.hpp>
#include <Hord/Rule/Unit.hpp>
//#include <Hord/Node/Column.hpp>
#include <Hord/Node/UnitBasic.hpp>
#include <Hord/Hive/UnitBasic.hpp>
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
	auto hive = Hord::Hive::UnitBasic::info.construct(
		Hord::Hive::ID{42u},
		Hord::Object::ID_NULL
	);
	Hord::System::Driver driver{true};

	// group error
	Hord::Error const err_test{Hord::ErrorCode::unknown, "oh no!"};

	// group metadata
	Hord::Data::MetaField meta_field{};
	Hord::Data::Metadata metadata{};

	// group data
	Hord::Data::Field field{};
	Hord::Data::Record record{};

	// group node
	//Hord::Rule::State rule_state{};
	//Hord::Rule::Unit rule{};
	Hord::Node::Column column{};
	auto node = Hord::Node::UnitBasic::info.construct(
		Hord::Node::ID{42},
		hive->get_id()
	);

	// Registering rule types
	Hord::Object::type_info const
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

	// Placeholding hives
	auto hive_basic_type = Hord::Object::type_cast<Hord::Hive::UnitType>(
		Hord::Hive::UnitBasic::info.type
	);
	try {
		driver.placehold_hive(
			hive_basic_type,
			DummyDatastore::s_type_info,
			Hord::String{}
		);
	} catch (Hord::Error const& err) {
		Hord::Log::report_error(err);
	}

	std::cout
		<< "first hive id: "
		<< Hord::Object::IDPrinter{
			*driver.placehold_hive(
				hive_basic_type,
				DummyDatastore::s_type_info,
				"./bork"
			).hive
		}
	<< std::endl;
	try {
		driver.placehold_hive(
			hive_basic_type,
			DummyDatastore::s_type_info,
			"./bork"
		);
	} catch (Hord::Error const& err) {
		Hord::Log::report_error(err);
	}
	return 0;
}
