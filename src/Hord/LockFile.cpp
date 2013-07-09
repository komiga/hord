
#include <Hord/ErrorCode.hpp>
#include <Hord/Error.hpp>
#include <Hord/LockFile.hpp>

#include <ceformat/print.hpp>

#include <cstdlib>
#include <cstdio>
#include <utility>

#include <unistd.h>
#include <sys/file.h>

namespace Hord {

// class LockFile implementation

#define HORD_SCOPE_CLASS_IDENT__ LockFile

LockFile::LockFile(LockFile&&) = default;
LockFile& LockFile::operator=(LockFile&&) = default;

LockFile::LockFile(
	String path
) noexcept
	: m_handle(NULL_HANDLE)
	, m_path(std::move(path))
{}

LockFile::~LockFile() {
	release();
}

#define HORD_SCOPE_FUNC_IDENT__ set_path

HORD_FMT_SCOPED_FQN(
	s_err_immutable,
	"cannot change path to `%s` while lock is active"
);

void
LockFile::set_path(
	String path
) {
	if (is_active()) {
		HORD_THROW_ERROR_F(
			ErrorCode::lockfile_immutable,
			s_err_immutable,
			m_path
		);
	} else {
		m_path.assign(std::move(path));
	}
}
#undef HORD_SCOPE_FUNC_IDENT__

#define HORD_SCOPE_FUNC_IDENT__ acquire

HORD_FMT_SCOPED_FQN(
	s_err_acquire_failed,
	"failed to acquire lock for `%s`"
);

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
			HORD_THROW_ERROR_F(
				ErrorCode::lockfile_acquire_failed,
				s_err_acquire_failed,
				m_path
			);
		}
	}
}
#undef HORD_SCOPE_FUNC_IDENT__

#define HORD_SCOPE_FUNC_IDENT__ release
void
LockFile::release() noexcept {
	if (is_active()) {
		// TODO: ::remove() on release?
		::close(m_handle);
		m_handle = NULL_HANDLE;
	}
}
#undef HORD_SCOPE_FUNC_IDENT__

#undef HORD_SCOPE_CLASS_IDENT__

} // namespace Hord

