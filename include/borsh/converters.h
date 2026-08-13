#pragma once
#ifndef BORSH_CPP20_CONVERTERS_H
#define BORSH_CPP20_CONVERTERS_H

#include <limits>

namespace borsh
{

Error to_bytes(IntegralType auto const& value, std::vector<uint8_t>& buffer)
{
    if constexpr (std::endian::native == std::endian::big)
    {
        append(buffer, byteswap(value));
    }
    else
    {
        append(buffer, value);
    }
    return Error::None;
}

inline Error to_bytes(bool value, std::vector<uint8_t>& buffer)
{
    buffer.push_back(value);
    return Error::None;
}

Error to_bytes(FloatType auto const& value, std::vector<uint8_t>& buffer)
{
    if (std::isnan(value)) [[unlikely]]
    {
        return Error::InvalidFloat;
    }

    if constexpr (std::endian::native == std::endian::big)
    {
        append(buffer, byteswap(float_to_int(value)));
    }
    else
    {
        append(buffer, float_to_int(value));
    }
    return Error::None;
}

inline Error to_bytes_size(std::size_t size, std::vector<uint8_t>& buffer)
{
    if (size > std::numeric_limits<uint32_t>::max())
    {
        return Error::LengthOverflow;
    }
    return to_bytes(static_cast<uint32_t>(size), buffer);
}

Error to_bytes(StringType auto const& value, std::vector<uint8_t>& buffer)
{
    if (auto error = to_bytes_size(value.length(), buffer); error != Error::None)
    {
        return error;
    }
    buffer.insert(buffer.end(), value.begin(), value.end());
    return Error::None;
}

Error to_bytes(ScalarArrayType auto const& array, std::vector<uint8_t>& buffer)
{
    for (const auto& item : array)
    {
        if (auto error = to_bytes(item, buffer); error != Error::None)
        {
            return error;
        }
    }
    return Error::None;
}

Error to_bytes(ScalarStdArrayType auto const& array, std::vector<uint8_t>& buffer)
{
    for (const auto& item : array)
    {
        if (auto error = to_bytes(item, buffer); error != Error::None)
        {
            return error;
        }
    }
    return Error::None;
}

template <typename T> Error read(T& value, const uint8_t*& buffer, std::size_t& remaining)
{
    if (sizeof(T) > remaining)
    {
        return Error::UnexpectedEnd;
    }
    std::memcpy(&value, buffer, sizeof(T));
    buffer += sizeof(T);
    remaining -= sizeof(T);
    return Error::None;
}

template <IntegralType T> Error from_bytes(T& value, const uint8_t*& buffer, std::size_t& remaining)
{
    static_assert(!std::is_const_v<T>, "T must not be const");

    T encoded;
    if (auto error = read(encoded, buffer, remaining); error != Error::None)
    {
        return error;
    }
    value = (std::endian::native == std::endian::big) ? byteswap(encoded) : encoded;
    return Error::None;
}

inline Error from_bytes(bool& value, const uint8_t*& buffer, std::size_t& remaining)
{
    uint8_t encoded;
    if (auto error = read(encoded, buffer, remaining); error != Error::None)
    {
        return error;
    }
    if (encoded > 1)
    {
        return Error::InvalidBool;
    }
    value = encoded;
    return Error::None;
}

template <FloatType T> Error from_bytes(T& value, const uint8_t*& buffer, std::size_t& remaining)
{
    static_assert(!std::is_const_v<T>, "T must not be const");

    decltype(float_to_int(value)) encoded;
    if (auto error = read(encoded, buffer, remaining); error != Error::None)
    {
        return error;
    }
    if constexpr (std::endian::native == std::endian::big)
    {
        encoded = byteswap(encoded);
    }
    value = int_to_float(encoded);
    if (std::isnan(value))
    {
        return Error::InvalidFloat;
    }
    return Error::None;
}

template <StringType T> Error from_bytes(T& value, const uint8_t*& buffer, std::size_t& remaining)
{
    static_assert(!std::is_const_v<T>, "T must not be const");

    uint32_t length;
    if (auto error = from_bytes(length, buffer, remaining); error != Error::None)
    {
        return error;
    }
    if (length > remaining)
    {
        return Error::UnexpectedEnd;
    }

    value.assign(reinterpret_cast<const char*>(buffer), length);
    buffer += length;
    remaining -= length;
    return Error::None;
}

template <ScalarType T, std::size_t N> Error from_bytes(T (&value)[N], const uint8_t*& buffer, std::size_t& remaining)
{
    static_assert(!std::is_const_v<decltype(value)>, "T must not be const");

    for (auto& element : value)
    {
        if (auto error = from_bytes(element, buffer, remaining); error != Error::None)
        {
            return error;
        }
    }
    return Error::None;
}

template <typename T, std::size_t N>
Error from_bytes(std::array<T, N>& value, const uint8_t*& buffer, std::size_t& remaining)
{
    static_assert(!std::is_const_v<T>, "T must not be const");

    for (auto& element : value)
    {
        if (auto error = from_bytes(element, buffer, remaining); error != Error::None)
        {
            return error;
        }
    }
    return Error::None;
}

} // namespace borsh

#endif
