
#include <Hord/utility.hpp>
#include <Hord/Cmd/Defs.hpp>
#include <Hord/Cmd/Stage.hpp>
#include <Hord/Cmd/Ops.hpp>

#include <ceformat/Format.hpp>
#include <ceformat/print.hpp>

#include <iostream>

namespace Hord {
namespace Cmd {

namespace {
static constexpr ceformat::Format const
	s_fmt_cmd_id_fields{"%08x[%c%c]"},
	s_fmt_cmd_identity{"%08x[%c%c]:%08x@%02x"}
;
} // anonymous namespace

std::ostream&
operator<<(
	std::ostream& stream,
	Cmd::IDFieldsPrinter const& printer
) {
	ceformat::write<s_fmt_cmd_id_fields>(
		stream,
		printer.fields.base,
		printer.fields.flag_host ? 'H' : 'C',
		printer.fields.flag_initiator ? 'I' : ' '
	);
	return stream;
}

std::ostream&
operator<<(
	std::ostream& stream,
	Cmd::Stage const& stage
) {
	ceformat::write<s_fmt_cmd_identity>(
		stream,
		stage.get_id_fields().base,
		stage.get_id_fields().flag_host ? 'H' : 'C',
		stage.get_id_fields().flag_initiator ? 'I' : ' ',
		enum_cast(stage.get_command_type()),
		enum_cast(stage.get_stage_type())
	);
	return stream;
}

} // namespace Cmd
} // namespace Hord
