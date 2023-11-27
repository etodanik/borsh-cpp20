//
// Copyright 2023 Daniel Povolotski (dannypovolotski at gmail dot com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
// documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
// Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

#include <array>
#include <cstddef>
#include <cstring>
#include <memory>
#include <vector>
#include <type_traits>
#include <string>
#include <stdexcept>
#include <bit>
#include <cstdint>
#include <algorithm>
#include <vector>
#include <memory>

#ifdef __SIZEOF_INT128__
using uint128_t = unsigned __int128;
using int128_t = __int128;

#ifndef UINT128_MAX
#define UINT128_MAX ((uint128_t)(-1))
#define INT128_MAX ((int128_t)(UINT128_MAX >> 1))
#define INT128_MIN (-INT128_MAX - 1)
#endif

#endif

namespace borsh
{
class Serializer;

enum SerializerDirection
{
    Serialize = 0,
    Deserialize = 1,
};

template <typename T>
concept IntegralType = std::is_integral_v<T>;

template <typename T>
#ifdef __SIZEOF_INT128__
concept FloatType = std::is_floating_point_v<T>;
#else
concept FloatType = std::is_same_v<T, float> || std::is_same_v<T, double>;
#endif

template <typename T>
concept NumericType = IntegralType<T> || FloatType<T>;

template <typename T>
concept StringType = std::is_same_v<T, std::string>;

template <typename T>
concept ScalarType = NumericType<T> || StringType<T>;

template <typename T>
concept is_bounded_array_v = std::rank_v<T> == 1 && std::extent_v<T> != 0;

template <typename T> using remove_extent_and_cv_t = std::remove_extent_t<std::remove_cv_t<T>>;

template <typename T, typename C>
concept is_same_remove_extent_v = std::is_same_v<remove_extent_and_cv_t<T>, C>;

template <typename T>
concept CharArrayType = is_bounded_array_v<T> && (is_same_remove_extent_v<T, char> || is_same_remove_extent_v<T, unsigned char>);

template <typename T>
concept IntegralArrayType = is_bounded_array_v<T> && IntegralType<remove_extent_and_cv_t<T>>;

template <typename T>
concept FloatArrayType = is_bounded_array_v<T> && FloatType<remove_extent_and_cv_t<T>>;

template <typename T>
concept NumericArrayType = IntegralArrayType<T> || FloatArrayType<T>;

template <typename T>
concept ScalarArrayType = is_bounded_array_v<T> && ScalarType<remove_extent_and_cv_t<T>>;

template <typename T>
concept NonScalarArrayType = is_bounded_array_v<T> && !ScalarType<remove_extent_and_cv_t<T>>;

template <typename T>
concept ArrayType = ScalarArrayType<T> || NonScalarArrayType<T>;

template <typename T>
concept SerializableElement = requires(T t, Serializer& s) { serialize(t, s); };

template <typename T>
concept SerializableArray =
    requires(T (&array)[], Serializer& s) { serialize(array, s); } && SerializableElement<remove_extent_and_cv_t<T>>;

template <typename T>
concept SerializableVector = requires(T t) {
    requires std::same_as<std::remove_cv_t<T>, std::vector<typename T::value_type>>;
    requires SerializableElement<std::remove_cv_t<typename T::value_type>> || SerializableArray<std::remove_cv_t<typename T::value_type>>;
};

template <typename T>
concept SerializableVectorVector = requires(T t) {
    requires std::same_as<std::remove_cv_t<T>, std::vector<typename T::value_type>>;
    requires SerializableVector<std::remove_cv_t<typename T::value_type>>;
};

template <typename T>
concept Serializable = SerializableElement<T> || SerializableArray<T> || SerializableVector<T> || SerializableVectorVector<T>;

template <typename T>
concept SerializableNonScalar = SerializableElement<T> && !ScalarType<T>;

template <typename T>
concept SerializableNonScalarArray = SerializableArray<T> && !ScalarType<remove_extent_and_cv_t<T>>;

template <typename T>
concept SerializableScalar = SerializableElement<T> && ScalarType<T>;

template <typename T>
concept Swappable = IntegralType<T> && std::has_unique_object_representations_v<T>;

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
        if constexpr (sizeof(long double) == sizeof(int128_t))
        {
            return std::bit_cast<int128_t>(value);
        }
        else if constexpr (sizeof(long double) == sizeof(int64_t))
        {
            return std::bit_cast<int64_t>(value);
        }
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
    else if constexpr (std::is_same_v<T, long double>
        && ((sizeof(long double) == sizeof(int64_t)) || (sizeof(long double) == sizeof(int128_t))))
    {
        return std::bit_cast<long double>(value);
    }
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
    if (isnan(value)) [[unlikely]]
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

template <SerializableVector T> void from_bytes(T& value, const uint8_t*& buffer)
{
    static_assert(!std::is_const_v<T>, "T must not be const");

    const int32_t length = *reinterpret_cast<const int32_t*>(buffer);
    buffer += sizeof(int32_t);

    value.clear();
    for (int32_t i = 0; i < length; ++i)
    {
        typename T::value_type element;
        if constexpr (SerializableNonScalar<typename T::value_type>)
        {
            element = deserialize<typename T::value_type>(buffer);
            buffer += sizeof(typename T::value_type);
        }
        else
        {
            from_bytes(element, buffer);
        }
        value.push_back(element);
    }
}

class Serializer
{
public:
    explicit Serializer(std::vector<uint8_t>& inBuffer, const uint8_t*& inBufferPointerReference, SerializerDirection inDirection)
        : direction(inDirection), buffer(inBuffer), bufferPointerReference(inBufferPointerReference)
    {
    }

