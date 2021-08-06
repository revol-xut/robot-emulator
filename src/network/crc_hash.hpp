//
// Author: Tassilo Tanneberger on 20/6/2021.
// Owner: GFaI e.V
// Project: Robo Simulator
//

#ifndef ROBO_SIMULATOR_CRC_HASH_HPP
#define ROBO_SIMULATOR_CRC_HASH_HPP

// credit for implementation: https://stackoverflow.com/questions/28675727/using-crc32-algorithm-to-hash-string-at-compile-time
// TODO: maybe try to avoid the macros
// with some slight modifications and additions

#include <cstring>
#include <cstdint>
#include <iostream>

// Generate CRC lookup table
template <unsigned c, int k = 8>
struct f : f<((c & 1) ? 0xedb88320 : 0) ^ (c >> 1), k - 1> {};
template <unsigned c> struct f<c, 0> { enum { value = c }; };

#define A(x) B(x) B(x + 128)
#define B(x) C(x) C(x +  64)
#define C(x) D(x) D(x +  32)
#define D(x) E(x) E(x +  16)
#define E(x) F(x) F(x +   8)
#define F(x) G(x) G(x +   4)
#define G(x) H(x) H(x +   2)
#define H(x) I(x) I(x +   1)
#define I(x) f<x>::value ,

constexpr unsigned crc_table[] = { A(0) };

// Constexpr implementation and helpers
constexpr auto crc32_impl(const uint8_t* p, size_t len, uint32_t crc) -> uint32_t {
    return len ?
        crc32_impl(p + 1, len - 1, (crc >> 8) ^ crc_table[(crc & 0xFF) ^ *p])
        : crc;
}

constexpr auto crc32(const uint8_t* data, size_t length) -> uint32_t {
    return ~crc32_impl(data, length, ~0);
}

constexpr auto strlen_c(const char* str) -> std::size_t {
    return *str ? 1 + strlen_c(str + 1) : 0;
}

constexpr auto crcFromCString(const char* str) -> int{
    return crc32((uint8_t*)str, strlen_c(str));
}

constexpr auto crcFromString(const std::string& str) -> int {
    return crc32((uint8_t*)str.data(), str.size());
}

#endif //ROBO_SIMULATOR_CRC_HASH_HPP