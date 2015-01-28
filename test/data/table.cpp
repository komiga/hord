
#include <Hord/String.hpp>
#include <Hord/Error.hpp>
#include <Hord/ErrorCode.hpp>
#include <Hord/utility.hpp>
#include <Hord/Data/Defs.hpp>
#include <Hord/Data/ValueRef.hpp>
#include <Hord/Data/TableSchema.hpp>
#include <Hord/Data/Table.hpp>

#include <duct/debug.hpp>

using namespace Hord;

bool
value_equal(
	Data::Table& table,
	unsigned row,
	unsigned col,
	Data::ValueRef const& value
) {
	auto it = table.begin();
	it += row;
	return value == it.get_field(col);
}

signed
main() {
	Data::Table table{};

	{
		Data::TableSchema schema{
			{"x", {Data::ValueType::integer, Data::Size::b16}}
		};
		DUCT_ASSERTE(table.configure(schema));
		Data::ValueRef values[1];
		values[0] = {static_cast<std::uint16_t>(~0u)};
		table.push_back(1, values);
		DUCT_ASSERTE(value_equal(table, 0, 0, values[0]));
		schema.columns()[0].index = 0;
		DUCT_ASSERTE(!table.configure(schema));
		DUCT_ASSERTE(value_equal(table, 0, 0, values[0]));
	}

	{
		auto schema = table.schema();
		auto& columns = schema.columns();
		columns[0].index = 0;
		columns[0].name = "y";
		schema.update();
		DUCT_ASSERTE(!table.configure(schema));
		columns[0].type = {Data::ValueType::integer, Data::Size::b8};
		schema.update();
		DUCT_ASSERTE(table.configure(schema));
		DUCT_ASSERTE(value_equal(table, 0, 0, {static_cast<std::uint8_t>(~0u)}));
	}

	{
		Data::TableSchema schema{
			{"x", {Data::ValueType::string, Data::Size::b32}}
		};
		DUCT_ASSERTE(table.configure(schema));
		DUCT_ASSERTE(table.empty());

		String str(512, 'V');
		Data::ValueRef values[1];
		values[0] = {str};
		table.push_back(1, values);

		auto& columns = schema.columns();
		columns[0].index = 0;
		columns[0].type = {Data::ValueType::string, Data::Size::b8};
		schema.update();
		DUCT_ASSERTE(table.configure(schema));
		str = String(static_cast<std::uint8_t>(~0u), 'V');
		DUCT_ASSERTE(value_equal(table, 0, 0, {str}));
	}

	{
		Data::TableSchema schema{};
		DUCT_ASSERTE(table.configure(schema));
		DUCT_ASSERTE(table.empty());
		DUCT_ASSERTE(!table.configure(schema));
	}

	try {
		Data::TableSchema schema{
			{"x", {Data::ValueType::null}}
		};
		schema.columns()[0].index = 0;
		table.configure(schema);
		DUCT_ASSERTE(false);
	} catch (Hord::Error const& err) {
		DUCT_ASSERTE(err.get_code() == ErrorCode::table_column_index_invalid);
	}

	try {
		Data::TableSchema const schema{
			{"", {Data::ValueType::null}}
		};
		table.configure(schema);
		DUCT_ASSERTE(false);
	} catch (Hord::Error const& err) {
		DUCT_ASSERTE(err.get_code() == ErrorCode::table_column_name_empty);
	}

	try {
		Data::TableSchema const schema{
			{"shared", {Data::ValueType::null}},
			{"shared", {Data::ValueType::null}}
		};
		table.configure(schema);
		DUCT_ASSERTE(false);
	} catch (Hord::Error const& err) {
		DUCT_ASSERTE(err.get_code() == ErrorCode::table_column_name_shared);
	}
	return 0;
}
