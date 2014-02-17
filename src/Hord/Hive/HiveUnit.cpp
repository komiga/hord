
#include <Hord/detail/gr_ceformat.hpp>
#include <Hord/Object/Defs.hpp>
#include <Hord/Hive/Unit.hpp>
#include <Hord/IO/Defs.hpp>
#include <Hord/IO/PropStream.hpp>

#include <duct/EndianUtils.hpp>
#include <duct/IO/arithmetic.hpp>

namespace Hord {
namespace Hive {

// class Unit implementation

#define HORD_SCOPE_CLASS Hive::Unit

namespace {
static constexpr Object::type_info const
s_type_info{
	Object::Type::Hive,
	true,
	false
};
} // anonymous namespace

Object::type_info const&
Unit::get_type_info_impl() const noexcept {
	return s_type_info;
}

Unit::Unit()
	: base(
		s_type_info.supplies_prop_primary,
		s_type_info.supplies_prop_auxiliary,
		static_cast<Object::ID>(Hive::NULL_ID),
		Object::NULL_ID
	)
{}

Unit::Unit(
	Hive::ID const id
) noexcept
	: base(
		s_type_info.supplies_prop_primary,
		s_type_info.supplies_prop_auxiliary,
		static_cast<Object::ID>(id),
		Object::NULL_ID
	)
{}

Unit::Unit(Unit&&) = default;
Unit::~Unit() noexcept = default;

Unit& Unit::operator=(Unit&&) = default;

bool
Unit::has_child(
	Object::ID const id
) const noexcept(noexcept(m_idset.find(id))) {
	return m_idset.cend() != m_idset.find(id);
}

// serialization

// NB: Hive only supplies the primary prop, so we rely on
// base to ensure we only ever receive that prop.

namespace {
enum : std::size_t {
	// 2KB block
	ID_BLOCK_COUNT = 512u
};
} // anonymous namespace

#define HORD_HIVE_CHECK_IO_ERROR__(err__)						\
	if (stream.fail()) {										\
		HORD_THROW_FMT(											\
			ErrorCode::serialization_io_failed,					\
			err__,												\
			get_id()											\
		);														\
	}
//

#define HORD_SCOPE_FUNC deserialize_impl
namespace {
HORD_DEF_FMT_FQN(
	s_err_read_failed,
	"failed to deserialize prop %08x -> primary: read error"
);
} // anonymous namespace

void
Unit::deserialize_impl(
	IO::InputPropStream& prop_stream
) {
	std::istream& stream = prop_stream.get_stream();

	// header
	std::size_t count
	= duct::IO::read_arithmetic<uint32_t>(
		stream,
		duct::Endian::little
	);

	HORD_HIVE_CHECK_IO_ERROR__(s_err_read_failed);

	// entries
	Object::ID id_block[ID_BLOCK_COUNT];
	std::size_t read_count = ID_BLOCK_COUNT;
	id_set_type des_idset{};
	des_idset.reserve(count);
	while (0u != count) {
		if (count < ID_BLOCK_COUNT) {
			read_count = count;
		}
		duct::IO::read_arithmetic_array(
			stream,
			id_block,
			read_count,
			duct::Endian::little
		);
		HORD_HIVE_CHECK_IO_ERROR__(s_err_read_failed);

		des_idset.insert(id_block, id_block + read_count);
		count -= read_count;
	}

	// commit
	m_idset.operator=(std::move(des_idset));
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC serialize_impl
namespace {
HORD_DEF_FMT_FQN(
	s_err_write_failed,
	"failed to serialize prop %08x -> primary: write error"
);
} // anonymous namespace

void
Unit::serialize_impl(
	IO::OutputPropStream& prop_stream
) const {
	std::ostream& stream = prop_stream.get_stream();

	// header
	std::size_t count = m_idset.size();
	duct::IO::write_arithmetic(
		stream,
		count,
		duct::Endian::little
	);

	HORD_HIVE_CHECK_IO_ERROR__(s_err_write_failed);

	// entries
	Object::ID id_block[ID_BLOCK_COUNT];
	std::size_t write_count = ID_BLOCK_COUNT;
	std::size_t copy_count = 0u;
	id_set_type::const_iterator it = m_idset.cbegin();
	while (0u != count) {
		if (count < ID_BLOCK_COUNT) {
			write_count = count;
		}
		for (
			copy_count = write_count;
			0u != copy_count;
			--copy_count, ++it
		) {
			// Should not be at end if still copying
			assert(m_idset.cend() != it);
			id_block[write_count - copy_count] = *it;
		}

		duct::IO::write_arithmetic_array(
			stream,
			id_block,
			write_count,
			duct::Endian::little
		);
		HORD_HIVE_CHECK_IO_ERROR__(s_err_write_failed);

		count -= write_count;
	}

	// Should be at end after writing
	assert(m_idset.cend() == it);
}
#undef HORD_SCOPE_FUNC

#undef HORD_SCOPE_CLASS

} // namespace Hive
} // namespace Hord
