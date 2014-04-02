
#include <Hord/serialization.hpp>
#include <Hord/Object/Defs.hpp>
#include <Hord/Hive/Unit.hpp>
#include <Hord/IO/Defs.hpp>
#include <Hord/IO/PropStream.hpp>

#include <duct/EndianUtils.hpp>
#include <duct/IO/arithmetic.hpp>

#include <cassert>
#include <limits>

#include <Hord/detail/gr_ceformat.hpp>

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

Unit::~Unit() noexcept = default;

Unit::Unit(Unit&&) = default;
Unit& Unit::operator=(Unit&&) = default;

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

#define HORD_SCOPE_FUNC deserialize_impl
namespace {
HORD_DEF_FMT_FQN(
	s_err_read_failed,
	HORD_SER_ERR_MSG_IO_PROP("read")
);
} // anonymous namespace

void
Unit::deserialize_impl(
	IO::InputPropStream& prop_stream
) try {
	auto ser = prop_stream.make_serializer();
	std::uint32_t count = 0;
	ser(count);

	// entries
	Object::ID id_block[ID_BLOCK_COUNT];
	std::size_t read_count = ID_BLOCK_COUNT;
	id_set_type des_idset{};
	des_idset.reserve(count);
	while (0u != count) {
		if (count < ID_BLOCK_COUNT) {
			read_count = count;
		}
		ser(Cacophony::make_sequence(id_block, read_count));
		des_idset.insert(id_block, id_block + read_count);
		count -= read_count;
	}

	// commit
	m_idset.operator=(std::move(des_idset));
} catch (SerializerError& serr) {
	HORD_THROW_SER_PROP(
		s_err_read_failed,
		serr,
		get_id(),
		"primary"
	);
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC serialize_impl
namespace {
HORD_DEF_FMT_FQN(
	s_err_write_failed,
	HORD_SER_ERR_MSG_IO_PROP("write")
);
} // anonymous namespace

void
Unit::serialize_impl(
	IO::OutputPropStream& prop_stream
) const try {
	auto ser = prop_stream.make_serializer();
	std::size_t count = m_idset.size();
	assert(std::numeric_limits<std::uint32_t>::max() >= count);
	ser(static_cast<std::uint32_t>(count));

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
		ser(Cacophony::make_sequence(id_block, write_count));
		count -= write_count;
	}

	// Should be at end after writing
	assert(m_idset.cend() == it);
} catch (SerializerError& serr) {
	HORD_THROW_SER_PROP(
		s_err_write_failed,
		serr,
		get_id(),
		"primary"
	);
}
#undef HORD_SCOPE_FUNC

#undef HORD_SCOPE_CLASS

} // namespace Hive
} // namespace Hord
