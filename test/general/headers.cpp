
//#include <Hord/config.hpp>
//#include <Hord/aux.hpp>
#include <Hord/traits.hpp>
//#include <Hord/String.hpp>
#include <Hord/utility.hpp>
//#include <Hord/ErrorCode.hpp>
#include <Hord/Error.hpp>
#include <Hord/Msg/Buffer.hpp>
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

#include "../common/common.hpp"
#include "../common/dummies.hpp"

using Hord::enum_cast;

signed
main() {
	// group driver
	auto hive = Hord::Hive::UnitBasic::info.construct(
		Hord::Hive::NULL_ID,
		Hord::Object::NULL_ID
	);
	Hord::System::Driver driver{true};

	// group error
	Hord::Error err{Hord::ErrorCode::unknown, "oh no!"};

	// group metadata
	//Hord::MetaField meta_field{};
	Hord::Data::StringMetaField meta_field_1{};
	Hord::Data::Int32MetaField meta_field_2{};
	Hord::Data::Int64MetaField meta_field_3{};
	Hord::Data::BoolMetaField meta_field_4{};
	Hord::Data::Metadata metadata{};

	Hord::Data::MetaField& mf = meta_field_1;
	std::cout
		<< static_cast<int>(mf.get_type_info().type) << ','
		<< static_cast<int>(meta_field_2.get_type_info().type) << ','
		<< static_cast<int>(meta_field_3.get_type_info().type) << ','
		<< static_cast<int>(meta_field_4.get_type_info().type)
	<< std::endl;

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
	Hord::Rule::type_info const
		rti_standard{
			Hord::Object::type_info{
				"Hord:test:rti_standard",
				Hord::Rule::Type{Hord::Rule::UnitType::null},
				{false, false},
				dummy_rule_type_construct
			},
			enum_cast(Hord::Data::FieldType::Text)
		},
		rti_zero_permitted{
			Hord::Object::type_info{
				"Hord:test:rti_zero_permitted",
				Hord::Rule::Type{Hord::Rule::UnitType::Composition},
				{false, false},
				dummy_rule_type_construct
			},
			0u
		},
		rti_valid{
			Hord::Object::type_info{
				"Hord:test:rti_valid",
				Hord::Rule::Type{Hord::Rule::UnitType::Composition},
				{false, false},
				dummy_rule_type_construct
			},
			enum_cast(Hord::Data::FieldType::Text)
		}
	;

	try {
		driver.register_rule_type(rti_standard);
	} catch (Hord::Error& e) {
		report_error(e);
	}
	try {
		driver.register_rule_type(rti_zero_permitted);
	} catch (Hord::Error& e) {
		report_error(e);
	}
	try {
		driver.register_rule_type(rti_valid);
		std::cout << "second register:" << std::endl;
		driver.register_rule_type(rti_valid);
	} catch (Hord::Error& e) {
		report_error(e);
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
	} catch (Hord::Error& e) {
		report_error(e);
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
	} catch (Hord::Error& e) {
		report_error(e);
	}
	return 0;
}
