#pragma once

#include <algorithm>
#include <cmath>
#include <string>

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
		const catchy::FalseString c = compare(l, r);
		if (c.IsTrue() == false)
		{
			std::ostringstream ss;
			ss << name << ": " << l << " != " << r;
			// hack: since empty string is true, let's use a single space as <no reason>
			if(c.reason != " ")
			{
				ss << " (" << c.reason << ")";
			}
			failures.emplace_back(ss.str());
		}
		return *this;
	}

	template<typename C>
	Self& add(const std::string& name, const C& convert){
		return impl(name, convert, [](const auto& l, const auto& r) {
			const auto same = l == r;
			if (same) return catchy::FalseString::True();
			return catchy::FalseString::False(" ");
		});
	}

	template<typename C, typename F>
	Self& add(const std::string& name, const C& convert, F min)
	{
		return impl(name, convert, [min](const auto& l, const auto& r) {
			const auto diff = std::abs(l - r);
			const auto close = diff < min;
			if (close) return catchy::FalseString::True();
			std::ostringstream ss;
			ss << "difference " << diff << " is larger than " << min;
			return catchy::FalseString::False(ss.str());
		});
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