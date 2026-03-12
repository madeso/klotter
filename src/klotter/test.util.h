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

	Self& add(const std::string& name, std::string (*convert)(const T&))
	{
		return impl(name, convert, [](const std::string& l, const std::string& r)
		{
			const auto same = l == r;
			if (same) return catchy::FalseString::True();
			std::ostringstream ss;
			const auto edit = levenshtein(l, r);
			const auto diff_index = find_index_of_first_diff(l, r);
			ss << "length diff " << l.length() << " vs " << r.length() << ", changes=" << edit << ", first diff at "
			   << diff_index << " (" << l[diff_index] << " vs " << r[diff_index] << ")";
			return catchy::FalseString::False(ss.str());
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

	// mit from https://github.com/guilhermeagostinelli/levenshtein/blob/master/levenshtein.cpp
	static int levenshtein(const std::string& foo, const std::string& bar) {
		const auto foo_size = foo.size();
		const auto bar_size = bar.size();

		std::size_t state[foo_size + 1][bar_size + 1];
		
		if (foo_size == 0)
			return bar_size;
		if (bar_size == 0)
			return foo_size;

		for (std::size_t foo_index = 0; foo_index <= foo_size; foo_index++)
		{
			state[foo_index][0] = foo_index;
		}

		for (std::size_t bar_index = 0; bar_index <= bar_size; bar_index++)
		{
			state[0][bar_index] = bar_index;
		}

		for (std::size_t foo_index = 1; foo_index <= foo_size; foo_index++)
		{
			for (int bar_index = 1; bar_index <= bar_size; bar_index++)
			{
				const auto change_cost = (bar[bar_index - 1] == foo[foo_index - 1]) ? 0 : 1;

				const auto cost_foo = state[foo_index - 1][bar_index] + 1;
				const auto cost_bar = state[foo_index][bar_index - 1] + 1;
				const auto cost_both = state[foo_index - 1][bar_index - 1] + change_cost;

				const auto min_cost = std::min(std::min(cost_foo, cost_bar), cost_both);
				state[foo_index][bar_index] = min_cost;
			}
		}
		
		return state[foo_size][bar_size];
	}

	static std::size_t find_index_of_first_diff(const std::string& first, const std::string& second) {
		std::size_t index = 0;
		for (; index <= std::min(first.length(), second.length()); index++)
		{
			if(first[index] != second[index]) return index;
		}
		return index;
	}
};

}