
//#include <Hord/config.hpp>
//#include <Hord/common_types.hpp>
//#include <Hord/common_enums.hpp>
//#include <Hord/aux.hpp>
#include <Hord/traits.hpp>
//#include <Hord/String.hpp>
#include <Hord/Error.hpp>
#include <Hord/MessageBuffer.hpp>
//#include <Hord/Metadata.hpp>
//#include <Hord/Record.hpp>
#include <Hord/Rule.hpp>
//#include <Hord/Column.hpp>
#include <Hord/Node.hpp>
#include <Hord/Hive.hpp>
//#include <Hord/Datastore.hpp>
//#include <Hord/IDGenerator.hpp>
#include <Hord/Driver.hpp>

#include <random>
#include <utility>
#include <iostream>
#include <iomanip>

class DummyIDGenerator
	: public Hord::IDGenerator {
private:
	std::mt19937 m_rng{std::mt19937::default_seed};

public:
	DummyIDGenerator()=default;
	DummyIDGenerator(DummyIDGenerator&&)=default;
	~DummyIDGenerator() override=default;

private:
	void seed_impl(int64_t seed_value) noexcept override {
		m_rng.seed(static_cast<std::mt19937::result_type>(
			seed_value
		));
	}
	Hord::ObjectID generate_impl() noexcept override {
		Hord::ObjectID id;
		do { id=m_rng(); } while (Hord::OBJECT_NULL==id);
		return id;
	}
};

class DummyDatastore
	: public Hord::Datastore {
private:
	using Hord::Datastore::type_info;
	using Hord::Datastore::State;

	std::iostream m_stream{nullptr};

public:
	static Datastore* construct(
		Hord::String root_path,
		Hord::HiveID const id
	) noexcept {
		try {
			return new DummyDatastore(std::move(root_path), (id));
		} catch (...) {
			return nullptr;
		}
	}

	static type_info const s_type_info;

private:
	DummyDatastore(Hord::String root_path, Hord::HiveID const id)
		: Hord::Datastore(
			std::move(root_path),
			id
		)
	{}
	DummyDatastore(DummyDatastore&&)=default;
	DummyDatastore& operator=(DummyDatastore&&)=default;
	~DummyDatastore() noexcept override=default;

private:
	void open_impl() override {
		Hord::Datastore::enable_state(State::opened);
	}
	void close_impl() override {
		Hord::Datastore::disable_state(State::opened);
	}

	std::istream& acquire_input_stream_impl(
		Hord::PropInfo const&
	) override {
		return m_stream;
	}
	std::ostream& acquire_output_stream_impl(
		Hord::PropInfo const&
	) override {
		return m_stream;
	}

	void release_input_stream_impl(
		Hord::PropInfo const&
	) override {}
	void release_output_stream_impl(
		Hord::PropInfo const&
	) override {}
};

Hord::Datastore::type_info const DummyDatastore::s_type_info{
	DummyDatastore::construct
};

void report_error(Hord::Error const& e) {
	std::cerr
		<<'['<<Hord::get_error_name(e.error_code())<<']'
		<<'\n'<<e.what_str()
	<<'\n'<<std::endl;
}

Hord::Rule* dummy_rule_type_construct(
	Hord::HiveID const,
	Hord::RuleID const
) {
	return nullptr;
}

int main() {
	// group driver
	Hord::Hive hive{};
	//Hord::IDGenerator id_generator{};
	DummyIDGenerator id_generator{};
	Hord::Driver driver{id_generator};

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
	Hord::Node node{hive.get_id(), Hord::NodeID{42}};

	// Registering rule types
	Hord::Rule::type_info const
		rti_standard{
			static_cast<Hord::RuleType>(
				Hord::StandardRuleTypes::ReservedLast
			),
			0u|static_cast<uint8_t>(Hord::FieldType::Text),
			dummy_rule_type_construct
		},
		rti_zero_permitted{
			1+
			static_cast<Hord::RuleType>(
				Hord::StandardRuleTypes::ReservedLast
			),
			0u,
			dummy_rule_type_construct
		},
		rti_valid{
			1+
			static_cast<Hord::RuleType>(
				Hord::StandardRuleTypes::ReservedLast
			),
			0u|static_cast<uint8_t>(Hord::FieldType::Text),
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
		std::cout<<"second register:"<<std::endl;
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
			<<"first hive id: "
			<<std::hex<<driver.placehold_hive(
				DummyDatastore::s_type_info,
				"./bork"
			).get_id()
		<<std::endl;
		driver.placehold_hive(
			DummyDatastore::s_type_info,
			"./bork"
		);
	} catch (Hord::Error& e) {
		report_error(e);
	}
	return 0;
}
