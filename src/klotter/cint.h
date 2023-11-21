#pragma once

constexpr float Cint_to_float(int i)
{
	return static_cast<float>(i);
}

constexpr std::size_t Cint_to_sizet(int i)
{
	return static_cast<std::size_t>(i);
}

constexpr u32 Csizet_to_u32(std::size_t i)
{
	return static_cast<u32>(i);
}

constexpr int Csizet_to_int(std::size_t i)
{
	return static_cast<int>(i);
}

constexpr int Cunsigned_int_to_int(unsigned int i)
{
	return static_cast<int>(i);
}
