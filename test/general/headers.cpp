
//#include <Hord/config.hpp>
//#include <Hord/common_types.hpp>
//#include <Hord/common_enums.hpp>
//#include <Hord/aux.hpp>
//#include <Hord/String.hpp>
#include <Hord/Error.hpp>
//#include <Hord/Metadata.hpp>
//#include <Hord/Record.hpp>
//#include <Hord/Rule.hpp>
//#include <Hord/Column.hpp>
//#include <Hord/Node.hpp>
//#include <Hord/Hive.hpp>
//#include <Hord/IDGenerator.hpp>
//#include <Hord/Serializer.hpp>
#include <Hord/Driver.hpp>

#include <iostream>

class DummySerializer : public Hord::Serializer {
public:
	DummySerializer()=default;
	DummySerializer(DummySerializer&&)=default;
	~DummySerializer()=default;

private:
	void serialize_hive_impl(Hord::Hive const&) override {}
	void serialize_node_impl(Hord::Node const&) override {}
	void serialize_rule_impl(Hord::Rule const&) override {}
	void deserialize_hive_impl(Hord::Hive&) override {}
	void deserialize_node_impl(Hord::Node&) override {}
	void deserialize_rule_impl(Hord::Rule&) override {}
};

class DummyIDGenerator : public Hord::IDGenerator {
public:
	DummyIDGenerator()=default;
	DummyIDGenerator(DummyIDGenerator&&)=default;
	~DummyIDGenerator()=default;

private:
	void seed_impl(int) noexcept override { return; }
	Hord::ObjectID generate_impl() noexcept override { return Hord::OBJECT_NULL; }
};

int main() {
	// group driver
	Hord::Hive hive{};
	//Hord::IDGenerator id_generator{};
	//Hord::Serializer serializer{};
	DummySerializer serializer{};
	DummyIDGenerator id_generator{};
	Hord::Driver driver{serializer, id_generator};

	// group error
	Hord::Error err{Hord::ErrorCode::unknown, "oh no!"};

	// group metadata
	//Hord::MetaField meta_field{};
	Hord::StringMetaField meta_field_1{};
	Hord::Int32MetaField meta_field_2{};
	Hord::Int64MetaField meta_field_3{};
	Hord::BoolMetaField meta_field_4{};
	Hord::Metadata metadata{};

	Hord::MetaField& mf=meta_field_1;
	std::cout
		<<static_cast<int>(mf.get_type_info().type)<<','
		<<static_cast<int>(meta_field_2.get_type_info().type)<<','
		<<static_cast<int>(meta_field_3.get_type_info().type)<<','
		<<static_cast<int>(meta_field_4.get_type_info().type)
	<<std::endl;

	// group data
	Hord::Field field{};
	Hord::Record record{};

	// group node
	//Hord::RuleState rule_state{};
	//Hord::Rule rule{};
	Hord::Column column{};
	Hord::Node node{hive, Hord::NodeID{42}};

	try {
		driver.placehold_hive("./bork");
		driver.placehold_hive("./bork");
	} catch (Hord::Error& e) {
		std::cerr
			<<Hord::get_error_name(e.error_code())
			<<": "<<e.what_str()
		<<std::endl;
	}
	return 0;
}
