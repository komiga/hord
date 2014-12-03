
#ifndef HORD_TEST_COMMON_DUMMIES_HPP_
#define HORD_TEST_COMMON_DUMMIES_HPP_

#include <Hord/IO/Datastore.hpp>

#include <new>
#include <random>
#include <utility>

class DummyDatastore
	: public Hord::IO::Datastore
{
private:
	using base = Hord::IO::Datastore;
	using base::type_info;
	using base::State;

	std::iostream m_stream{nullptr};

	DummyDatastore() = delete;
	DummyDatastore(DummyDatastore const&) = delete;
	DummyDatastore(DummyDatastore&&) = delete;
	DummyDatastore& operator=(DummyDatastore const&) = delete;
	DummyDatastore& operator=(DummyDatastore&&) = delete;

public:
	static Datastore::UPtr
	construct(
		Hord::String root_path
	) noexcept {
		return Datastore::UPtr{
			new(std::nothrow) DummyDatastore(std::move(root_path))
		};
	}

	static type_info const s_type_info;

private:
	DummyDatastore(
		Hord::String root_path
	)
		: Hord::IO::Datastore(
			s_type_info,
			std::move(root_path)
		)
	{}

public:
	~DummyDatastore() noexcept override = default;

private:
	void
	open_impl(
		bool const /*create_if_nonexistent*/
	) override {
		base::enable_state(State::opened);
	}
	void
	close_impl() override {
		base::disable_state(State::opened);
	}

	std::istream&
	acquire_input_stream_impl(
		Hord::IO::PropInfo const&
	) override {
		return m_stream;
	}
	std::ostream&
	acquire_output_stream_impl(
		Hord::IO::PropInfo const&
	) override {
		return m_stream;
	}

	void
	release_input_stream_impl(
		Hord::IO::PropInfo const&
	) override {}
	void
	release_output_stream_impl(
		Hord::IO::PropInfo const&
	) override {}

	Hord::Object::ID
	generate_id_impl(
		Hord::System::IDGenerator&
	) const noexcept override {
		return Hord::Object::ID_NULL;
	}

	Hord::IO::Datastore::storage_info_map_type::const_iterator
	create_object_impl(
		Hord::Object::ID const,
		Hord::Object::type_info const&,
		Hord::IO::Linkage const
	) override {
		return get_storage_info().cend();
	}
	void
	destroy_object_impl(
		Hord::Object::ID const
	) override {}
};

template struct Hord::IO::Datastore::ensure_traits<DummyDatastore>;

Hord::IO::Datastore::type_info const
DummyDatastore::s_type_info{
	DummyDatastore::construct
};

Hord::Object::UPtr
dummy_rule_type_construct(
	Hord::Object::ID const,
	Hord::Object::ID const
) noexcept {
	return Hord::Object::UPtr{};
}

#endif // HORD_TEST_COMMON_DUMMIES_HPP_
