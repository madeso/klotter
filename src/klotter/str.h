#pragma once

struct Str
{
	std::ostringstream ss;

	template<typename T>
	Str& operator<<(const T& t)
	{
		ss << t;
		return *this;
	}

	std::string str() const;

	operator std::string() const;
};
