/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <Hord/System/IDGenerator.hpp>

#include <random>

namespace Hord {
namespace System {

// class IDGenerator implementation

// FIXME: Defect with the stdlib (as of N3485):
// Despite 26.5.3 [rand.eng] claiming no functions within the section
// throw unless otherwise specified and that *no* exceptions are
// specified for mersenne_twister_engine, its ctors are not marked
// noexcept.

// There is sadness. We are sad. This is a great crime.

IDGenerator::~IDGenerator() noexcept = default;

IDGenerator::IDGenerator() /*noexcept*/ = default;
IDGenerator::IDGenerator(IDGenerator&&) noexcept = default;
IDGenerator& IDGenerator::operator=(IDGenerator&&) noexcept = default;

void
IDGenerator::seed(
	std::int64_t seed_value
) noexcept {
	m_rng.seed(static_cast<std::mt19937::result_type>(
		seed_value
	));
}

} // namespace System
} // namespace Hord
