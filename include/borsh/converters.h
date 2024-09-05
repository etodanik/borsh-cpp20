#pragma once
#ifndef BORSH_CPP20_CONVERTERS_H
#define BORSH_CPP20_CONVERTERS_H

namespace borsh
{

void to_bytes(IntegralType auto const& value, std::vector<uint8_t>& buffer)
{
    if constexpr (std::endian::native == std::endian::big)
    {
        append(buffer, byteswap(value));
    }

    append(buffer, value);
}

void to_bytes(FloatType auto const& value, std::vector<uint8_t>& buffer)
{
    if (std::isnan(value)) [[unlikely]]
    {
        throw std::invalid_argument("NaN is not allowed");
    }

    if constexpr (std::endian::native == std::endian::big)
    {
        append(buffer, byteswap(float_to_int(value)));
    }

    append(buffer, float_to_int(value));
}

void to_bytes(StringType auto const& value, std::vector<uint8_t>& buffer)
{
    append(buffer, static_cast<int32_t>(value.length()));

    for (char c : value)
    {
        buffer.push_back(static_cast<int8_t>(c));
    }
}

void to_bytes(ScalarArrayType auto const& array, std::vector<uint8_t>& buffer)
{
    for (auto item : array)
    {
        to_bytes(item, buffer);
    }
}

void to_bytes(ScalarStdArrayType auto const& array, std::vector<uint8_t>& buffer)
{
    for (auto item : array)
    {
        to_bytes(item, buffer);
    }
}

template <NumericType T> void from_bytes(T& value, const uint8_t*& buffer)
{
    static_assert(!std::is_const_v<T>, "T must not be const");

    value = (std::endian::native == std::endian::big) ? byteswap(*reinterpret_cast<const T*>(buffer)) : *reinterpret_cast<const T*>(buffer);
    buffer += sizeof(T);
}

template <FloatType T> void from_bytes(T& value, const uint8_t*& buffer)
{
    static_assert(!std::is_const_v<T>, "T must not be const");

    value = (std::endian::native == std::endian::big) ? int_to_float(byteswap(float_to_int(*reinterpret_cast<const T*>(buffer))))
                                                      : *reinterpret_cast<const T*>(buffer);
    buffer += sizeof(T);
}

template <StringType T> void from_bytes(T& value, const uint8_t*& buffer)
{
    static_assert(!std::is_const_v<T>, "T must not be const");

    const int32_t length = *reinterpret_cast<const int32_t*>(buffer);
    buffer += sizeof(int32_t);

    value.clear();
    for (int32_t i = 0; i < length; ++i)
    {
        value.push_back(static_cast<typename T::value_type>(*(buffer++)));
    }
}

template <ScalarType T, std::size_t N> void from_bytes(T (&value)[N], const uint8_t*& buffer)
{
    static_assert(!std::is_const_v<decltype(value)>, "T must not be const");

    for (auto& element : value)
    {
        from_bytes(element, buffer);
    }
}

template <typename T, std::size_t N>
void from_bytes(std::array<T, N>& value, const uint8_t*& buffer)
{
    static_assert(!std::is_const_v<T>, "T must not be const");

    for (auto& element : value)
    {
        from_bytes(element, buffer);
    }
}

} // namespace borsh

#endif