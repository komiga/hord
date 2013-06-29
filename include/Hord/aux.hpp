/**
@file aux.hpp
@brief Auxiliary stdlib specializations.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_AUX_HPP_
#define HORD_AUX_HPP_

#include "./config.hpp"

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace Hord {
namespace aux {

/**
	@addtogroup aux
	@{
*/

/**
	@c std::basic_string<CharT, Traits>.
*/
template<
	typename CharT,
	class Traits = std::char_traits<CharT>
>
using basic_string
= std::basic_string<
	CharT, Traits,
	HORD_AUX_ALLOCATOR<CharT>
>;

/**
	@c std::vector<T>.
*/
template<
	typename T
>
using vector
= std::vector<
	T,
	HORD_AUX_ALLOCATOR<T>
>;

/**
	@c std::unordered_map<Key, T, Hash, KeyEqual>.
*/
template<
	typename Key,
	typename T,
	class Hash = std::hash<Key>,
	class KeyEqual = std::equal_to<Key>
>
using unordered_map
= std::unordered_map<
	Key, T, Hash, KeyEqual,
	HORD_AUX_ALLOCATOR<std::pair<Key const, T> >
>;

/**
	@c std::unordered_set<Key, Hash, KeyEqual>.
*/
template<
	typename Key,
	class Hash = std::hash<Key>,
	class KeyEqual = std::equal_to<Key>
>
using unordered_set
= std::unordered_set<
	Key, Hash, KeyEqual,
	HORD_AUX_ALLOCATOR<Key>
>;

/** @} */ // end of doc-group aux

} // namespace aux
} // namespace Hord

#endif // HORD_AUX_HPP_
