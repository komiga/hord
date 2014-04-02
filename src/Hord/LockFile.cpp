
#include <Hord/LockFile.hpp>

#include <cstdlib>
#include <cstdio>
#include <utility>

#include <unistd.h>
#include <sys/file.h>

#include <Hord/detail/gr_ceformat.hpp>

namespace Hord {

// class LockFile implementation

#define HORD_SCOPE_CLASS LockFile

LockFile::~LockFile() {
	release();
}

LockFile::LockFile(
	String path
) noexcept
	: m_handle(NULL_HANDLE)
	, m_path(std::move(path))
{}

#define HORD_SCOPE_FUNC set_path
namespace {
HORD_DEF_FMT_FQN(
	s_err_immutable,
	"cannot change path to `%s` while lock is active"
);
} // anonymous namespace

void
LockFile::set_path(
	String path
) {
	if (is_active()) {
		HORD_THROW_FMT(
			ErrorCode::lockfile_immutable,
			s_err_immutable,
			m_path
		);
	} else {
		m_path.assign(std::move(path));
	}
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC acquire
namespace {
HORD_DEF_FMT_FQN(
	s_err_acquire_failed,
	"failed to acquire lock for `%s`"
);
} // anonymous namespace

void
LockFile::acquire() {
	if (!is_active()) {
		handle_type const fd = ::open(
			m_path.c_str(),
			O_CREAT | O_RDONLY,
			S_IRUSR | S_IRGRP | S_IROTH // 444
		);
		if (NULL_HANDLE != fd) {
			if (0 == ::flock(fd, LOCK_EX | LOCK_NB)) {
				m_handle = fd;
			} else {
				::close(fd);
			}
		}
		if (NULL_HANDLE == m_handle) {
			// TODO: add std::strerror()
			HORD_THROW_FMT(
				ErrorCode::lockfile_acquire_failed,
				s_err_acquire_failed,
				m_path
			);
		}
	}
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC release
void
LockFile::release() noexcept {
	if (is_active()) {
		// TODO: ::remove() on release?
		::close(m_handle);
		m_handle = NULL_HANDLE;
	}
}
#undef HORD_SCOPE_FUNC

#undef HORD_SCOPE_CLASS

} // namespace Hord

