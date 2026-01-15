#pragma once

#include "catchy/falsestring.h"

namespace testing
{

constexpr float machine_epsilon = 0.001f;

template<typename T>
struct Equaler
{
	using Self = Equaler<T>;

	T lhs;
	T rhs;

	std::vector<std::string> failures;

	Equaler(T l, T r)
		: lhs(std::move(l))
		, rhs(std::move(r))
	{}

	template<typename F, typename C>
	Self& impl(
		const std::string& name,
		const F& convert,
		const C& compare
	)
	{
		const auto l = convert(lhs);
		const auto r = convert(rhs);
		if (compare(l, r) == false)
		{
			std::ostringstream ss;
			ss << name << ": " << l << " != " << r;
			failures.emplace_back(ss.str());
		}
		return *this;
	}

	template<typename C>
	Self& add(const std::string& name, const C& convert){
		return impl(name, convert, [](const auto& l, const auto& r) { return l == r; });
	}

	template<typename C, typename F>
	Self& add(const std::string& name, const C& convert, F min)
	{
		return impl(name, convert, [min](const auto& l, const auto& r) { return std::abs(l-r) < min; });
	}

	[[nodiscard]] catchy::FalseString resolve() const
	{
		if (failures.empty())
		{
			return catchy::FalseString::True();
		}

		std::ostringstream ss;

		for (const auto& fa: failures)
		{
			ss << "  " << fa << '\n';
		}

		return catchy::FalseString::False(ss.str());
	}
};

}