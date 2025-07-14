#include "catch2/catch_test_macros.hpp"
#include "catchy/falsestring.h"

#include "klotter/scurve.h"

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

using namespace klotter;


catchy::FalseString is_equal(const SCurve& lhs, SCurve rhs)
{
	constexpr float machine_epsilon = 0.001f;
	return Equaler{lhs, rhs}
		.add(
			"s", [](const SCurve& s) { return s.slope; }, machine_epsilon
		)
		.add(
			"t", [](const SCurve& s) { return s.threshold; }, machine_epsilon
		)
		.resolve();
}

TEST_CASE("scurve_from_input", "[scurve]")
{
	const auto middle = s_curve_from_input(0.5f, 0.5f);
    CHECK(is_equal(middle, {1.0f, 0.75f}));

    const auto top_right = s_curve_from_input(1.0f, 1.0f);
	CHECK(is_equal(top_right, {243.0f, 0.5f}));

    const auto bottom_right = s_curve_from_input(1.0f, 0.0f);
	CHECK(is_equal(bottom_right, {243.0f, 1.0f}));
}
