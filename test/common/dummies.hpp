
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
	static Datastore*
	construct(
		Hord::String root_path
	) noexcept {
		return new(std::nothrow) DummyDatastore(
			std::move(root_path)
		);
	}

	static type_info const s_type_info;

private:
	DummyDatastore(
		Hord::String root_path
	)
		: Hord::IO::Datastore(
			std::move(root_path)
		)
	{}

public:
	~DummyDatastore() noexcept override = default;

private:
	void
	open_impl() override {
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
};

template struct Hord::IO::Datastore::ensure_traits<DummyDatastore>;

Hord::IO::Datastore::type_info const
DummyDatastore::s_type_info{
	DummyDatastore::construct
};

Hord::Rule::Unit*
dummy_rule_type_construct(
	Hord::Hive::ID const,
	Hord::Rule::ID const
) noexcept {
	return nullptr;
}

#endif // HORD_TEST_COMMON_DUMMIES_HPP_
