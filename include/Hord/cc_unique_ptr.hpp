/**
@file cc_unique_ptr.hpp
@brief const-correct unique_ptr.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_CC_UNIQUE_PTR_HPP_
#define HORD_CC_UNIQUE_PTR_HPP_

#include <Hord/config.hpp>
#include <type_traits>
#include <utility>
#include <memory>

namespace Hord {

// Forward declarations
template<
	typename _Tp,
	typename _Dp = std::default_delete<_Tp>
>
class cc_unique_ptr;

/**
	@addtogroup etc
	@{
*/
/**
	@addtogroup memory
	@{
*/

/**
	const-correct std::unique_ptr.
*/
template<
	typename _Tp,
	typename _Dp
>
class cc_unique_ptr final {
private:
	using base = std::unique_ptr<_Tp, _Dp>;

public:
	using pointer = typename base::pointer;
	using element_type = typename base::element_type;
	using deleter_type = typename base::deleter_type;

	using const_pointer = element_type const*;
	using reference
		= typename std::add_lvalue_reference<element_type>::type;
	using const_reference
		= typename std::add_lvalue_reference<element_type const>::type;

private:
	using d1ctor_deleter_type
	= typename std::conditional<
		std::is_reference<deleter_type>::value,
		deleter_type,
		const deleter_type&
	>::type;

	base m_base;

public:
// constructors
	constexpr
	cc_unique_ptr() noexcept = default;

	constexpr
	cc_unique_ptr(
		std::nullptr_t
	) noexcept
		: m_base(nullptr)
	{}

	explicit
	cc_unique_ptr(
		pointer ptr
	) noexcept
		: m_base(std::move(ptr))
	{}

	cc_unique_ptr(
		pointer ptr,
		d1ctor_deleter_type deleter
	) noexcept
		: m_base(std::move(ptr), std::forward<d1ctor_deleter_type>(deleter))
	{}

	cc_unique_ptr(
		pointer ptr,
		typename std::remove_reference<deleter_type>::type&& deleter
	) noexcept
		: m_base(std::move(ptr), std::move(deleter))
	{}

	cc_unique_ptr(cc_unique_ptr const&) = delete;

	cc_unique_ptr(
		cc_unique_ptr&& upc
	) noexcept
		: m_base(std::move(upc.m_base))
	{}

/*
typename = typename std::enable_if<
	std::is_convertible<
		typename std::unique_ptr<_Up, _Ep>::pointer,
		pointer
	>::value
	&& !std::is_array<_Up>::value
	&& std::is_convertible<_Ep, deleter_type>::value
	&& (
		!std::is_reference<deleter_type>::value ||
		std::is_same<deleter_type, _Ep>::value
	)
>::type
*/
	template<
		typename _Up,
		typename _Ep
	>
	cc_unique_ptr(
		cc_unique_ptr<_Up, _Ep>&& up
	) noexcept
		: m_base(std::forward<std::unique_ptr<_Up, _Ep>>(up))
	{}

// destructor
	// NB: The Standard specifies no noexcept, but says:
	//   The expression get_deleter()(get()) shall be well formed,
	//   shall have well-defined behavior, and shall not throw
	//   exceptions.
	// Which suggests the unique_ptr implementation is allowed to
	// throw. libc++ concurs with the specification, but libstdc++
	// does not.
	~cc_unique_ptr() noexcept(noexcept(m_base.~base())) = default;

// assignment
	cc_unique_ptr& operator=(cc_unique_ptr const&) = delete;
	cc_unique_ptr& operator=(cc_unique_ptr&&) noexcept = default;

	template<
		typename _Up,
		typename _Ep
	>
	cc_unique_ptr&
	operator=(
		cc_unique_ptr<_Up, _Ep>&& up
	) noexcept {
		m_base.operator=(std::forward<std::unique_ptr<_Up, _Ep>>(up));
		return *this;
	}

	cc_unique_ptr&
	operator=(
		std::nullptr_t
	) noexcept {
		m_base.operator=(nullptr);
		return *this;
	}

// observers
	pointer
	get() noexcept {
		return m_base.get();
	}

	const_pointer
	get() const noexcept {
		return m_base.get();
	}

	deleter_type&
	get_deleter() noexcept {
		return m_base.get_deleter();
	}

	deleter_type const&
	get_deleter() const noexcept {
		return m_base.get_deleter();
	}

	reference
	operator*() {
		return m_base.operator*();
	}

	const_reference
	operator*() const {
		return m_base.operator*();
	}

	pointer
	operator->() noexcept {
		return m_base.operator->();
	}

	const_pointer
	operator->() const noexcept {
		return m_base.operator->();
	}

	explicit
	operator bool() const noexcept {
		return m_base.operator bool();
	}

// modifiers
	pointer
	release() noexcept {
		return m_base.release();
	}

	void
	reset(
		pointer ptr = pointer()
	) noexcept {
		m_base.reset(std::forward<pointer>(ptr));
	}

	void
	swap(
		cc_unique_ptr& other
	) noexcept {
		m_base.swap(other.m_base);
	}
};

// specialized algorithms

// NB: The Standard does not give T1-T2 comparison operators noexcept,
// but get() is specified as having noexcept, so all of these
// operators do _not_ follow the standard (because they have
// noexcept).

