#pragma once

#include <optional>

// todo(Gustav): remove and upgrade to cpp 23 that has improved api https://devblogs.microsoft.com/cppblog/cpp23s-optional-and-expected/
/// A poor man's std::expected.
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
