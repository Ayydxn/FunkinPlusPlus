#pragma once

// boost::hash_combine's mixing constant/shift pattern - Standard, well-distributed way to fold a new hash into an existing one without just XOR'ing
// (which is commutative and order-insensitive, exactly what you don't want for combining several fields into one hash).
template<typename T>
void HashCombine(size_t& Seed, const T& Value)
{
    Seed ^= std::hash<T>()(Value) + 0x9e3779b9 + (Seed << 6) + (Seed >> 2);
}
