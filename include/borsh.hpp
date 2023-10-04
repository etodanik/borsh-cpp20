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
concept SerializableNonScalar = requires(T t, Serializer& s) { serialize(t, s); } && !ScalarType<T>;

#ifndef __cpp_lib_byteswap
template <typename T>
concept Swappable = IntegralType<T> && std::has_unique_object_representations_v<T>;

// a polyfill of sorts for https://en.cppreference.com/w/cpp/numeric/byteswap coming in C++23
constexpr auto byteswap(Swappable auto value) noexcept -> decltype(value)
{
    auto value_representation = std::bit_cast<std::array<std::byte, sizeof(decltype(value))>>(value);
    std::ranges::reverse(value_representation);
    return bit_cast<decltype(value)>(value_representation);
}
#endif

template <typename T> static void append(std::vector<uint8_t>& buffer, const T& value)
{
    const auto*    begin = reinterpret_cast<const uint8_t*>(&value);
    const uint8_t* end = begin + sizeof(value);
    buffer.insert(buffer.end(), begin, end);
}

void to_bytes(IntegralType auto const& value, std::vector<uint8_t>& buffer)
{
    // convert to little endian before serializing
    if constexpr (std::endian::native == std::endian::big)
    {
        append(buffer, byteswap(value));
    }
    append(buffer, value);
}

void to_bytes(StringType auto const& value, std::vector<uint8_t>& buffer)
{
    append(buffer, value);
}

template <IntegralType T> void from_bytes(T& value, const uint8_t*& buffer)
{
    static_assert(!std::is_const_v<T>, "T must not be const");

    // convert from little endian before deserializing
    value = (std::endian::native == std::endian::big) ? byteswap(*reinterpret_cast<const T*>(buffer)) : *reinterpret_cast<const T*>(buffer);
    buffer += sizeof(T);
}

template <StringType T> void from_bytes(T& value, const uint8_t*& buffer)
{
    static_assert(!std::is_const_v<T>, "T must not be const");

    value = *reinterpret_cast<const T*>(buffer);
    buffer += sizeof(T);
}

class Serializer
{
public:
    explicit Serializer(std::vector<uint8_t>& inBuffer, const uint8_t*& inBufferPointerReference)
        : buffer(inBuffer), bufferPointerReference(inBufferPointerReference)
    {
    }

    template <typename... Args> Serializer& operator()(Args&... args)
    {
        (visit(args), ...);
        return *this;
    }

private:
    std::vector<uint8_t>& buffer;
    const uint8_t*&       bufferPointerReference;

    void visit(SerializableNonScalar auto& value) { serialize(value, (*this)); }

    template <ScalarType T, std::enable_if_t<std::is_const_v<T>, bool> = true> void visit(T& value) { to_bytes(value, buffer); }

    template <ScalarType T, std::enable_if_t<!std::is_const_v<T>, bool> = true> void visit(T& value)
    {
        from_bytes(value, bufferPointerReference);
    }
};

//    class Deserializer {
//        const uint8_t *&buf;
//
//    public:
//        constexpr explicit Deserializer(const uint8_t *&buf) : buf(buf) {}
//
//        template<typename T>
//        requires IntegralType<T> || StringType<T>
//        void operator()(T &value) {
//            from_bytes(value, buf);
//        }
//
//        template<typename... Ts>
//        void operator()(Ts &... values) {
//            (from_bytes(values, buf), ...);
//        }
//    };

// serialize for basic types
template <typename T>
    requires IntegralType<T> || StringType<T>
auto serialize(const T& value, Serializer& serializer)
{
    return serializer(value);
}

//    // from_tuple for basic types
//    template<typename T>
//    requires IntegralType<T> || StringType<T>
//    void from_tuple(T &value, Deserializer &s) {
//        s(value);
//    }

template <Serializable T> std::vector<uint8_t> serialize(const T& object)
{
    std::vector<uint8_t> buffer;
    const uint8_t*       data = buffer.data();
    Serializer           serializer(buffer, data);
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
    Serializer     serializer(buffer, data);
    serialize(object, serializer);
    // from_tuple(obj, d);
    return object;
}
} // namespace borsh
