#pragma once

/** A poor man's fmt::format

 Example usage:

 ```cpp
 std::string s = Str{} << "Hello, world! " << 42;
 ```

*/
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
