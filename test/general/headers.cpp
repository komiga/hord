
//#include <Hord/config.hpp>
//#include <Hord/aux.hpp>
#include <Hord/traits.hpp>
//#include <Hord/String.hpp>
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
#include <Hord/Node/Unit.hpp>
#include <Hord/Hive/Unit.hpp>
//#include <Hord/IO/Datastore.hpp>
//#include <Hord/System/IDGenerator.hpp>
#include <Hord/System/Driver.hpp>

#include <utility>
#include <iostream>
#include <iomanip>

#include "../common/common.hpp"
#include "../common/dummies.hpp"

signed
main() {
	// group driver
	Hord::Hive::Unit hive{};
	Hord::System::Driver driver{};

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
	Hord::Node::Unit node{hive.get_id(), Hord::Node::ID{42}};

	// Registering rule types
	Hord::Rule::type_info const
		rti_standard{
			static_cast<Hord::Rule::Type>(
				Hord::Rule::StandardTypes::ReservedLast
			),
			0u | static_cast<std::uint8_t>(Hord::Data::FieldType::Text),
			dummy_rule_type_construct
		},
		rti_zero_permitted{
			1 + static_cast<Hord::Rule::Type>(
				Hord::Rule::StandardTypes::ReservedLast
			),
			0u,
			dummy_rule_type_construct
		},
		rti_valid{
			1 + static_cast<Hord::Rule::Type>(
				Hord::Rule::StandardTypes::ReservedLast
			),
			0u | static_cast<std::uint8_t>(Hord::Data::FieldType::Text),
			dummy_rule_type_construct
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
	try {
		driver.placehold_hive(
			DummyDatastore::s_type_info,
			Hord::String{}
		);
	} catch (Hord::Error& e) {
		report_error(e);
	}
	try {
		std::cout
			<< "first hive id: "
			<< Hord::Object::IDPrinter{
				driver.placehold_hive(
					DummyDatastore::s_type_info,
					"./bork"
				).hive
			}
		<< std::endl;
		driver.placehold_hive(
			DummyDatastore::s_type_info,
			"./bork"
		);
	} catch (Hord::Error& e) {
		report_error(e);
	}
	return 0;
}
