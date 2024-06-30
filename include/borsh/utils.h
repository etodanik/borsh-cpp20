#pragma once
#ifndef BORSH_CPP20_UTILS_H
#define BORSH_CPP20_UTILS_H

namespace borsh
{

template <FloatType T> auto float_to_int(T value) -> auto
{
    if constexpr (std::is_same_v<T, float> && sizeof(float) == sizeof(int32_t))
    {
        return std::bit_cast<int32_t>(value);
    }
    else if constexpr (std::is_same_v<T, double> && sizeof(double) == sizeof(int64_t))
    {
        return std::bit_cast<int64_t>(value);
    }
    else if constexpr (std::is_same_v<T, long double>)
    {
        if constexpr (sizeof(long double) == sizeof(int64_t))
        {
            return std::bit_cast<int64_t>(value);
        }
#ifdef BORSH_HAVE_INTRINSIC_INT128
        else if constexpr (sizeof(long double) == sizeof(int128_t))
        {
            return std::bit_cast<int128_t>(value);
        }
#endif
        else
        {
            static_assert(!std::is_same_v<T, T>, "Float on this target platform is of an unsupported length");
        }
    }
    else
    {
        static_assert(!std::is_same_v<T, T>, "Unsupported type for float_to_int or type is a non standard length on target platform");
    }
}

template <IntegralType T> auto int_to_float(T value) -> auto
{
    if constexpr (std::is_same_v<T, int32_t> && sizeof(float) == sizeof(int32_t))
    {
        return std::bit_cast<float>(value);
    }
    else if constexpr (std::is_same_v<T, int64_t> && sizeof(double) == sizeof(int64_t))
    {
        return std::bit_cast<double>(value);
    }
#ifdef BORSH_HAVE_INTRINSIC_INT128
    else if constexpr (std::is_same_v<T, int128_t> && (sizeof(long double) == sizeof(int64_t) || sizeof(long double) == sizeof(int128_t)))
    {
        return std::bit_cast<long double>(value);
    }
#endif
    else
    {
        static_assert(!std::is_same_v<T, T>, "Unsupported type for int_to_float or type is a non standard length on target platform");
    }
}

// a placeholder for https://en.cppreference.com/w/cpp/numeric/byteswap coming in C++23
constexpr auto byteswap(Swappable auto value) noexcept -> decltype(value)
{
    auto value_representation = std::bit_cast<std::array<std::byte, sizeof(decltype(value))>>(value);
    std::ranges::reverse(value_representation);
    return bit_cast<decltype(value)>(value_representation);
}

template <typename T> static void append(std::vector<uint8_t>& buffer, const T& value)
{
    const auto*    begin = reinterpret_cast<const uint8_t*>(&value);
    const uint8_t* end = begin + sizeof(value);
    buffer.insert(buffer.end(), begin, end);
}

} // namespace borsh

#endif