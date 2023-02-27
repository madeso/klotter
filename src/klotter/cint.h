#pragma once

constexpr int Csizet_to_int(std::size_t i)
{
    return static_cast<int>(i);
}

constexpr int Cunsigned_int_to_int(unsigned int i)
{
    return static_cast<int>(i);
}
