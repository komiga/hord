/**
@file
@brief Auxiliary stdlib specializations.
@ingroup aux

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#pragma once

#include <Hord/config.hpp>

#include <string>
#include <vector>
#include <deque>
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
	class CharT,
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
template<class T>
using vector
= std::vector<
	T,
	HORD_AUX_ALLOCATOR<T>
>;

/**
	@c std::deque<T>.
*/
template<class T>
using deque
= std::deque<
	T,
	HORD_AUX_ALLOCATOR<T>
>;

/**
	@c std::unordered_map<Key, T, Hash, KeyEqual>.
*/
template<
	class Key,
	class T,
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
	class Key,
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
