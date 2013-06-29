
#ifndef HORD_TEST_COMMON_DUMMIES_HPP_
#define HORD_TEST_COMMON_DUMMIES_HPP_

#include <Hord/IDGenerator.hpp>
#include <Hord/Datastore.hpp>

#include <new>
#include <random>
#include <utility>

class DummyIDGenerator
	: public Hord::IDGenerator
{
private:
	std::mt19937 m_rng{std::mt19937::default_seed};

public:
	DummyIDGenerator() = default;
	DummyIDGenerator(DummyIDGenerator&&) = default;
	~DummyIDGenerator() override = default;

private:
	void
	seed_impl(
		int64_t seed_value
	) noexcept override {
		m_rng.seed(static_cast<std::mt19937::result_type>(
			seed_value
		));
	}
	Hord::ObjectID
	generate_impl() noexcept override {
		Hord::ObjectID id;
		do { id = m_rng(); }
		while (Hord::OBJECT_NULL == id);
		return id;
	}
};

class DummyDatastore
	: public Hord::Datastore
{
private:
	using Hord::Datastore::type_info;
	using Hord::Datastore::State;

	std::iostream m_stream{nullptr};

	DummyDatastore() = delete;
	DummyDatastore(DummyDatastore const&) = delete;
	DummyDatastore(DummyDatastore&&) = delete;
	DummyDatastore& operator=(DummyDatastore const&) = delete;
	DummyDatastore& operator=(DummyDatastore&&) = delete;

public:
	static Datastore*
	construct(
		Hord::String root_path,
		Hord::HiveID const id
	) noexcept {
		return new(std::nothrow) DummyDatastore(std::move(root_path), id);
	}

	static type_info const s_type_info;

private:
	DummyDatastore(
		Hord::String root_path,
		Hord::HiveID const id
	)
		: Hord::Datastore(
			std::move(root_path),
			id
		)
	{}

public:
	~DummyDatastore() noexcept override = default;

private:
	void
	open_impl() override {
		Hord::Datastore::enable_state(State::opened);
	}
	void
	close_impl() override {
		Hord::Datastore::disable_state(State::opened);
	}

	std::istream&
	acquire_input_stream_impl(
		Hord::PropInfo const&
	) override {
		return m_stream;
	}
	std::ostream&
	acquire_output_stream_impl(
		Hord::PropInfo const&
	) override {
		return m_stream;
	}

	void
	release_input_stream_impl(
		Hord::PropInfo const&
	) override {}
	void
	release_output_stream_impl(
		Hord::PropInfo const&
	) override {}
};

template struct Hord::Datastore::ensure_traits<DummyDatastore>;

Hord::Datastore::type_info const
DummyDatastore::s_type_info{
	DummyDatastore::construct
};

Hord::Rule*
dummy_rule_type_construct(
	Hord::HiveID const,
	Hord::RuleID const
) {
	return nullptr;
}

#endif // HORD_TEST_COMMON_DUMMIES_HPP_
