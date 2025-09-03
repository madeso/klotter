#pragma once

#include <algorithm>

namespace klotter
{

// ------------------------------------------------------------------------------------------------
// std utils

// convert an enum class to it's underlying (int) type
// src: https://twitter.com/idoccor/status/1314664849276899328
template<typename E>
constexpr typename std::underlying_type_t<E> base_cast(E e) noexcept
{
	return static_cast<typename std::underlying_type_t<E>>(e);
}

template<typename E>
constexpr bool is_flag_set(E var, E flag)
{
	return (base_cast(var) & base_cast(flag)) > 0;
}

template<typename T>
bool all_equal(const T& t)
{
	return std::adjacent_find( std::begin(t), std::end(t), std::not_equal_to<>() ) == std::end(t);
}

}  //  namespace klotter
