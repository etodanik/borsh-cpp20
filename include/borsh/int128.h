#pragma once
#ifndef BORSH_CPP20_INT128_H
#define BORSH_CPP20_INT128_H

#ifdef __SIZEOF_INT128__
#if (defined(__clang__) && !defined(_WIN32)) || (defined(__CUDACC__) && __CUDACC_VER_MAJOR__ >= 9) \
    || (defined(__GNUC__) && !defined(__clang__) && !defined(__CUDACC__))
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#define BORSH_HAVE_INTRINSIC_INT128 1
using uint128_t = unsigned __int128;
using int128_t = __int128;
#ifndef UINT128_MAX
#define UINT128_MAX ((uint128_t)(-1))
#define INT128_MAX ((int128_t)(UINT128_MAX >> 1))
#define INT128_MIN (-INT128_MAX - 1)
#endif
#pragma GCC diagnostic pop
#endif
#endif

#endif