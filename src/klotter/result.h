#pragma once

#include <optional>

template<typename T, typename E = std::string>
struct Result
{
	std::optional<T> value;
	std::optional<E> e;

	explicit Result(T tt)
		: value(std::move(tt))
	{
	}

	explicit Result(const E& ee)
		: e(ee)
	{
	}

	explicit operator bool() const
	{
		return value.has_value();
	}
};
