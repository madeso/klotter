#pragma once

namespace klotter
{

struct HashCombiner
{
	std::size_t result = 17;

	template<typename T>
	HashCombiner& combine(const T& t)
	{
		// src: https://stackoverflow.com/a/17017281/180307
		// numbers from: https://stackoverflow.com/a/1646913/180307
		// A word of warning, this is (a variation of) the Berstein hash, and because nobody
		// knows why it does well in tests it is not advisable when hashing is critical.
		// See eternallyconfuzzled.com/tuts/algorithms/jsw_tut_hashing.aspx
		result = result * 31 + std::hash<T>{}(t);
		return *this;
	}
};

}  //  namespace klotter

#define HASH_DEF_BEGIN(TYPE) \
	template<> \
	struct std::hash<TYPE> \
	{ \
		std::size_t operator()(const TYPE& x) const \
		{ \
			return klotter::HashCombiner \
			{ \
			}
#define HASH_DEF(NAME) .combine(x.NAME)
#define HASH_DEF_END() \
	.result; \
	} \
	} \
	;
