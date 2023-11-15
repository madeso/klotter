#pragma once

#include <optional>

template<typename T, typename E = std::string>
struct Result
{
	std::optional<T> value;
	std::optional<E> e;

	Result(T&& tt)
		: value(std::move(tt))
	{
	}

	Result(const E& ee)
		: e(ee)
	{
	}

	operator bool() const
	{
		return value.has_value();
	}
};
