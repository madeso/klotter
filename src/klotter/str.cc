#include "klotter/str.h"

std::string Str::str() const
{
    return ss.str();
}

Str::operator std::string() const
{
    return str();
}