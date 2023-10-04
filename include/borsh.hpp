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

#include <tuple>
#include <vector>
#include <type_traits>
#include <string>
#include <stdexcept>
#include <bit>
#include <version>
#include <cstdint>
#include <algorithm>

namespace borsh
{
class Serializer;

class Deserializer;

enum SerializerDirection
{
    Serialize = 0,
    Deserialize = 1,
};

template <typename T>
inline constexpr bool is_bounded_char_array_v = std::rank_v<T> == 1 && std::extent_v<T> != 0
    && (std::is_same_v<std::remove_extent_t<std::remove_cv_t<T>>, char>
        || std::is_same_v<std::remove_extent_t<std::remove_cv_t<T>>, unsigned char>);

template <typename T>
concept IntegralType = std::is_integral_v<T>;

template <typename T>
concept StringType = std::is_same_v<T, std::string>;

template <typename T>
concept CharArrayType = is_bounded_char_array_v<T>;

template <typename T>
concept ScalarType = IntegralType<T> || StringType<T>;

template <typename T>
concept ArrayType = CharArrayType<T>;

template <typename T>
concept Serializable = requires(T t, Serializer& s) { serialize(t, s); };

template <typename T>
concept SerializableNonScalar = Serializable<T> && !ScalarType<T>;

template <typename T>
concept SerializableScalar = Serializable<T> && ScalarType<T>;

template <typename T>
concept Swappable = IntegralType<T> && std::has_unique_object_representations_v<T>;

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

void to_bytes(StringType auto const& value, std::vector<uint8_t>& buffer)
{
    append(buffer, static_cast<int32_t>(value.length()));

    for (char c : value)
    {
        buffer.push_back(static_cast<int8_t>(c));
    }
}

template <IntegralType T> void from_bytes(T& value, const uint8_t*& buffer)
{
    static_assert(!std::is_const_v<T>, "T must not be const");

    value = (std::endian::native == std::endian::big) ? byteswap(*reinterpret_cast<const T*>(buffer)) : *reinterpret_cast<const T*>(buffer);
    buffer += sizeof(T);
}

template <StringType T> void from_bytes(T& value, const uint8_t*& buffer)
{
    static_assert(!std::is_const_v<T>, "T must not be const");

    int32_t length = *reinterpret_cast<const int32_t*>(buffer);
    buffer += sizeof(int32_t);

    value.clear();
    for (int32_t i = 0; i < length; ++i)
    {
        value.push_back(static_cast<typename T::value_type>(*(buffer++)));
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
    SerializerDirection   direction;
    std::vector<uint8_t>& buffer;
    const uint8_t*&       bufferPointerReference;

    template <typename T> void visit(T& value)
    {
        if (direction == SerializerDirection::Serialize)
        {
            if constexpr (ScalarType<T>)
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
            if constexpr (ScalarType<T>)
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

template <typename T>
    requires IntegralType<T> || StringType<T>
auto serialize(T& value, Serializer& serializer)
{
    return serializer(value);
}

template <ScalarType T> std::vector<uint8_t> serialize(const T& value)
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

template <ScalarType T> T deserialize(std::vector<uint8_t>& buffer)
{
    const uint8_t* data = buffer.data();
    T              value;
    from_bytes(value, data);
    return value;
}

template <SerializableNonScalar T> T deserialize(std::vector<uint8_t>& buffer)
{
    const uint8_t* data = buffer.data();
    auto           object = T{};
    Serializer     serializer(buffer, data, SerializerDirection::Deserialize);
    serialize(object, serializer);
    return object;
}
} // namespace borsh