// operator==

template<
	typename _Tp1, typename _Dp1,
	typename _Tp2, typename _Dp2
>
inline bool
operator==(
	cc_unique_ptr<_Tp1, _Dp1> const& _x,
	cc_unique_ptr<_Tp2, _Dp2> const& _y
) noexcept {
	return _x.get() == _y.get();
}

template<
	typename _Tp, typename _Dp
>
inline bool
operator==(
	cc_unique_ptr<_Tp, _Dp> const& _x,
	std::nullptr_t
) noexcept {
	return !_x;
}

template<
	typename _Tp, typename _Dp
>
inline bool
operator==(
	std::nullptr_t,
	cc_unique_ptr<_Tp, _Dp> const& _x
) noexcept {
	return !_x;
}

// operator!=

template<
	typename _Tp1, typename _Dp1,
	typename _Tp2, typename _Dp2
>
inline bool
operator!=(
	cc_unique_ptr<_Tp1, _Dp1> const& _x,
	cc_unique_ptr<_Tp2, _Dp2> const& _y
) noexcept {
	return _x.get() != _y.get();
}

template<
	typename _Tp, typename _Dp
>
inline bool
operator!=(
	cc_unique_ptr<_Tp, _Dp> const& _x,
	std::nullptr_t
) noexcept {
	return (bool)_x;
}

template<
	typename _Tp, typename _Dp
>
inline bool
operator!=(
	std::nullptr_t,
	cc_unique_ptr<_Tp, _Dp> const& _x
) noexcept {
	return (bool)_x;
}

// operator<

template<
	typename _Tp1, typename _Dp1,
	typename _Tp2, typename _Dp2
>
inline bool
operator<(
	cc_unique_ptr<_Tp1, _Dp1> const& _x,
	cc_unique_ptr<_Tp2, _Dp2> const& _y
) noexcept {
	using _CT = typename std::common_type<
		typename cc_unique_ptr<_Tp1, _Dp1>::pointer,
		typename cc_unique_ptr<_Tp2, _Dp2>::pointer
	>::type;
	return std::less<_CT>(_x.get(), _y.get());
}

template<
	typename _Tp, typename _Dp
>
inline bool
operator<(
	cc_unique_ptr<_Tp, _Dp> const& _x,
	std::nullptr_t
) noexcept {
	return std::less<typename cc_unique_ptr<_Tp, _Dp>::pointer>(
		_x.get(), nullptr
	);
}

template<
	typename _Tp, typename _Dp
>
inline bool
operator<(
	std::nullptr_t,
	cc_unique_ptr<_Tp, _Dp> const& _x
) noexcept {
	return std::less<typename cc_unique_ptr<_Tp, _Dp>::pointer>(
		nullptr, _x.get()
	);
}

// operator<=

template<
	typename _Tp1, typename _Dp1,
	typename _Tp2, typename _Dp2
>
inline bool
operator<=(
	cc_unique_ptr<_Tp1, _Dp1> const& _x,
	cc_unique_ptr<_Tp2, _Dp2> const& _y
) noexcept {
	return !(_y < _x);
}

template<
	typename _Tp, typename _Dp
>
inline bool
operator<=(
	cc_unique_ptr<_Tp, _Dp> const& _x,
	std::nullptr_t
) noexcept {
	return !(nullptr < _x);
}

template<
	typename _Tp, typename _Dp
>
inline bool
operator<=(
	std::nullptr_t,
	cc_unique_ptr<_Tp, _Dp> const& _x
) noexcept {
	return !(_x < nullptr);
}

// operator>

template<
	typename _Tp1, typename _Dp1,
	typename _Tp2, typename _Dp2
>
inline bool
operator>(
	cc_unique_ptr<_Tp1, _Dp1> const& _x,
	cc_unique_ptr<_Tp2, _Dp2> const& _y
) noexcept {
	return _y < _x;
}

template<
	typename _Tp, typename _Dp
>
inline bool
operator>(
	cc_unique_ptr<_Tp, _Dp> const& _x,
	std::nullptr_t
) noexcept {
	return nullptr < _x;
}

template<
	typename _Tp, typename _Dp
>
inline bool
operator>(
	std::nullptr_t,
	cc_unique_ptr<_Tp, _Dp> const& _x
) noexcept {
	return _x < nullptr;
}

// operator>=

template<
	typename _Tp1, typename _Dp1,
	typename _Tp2, typename _Dp2
>
inline bool
operator>=(
	cc_unique_ptr<_Tp1, _Dp1> const& _x,
	cc_unique_ptr<_Tp2, _Dp2> const& _y
) noexcept {
	return !(_x < _y);
}

template<
	typename _Tp, typename _Dp
>
inline bool
operator>=(
	cc_unique_ptr<_Tp, _Dp> const& _x,
	std::nullptr_t
) noexcept {
	return !(_x < nullptr);
}

template<
	typename _Tp, typename _Dp
>
inline bool
operator>=(
	std::nullptr_t,
	cc_unique_ptr<_Tp, _Dp> const& _x
) noexcept {
	return !(nullptr < _x);
}

/** @} */ // end of doc-group memory
/** @} */ // end of doc-group etc

} // namespace Hord

#endif // HORD_CC_UNIQUE_PTR_HPP_