    template <typename... Args> Serializer& operator()(Args&... args)
    {
        (visit(args), ...);
        return *this;
    }

private:
    const SerializerDirection direction;
    std::vector<uint8_t>&     buffer;
    const uint8_t*&           bufferPointerReference;

    /**
     * This handles the execution path for the compiler where a const variable was passed to serialize().
     * Normally, the compiler would not have access to `direction` at compile-time, therefore trying to
     * explore the deserialization code path and returning a variety of errors. This helps prevent that
     * and offloads the undesirable situation to an exception.
     * @tparam T
     * @param value
     */
    template <typename T> void visit(const T& value)
    {
        if (direction == SerializerDirection::Serialize)
        {
            if constexpr (SerializableVector<T>)
            {
                append(buffer, static_cast<int32_t>(value.size()));

                for (auto item : value)
                {
                    if constexpr (SerializableNonScalar<typename T::value_type>)
                    {
                        auto serializedItem = serialize(item, *this);
                        buffer.insert(buffer.end(), serializedItem.begin(), serializedItem.end());
                    }
                    else
                    {
                        to_bytes(item, buffer);
                    }
                }
            }
            else if constexpr (ScalarType<T> || ScalarArrayType<T>)
            {
                to_bytes(value, buffer);
            }
            else
            {
                serialize(value, *this);
            }
        }
        else [[unlikely]]
        {
            throw std::runtime_error("Cannot deserialize into a const object");
        }
    }

    template <typename T> void visit(T& value)
    {
        if (direction == SerializerDirection::Serialize)
        {
            if constexpr (SerializableVector<T>)
            {
                append(buffer, static_cast<int32_t>(value.size()));

                for (auto item : value)
                {
                    if constexpr (SerializableNonScalar<typename T::value_type>)
                    {
                        serialize(item, *this);
                    }
                    else
                    {
                        to_bytes(item, buffer);
                    }
                }
            }
            else if constexpr (ScalarType<T> || ScalarArrayType<T>)
            {
                to_bytes(value, buffer);
            }
            else
            {
                serialize(value, *this);
            }
        }
        else
        {
            if constexpr (ScalarType<T> || ScalarArrayType<T> || SerializableVector<T>)
            {
                from_bytes(value, bufferPointerReference);
            }
            else
            {
                serialize(value, *this);
            }
        }
    }
};

auto serialize(ArrayType auto (&array)[], Serializer& serializer)
{
    return serializer(array);
}

auto serialize(ScalarType auto& value, Serializer& serializer)
{
    return serializer(value);
}

auto serialize(SerializableVector auto& value, Serializer& serializer)
{
    return serializer(value);
}

template <typename T>
    requires ScalarType<T> || ScalarArrayType<T>
std::vector<uint8_t> serialize(const T& value)
{
    std::vector<uint8_t> buffer;
    to_bytes(value, buffer);
    return buffer;
}

template <SerializableNonScalar T> std::vector<uint8_t> serialize(T& object)
{
    std::vector<uint8_t> buffer;
    const uint8_t*       data = buffer.data();
    Serializer           serializer(buffer, data, SerializerDirection::Serialize);
    serialize(object, serializer);
    return buffer;
}

std::vector<uint8_t> serialize(SerializableNonScalarArray auto (&array)[])
{
    std::vector<uint8_t> buffer;
    const uint8_t*       data = buffer.data();
    Serializer           serializer(buffer, data, SerializerDirection::Serialize);
    serialize(array, serializer);
    return buffer;
}

template <typename T>
    requires ScalarType<T>
T deserialize(std::vector<uint8_t>& buffer)
{
    const uint8_t* data = buffer.data();
    T              value;
    from_bytes(value, data);
    return value;
}

template <typename T, std::size_t N>
    requires ScalarType<T>
void deserialize(T (&value)[N], std::vector<uint8_t>& buffer)
{
    const uint8_t* data = buffer.data();
    from_bytes(value, data);
}

template <SerializableNonScalar T> T deserialize(std::vector<uint8_t>& buffer)
{
    const uint8_t* data = buffer.data();
    auto           object = T{};
    Serializer     serializer(buffer, data, SerializerDirection::Deserialize);
    serialize(object, serializer);
    return object;
}

void deserialize(SerializableNonScalarArray auto (&value)[], std::vector<uint8_t>& buffer)
{
    const uint8_t* data = buffer.data();
    Serializer     serializer(buffer, data, SerializerDirection::Deserialize);
    serialize(value, serializer);
}

} // namespace borsh
