#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <vector>
#include <string>

#include "borsh.h"
#include "boost/ut.hpp"

struct Vector2D
{
    int32_t x;
    int32_t y;
};

struct Line
{
    Vector2D    a;
    Vector2D    b;
    std::string name;
};

struct Box
{
    std::array<int, 2> dimensions;
    std::string name;
};

struct Misaligned
{
    uint8_t  byte;
    uint32_t word;
};

auto serialize(const Vector2D& data, borsh::Encoder& encoder)
{
    return encoder(data.x, data.y);
}

auto deserialize(Vector2D& data, borsh::Decoder& decoder)
{
    return decoder(data.x, data.y);
}

auto serialize(const Line& data, borsh::Encoder& encoder)
{
    return encoder(data.a, data.b, data.name);
}

auto deserialize(Line& data, borsh::Decoder& decoder)
{
    return decoder(data.a, data.b, data.name);
}

auto serialize(const Box& data, borsh::Encoder& encoder)
{
    return encoder(data.dimensions, data.name);
}

auto deserialize(Box& data, borsh::Decoder& decoder)
{
    return decoder(data.dimensions, data.name);
}

auto serialize(const Misaligned& data, borsh::Encoder& encoder)
{
    return encoder(data.byte, data.word);
}

auto deserialize(Misaligned& data, borsh::Decoder& decoder)
{
    return decoder(data.byte, data.word);
}

template <typename T> auto expect_serialize_without_error(T&& value)
{
    std::vector<uint8_t> bytes;
    boost::ut::expect(borsh::serialize(value, bytes) == borsh::Error::None);
    return bytes;
}

template <typename T> auto expect_deserialize_without_error(const std::vector<uint8_t>& bytes)
{
    T value{};
    boost::ut::expect(borsh::deserialize(value, bytes) == borsh::Error::None);
    return value;
}

int main()
{
    using namespace boost::ut;
    using namespace borsh;

    "custom concepts"_test = [] {
        "should all pass asserts"_test = [] {
            static_assert(ArrayType<char[10]>);
            static_assert(ArrayType<const char[15]>);
            static_assert(!ArrayType<char[]>);
            static_assert(!ArrayType<const char[]>);

            static_assert(ArrayType<unsigned char[10]>);
            static_assert(ArrayType<const unsigned char[15]>);
            static_assert(!ArrayType<unsigned char[]>);
            static_assert(!ArrayType<const unsigned char[]>);

            static_assert(StringType<std::string>);

            static_assert(CharArrayType<char[10]>);
            static_assert(CharArrayType<const char[15]>);
            static_assert(!StringType<char[]>);
            static_assert(!StringType<const char[]>);

            static_assert(CharArrayType<unsigned char[10]>);
            static_assert(CharArrayType<const unsigned char[15]>);
            static_assert(!StringType<unsigned char[]>);
            static_assert(!StringType<const unsigned char[]>);

            static_assert(Serializable<char[10]>);
            static_assert(Serializable<const char[15]>);
            static_assert(!Serializable<char[]>);
            static_assert(!Serializable<const char[]>);

            static_assert(Serializable<unsigned char[10]>);
            static_assert(Serializable<const unsigned char[15]>);
            static_assert(!Serializable<unsigned char[]>);
            static_assert(!Serializable<const unsigned char[]>);
        };
    };

    "types"_test = [] {
        "integers"_test = [] {
            static_assert(Serializable<int8_t>);
            static_assert(Serializable<int16_t>);
            static_assert(Serializable<int32_t>);
            static_assert(Serializable<int64_t>);
            static_assert(Serializable<uint8_t>);
            static_assert(Serializable<uint16_t>);
            static_assert(Serializable<uint32_t>);
            static_assert(Serializable<uint64_t>);
#ifdef BORSH_HAVE_INTRINSIC_INT128
            static_assert(Serializable<int128_t>);
            static_assert(Serializable<uint128_t>);
#endif

            const int8_t max8 = INT8_MAX;
            const int8_t min8 = INT8_MIN;

            auto serializedMax8 = expect_serialize_without_error(max8);
            expect(eq(serializedMax8.size(), sizeof(int8_t)));
            expect(eq(serializedMax8, std::vector<uint8_t>{ 0b01111111 }));
            expect(eq(expect_deserialize_without_error<int8_t>(serializedMax8), INT8_MAX));

            auto serializedMin8 = expect_serialize_without_error(min8);
            expect(eq(serializedMin8.size(), sizeof(int8_t)));
            expect(eq(serializedMin8, std::vector<uint8_t>{ 0b10000000 }));
            expect(eq(expect_deserialize_without_error<int8_t>(serializedMin8), INT8_MIN));

            int16_t max16 = INT16_MAX;
            int16_t min16 = INT16_MIN;

            auto serializedMax16 = expect_serialize_without_error(max16);
            expect(eq(serializedMax16.size(), sizeof(int16_t)));
            expect(eq(serializedMax16, std::vector<uint8_t>{ 0b11111111, 0b01111111 }));
            expect(eq(expect_deserialize_without_error<int16_t>(serializedMax16), INT16_MAX));

            auto serializedMin16 = expect_serialize_without_error(min16);
            expect(eq(serializedMin16.size(), sizeof(int16_t)));
            expect(eq(serializedMin16, std::vector<uint8_t>{ 0b00000000, 0b10000000 }));
            expect(eq(expect_deserialize_without_error<int16_t>(serializedMin16), INT16_MIN));

            int32_t max32 = INT32_MAX;
            int32_t min32 = INT32_MIN;

            auto serializedMax32 = expect_serialize_without_error(max32);
            expect(eq(serializedMax32.size(), sizeof(int32_t)));
            expect(eq(serializedMax32, std::vector<uint8_t>{ 0b11111111, 0b11111111, 0b11111111, 0b01111111 }));
            expect(eq(expect_deserialize_without_error<int32_t>(serializedMax32), INT32_MAX));

            auto serializedMin32 = expect_serialize_without_error(min32);
            expect(eq(serializedMin32.size(), sizeof(int32_t)));
            expect(eq(serializedMin32, std::vector<uint8_t>{ 0b00000000, 0b00000000, 0b00000000, 0b10000000 }));
            expect(eq(expect_deserialize_without_error<int32_t>(serializedMin32), INT32_MIN));

            int64_t max64 = INT64_MAX;
            int64_t min64 = INT64_MIN;

            auto serializedMax64 = expect_serialize_without_error(max64);
            expect(eq(serializedMax64.size(), sizeof(int64_t)));
            expect(eq(serializedMax64,
                std::vector<uint8_t>{ 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b01111111 }));
            expect(eq(expect_deserialize_without_error<int64_t>(serializedMax64), INT64_MAX));

            auto serializedMin64 = expect_serialize_without_error(min64);
            expect(eq(serializedMin64.size(), sizeof(int64_t)));
            expect(eq(serializedMin64,
                std::vector<uint8_t>{ 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b10000000 }));
            expect(eq(expect_deserialize_without_error<int64_t>(serializedMin64), INT64_MIN));

#ifdef BORSH_HAVE_INTRINSIC_INT128
            int128_t max128 = INT128_MAX;
            int128_t min128 = INT128_MIN;

            auto serializedMax128 = expect_serialize_without_error(max128);
            expect(serializedMax128.size() == sizeof(int128_t));
            expect(serializedMax128
                == std::vector<uint8_t>{ 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111,
                    0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b01111111 });
            expect(expect_deserialize_without_error<int128_t>(serializedMax128) == INT128_MAX);

            auto serializedMin128 = expect_serialize_without_error(min128);
            expect(serializedMin128.size() == sizeof(int128_t));
            expect(serializedMin128
                == std::vector<uint8_t>{ 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
                    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b10000000 });
            expect(expect_deserialize_without_error<int128_t>(serializedMin128) == INT128_MIN);
#endif
        };

        "float"_test = [] {
            static_assert(float_to_int(1.0f) == 0x3f800000);
            static_assert(int_to_float(0x3f800000) == 1.0f);
            static_assert(Serializable<float>);
            float floatValue = 3.1415927f;
            auto  serializedFloat = expect_serialize_without_error(floatValue);
            expect(eq(serializedFloat.size(), sizeof(float)));
            expect(eq(serializedFloat, std::vector<uint8_t>{ 0b11011011, 0b00001111, 0b01001001, 0b01000000 }));
            auto deserializedFloat = expect_deserialize_without_error<float>(serializedFloat);
            expect(eq(deserializedFloat, floatValue));

            static_assert(Serializable<double>);
            double doubleValue = 3.141592653589793;
            auto   serializedDouble = expect_serialize_without_error(doubleValue);
            expect(eq(serializedDouble.size(), sizeof(double)));
            expect(eq(serializedDouble,
                std::vector<uint8_t>{ 0b00011000, 0b00101101, 0b01000100, 0b01010100, 0b11111011, 0b00100001, 0b00001001, 0b01000000 }));
            auto deserializedDouble = expect_deserialize_without_error<double>(serializedDouble);
            expect(eq(deserializedDouble, doubleValue));

            static_assert(!Serializable<long double>);
        };

        "bool"_test = [] {
            static_assert(Serializable<bool>);

            auto serializedTrue = expect_serialize_without_error(true);
            auto serializedFalse = expect_serialize_without_error(false);

            expect(eq(serializedTrue.size(), sizeof(bool)) and eq(expect_deserialize_without_error<bool>(serializedTrue), true)
                and eq(serializedTrue, std::vector<uint8_t>{ 0x00000001 }));
            expect(eq(serializedFalse.size(), sizeof(bool)) and eq(expect_deserialize_without_error<bool>(serializedFalse), false)
                and eq(serializedFalse, std::vector<uint8_t>{ 0x00000000 }));
        };

        "string"_test = [] {
            static_assert(Serializable<std::string>);

            auto string = std::string("hello 🚀");

            auto serializedString = expect_serialize_without_error(string);
            expect(eq(serializedString.size(), static_cast<size_t>(14)));
            expect(eq(serializedString,
                std::vector<uint8_t>{ //
                    // int32_t representation of the string length (little endian)
                    0b00001010, 0b00000000, 0b00000000, 0b00000000,
                    // utf-8 string
                    0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x20, 0xf0, 0x9f, 0x9a, 0x80 }));

            auto deserializedString = expect_deserialize_without_error<std::string>(serializedString);
            expect(eq(deserializedString, string));
        };

        "struct"_test = [] {
            static_assert(Serializable<Vector2D>);

            Vector2D point{ 10, 20 };
            auto     buffer = expect_serialize_without_error(point);
            expect(eq(buffer.size(), sizeof(int32_t) * 2));
            auto deserialized = expect_deserialize_without_error<Vector2D>(buffer);
            expect(eq(deserialized.x, 10) and eq(deserialized.y, 20));
        };

        "struct with std::array"_test = [] {
            static_assert(Serializable<Box>);

            Box point{ {10, 20}, "my box" };
            auto     buffer = expect_serialize_without_error(point);
            expect(eq(buffer.size(), (sizeof(int32_t) * 2) + 10));
            auto deserialized = expect_deserialize_without_error<Box>(buffer);
            expect(eq(deserialized.dimensions.at(0), 10) and eq(deserialized.dimensions.at(1), 20));
            expect(eq(deserialized.name, std::string("my box")));
        };

        "nested struct"_test = [] {
            static_assert(Serializable<Line>);

            Line line{ { 5, 10 }, { 15, 25 }, "my line" };

            auto buffer = expect_serialize_without_error(line);
            expect(eq(static_cast<int>(buffer.size()), 27));
            auto deserialized = expect_deserialize_without_error<Line>(buffer);
            expect(eq(deserialized.a.x, 5) and eq(deserialized.a.y, 10));
            expect(eq(deserialized.b.x, 15) and eq(deserialized.b.y, 25));
            expect(eq(deserialized.name, std::string("my line")));
        };

        "bounded c style array of integers"_test = [] {
            static_assert(Serializable<uint32_t[10]>);
            static_assert(Serializable<const uint32_t[15]>);
            static_assert(!Serializable<uint32_t[]>);
            static_assert(!Serializable<const uint32_t[]>);

            const int32_t array[] = { 15, -20, 10, 3435, -4011 };
            int32_t       deserializedArray[5];

            auto serializedArray = expect_serialize_without_error(array);
            expect(eq(serializedArray.size(), sizeof(int32_t) * 5));
            expect(eq(serializedArray,
                std::vector<uint8_t>{
                    0b00001111,
                    0b00000000,
                    0b00000000,
                    0b00000000,
                    0b11101100,
                    0b11111111,
                    0b11111111,
                    0b11111111,
                    0b00001010,
                    0b00000000,
                    0b00000000,
                    0b00000000,
                    0b01101011,
                    0b00001101,
                    0b00000000,
                    0b00000000,
                    0b01010101,
                    0b11110000,
                    0b11111111,
                    0b11111111,
                }));
            expect(deserialize(deserializedArray, serializedArray) == Error::None);
            expect(std::equal(std::begin(array), std::end(array), std::begin(deserializedArray)));
        };

        "std::array of integers"_test = [] {
            static_assert(!ScalarArrayType<std::array<uint32_t, 10>>);
            static_assert(!NonScalarArrayType<std::array<uint32_t, 10>>);
            static_assert(ScalarStdArrayType<std::array<uint32_t, 10>>);
            static_assert(!NonScalarStdArrayType<std::array<uint32_t, 10>>);
            static_assert(Serializable<std::array<uint32_t, 10>>);
            static_assert(Serializable<const std::array<uint32_t, 15>>);

            const std::array array = { 15, -20, 10, 3435, -4011 };

            auto serializedArray = expect_serialize_without_error(array);
            expect(eq(serializedArray.size(), sizeof(int32_t) * 5));
            expect(eq(serializedArray,
                std::vector<uint8_t>{
                    0b00001111,
                    0b00000000,
                    0b00000000,
                    0b00000000,
                    0b11101100,
                    0b11111111,
                    0b11111111,
                    0b11111111,
                    0b00001010,
                    0b00000000,
                    0b00000000,
                    0b00000000,
                    0b01101011,
                    0b00001101,
                    0b00000000,
                    0b00000000,
                    0b01010101,
                    0b11110000,
                    0b11111111,
                    0b11111111,
                }));
            auto deserializedArray = expect_deserialize_without_error<std::array<int32_t, 5>>(serializedArray);
            expect(std::equal(std::begin(array), std::end(array), std::begin(deserializedArray)));
        };

        "vector of integers"_test = [] {
            static_assert(Serializable<std::vector<int32_t>>);
            static_assert(Serializable<const std::vector<int32_t>>);

            const std::vector<int32_t> vector = { 15, -20, 10, 3435, -4011 };

            auto serializedVector = expect_serialize_without_error(vector);
            expect(eq(serializedVector.size(), sizeof(int32_t) * 6));
            expect(eq(serializedVector,
                std::vector<uint8_t>{
                    0b00000101,
                    0b00000000,
                    0b00000000,
                    0b00000000,
                    0b00001111,
                    0b00000000,
                    0b00000000,
                    0b00000000,
                    0b11101100,
                    0b11111111,
                    0b11111111,
                    0b11111111,
                    0b00001010,
                    0b00000000,
                    0b00000000,
                    0b00000000,
                    0b01101011,
                    0b00001101,
                    0b00000000,
                    0b00000000,
                    0b01010101,
                    0b11110000,
                    0b11111111,
                    0b11111111,
                }));

            auto deserializedVector = expect_deserialize_without_error<std::vector<int32_t>>(serializedVector);
            expect(std::equal(vector.begin(), vector.end(), deserializedVector.begin()));
        };

        "vector of structs"_test = [] {
            static_assert(Serializable<std::vector<Line>>);

            const std::vector<Line> vector = { { { 5, 10 }, { 15, 25 }, "hello 🚀" }, { { 25, 30 }, { 45, 75 }, "olleh 🚀" } };

            auto serializedVector = expect_serialize_without_error(vector);
            expect(eq(serializedVector.size(),
                static_cast<size_t>(
                    // the result should be the raw length of the types with prepended length
                    sizeof(uint32_t)          // length
                    + sizeof(Vector2D) * 2    // two Vector structs
                    + static_cast<size_t>(14) // string
                    + sizeof(Vector2D) * 2    // two Vector structs
                    + static_cast<size_t>(14) // string
                    )));

            auto deserializedVector = expect_deserialize_without_error<std::vector<Line>>(serializedVector);

            expect(eq(deserializedVector.at(0).a.x, 5) and eq(deserializedVector.at(0).a.y, 10));
            expect(eq(deserializedVector.at(0).b.x, 15) and eq(deserializedVector.at(0).b.y, 25));
            expect(eq(deserializedVector.at(0).name, std::string("hello 🚀")));
            expect(eq(deserializedVector.at(1).a.x, 25) and eq(deserializedVector.at(1).a.y, 30));
            expect(eq(deserializedVector.at(1).b.x, 45) and eq(deserializedVector.at(1).b.y, 75));
            expect(eq(deserializedVector.at(1).name, std::string("olleh 🚀")));
        };

        "non-scalar arrays"_test = [] {
            std::array<Vector2D, 2> array{ { { 1, 2 }, { 3, 4 } } };
            auto bytes = expect_serialize_without_error(array);
            auto decoded = expect_deserialize_without_error<std::array<Vector2D, 2>>(bytes);
            expect(decoded[0].x == 1 and decoded[1].y == 4);

            Vector2D cArray[] = { { 5, 6 }, { 7, 8 } };
            Vector2D cDecoded[2];
            bytes = expect_serialize_without_error(cArray);
            expect(deserialize(cDecoded, bytes) == Error::None);
            expect(cDecoded[0].y == 6 and cDecoded[1].x == 7);
        };

        "checked input"_test = [] {
            if constexpr (sizeof(std::size_t) > sizeof(uint32_t))
            {
                std::vector<uint8_t> bytes;
                expect(to_bytes_size(static_cast<std::size_t>(UINT32_MAX) + 1, bytes) == Error::LengthOverflow);
            }

            std::vector<uint8_t> empty;
            uint64_t             integer = 42;
            expect(deserialize(integer, empty) == Error::UnexpectedEnd and integer == 42);

            std::vector<uint8_t> shortString{ 2, 0, 0, 0, 'a' };
            std::string          string = "unchanged";
            expect(deserialize(string, shortString) == Error::UnexpectedEnd and string == "unchanged");

            std::vector<uint8_t> invalidLength{ 0xff, 0xff, 0xff, 0xff };
            std::vector<uint8_t> vector{ 42 };
            expect(deserialize(vector, invalidLength) == Error::UnexpectedEnd and vector == std::vector<uint8_t>{ 42 });

            std::vector<uint8_t> invalidBool{ 2 };
            bool                 boolean = true;
            expect(deserialize(boolean, invalidBool) == Error::InvalidBool and boolean);

            std::vector<uint8_t> nan{ 0, 0, 0xc0, 0x7f };
            float                number = 1;
            expect(deserialize(number, nan) == Error::InvalidFloat and std::isnan(number));

            std::vector<uint8_t> trailing{ 1, 0 };
            uint8_t              byte = 42;
            expect(deserialize(byte, trailing) == Error::TrailingBytes and byte == 1);

            auto                 invalidFloat = std::numeric_limits<float>::quiet_NaN();
            std::vector<uint8_t> output{ 42 };
            output.reserve(64);
            const auto capacity = output.capacity();
            expect(serialize(invalidFloat, output) == Error::InvalidFloat and output.empty() and output.capacity() == capacity);
        };

        "unaligned field"_test = [] {
            Misaligned value{ 1, 0x12345678 };
            auto bytes = expect_serialize_without_error(value);
            auto decoded = expect_deserialize_without_error<Misaligned>(bytes);
            expect(decoded.byte == value.byte and decoded.word == value.word);
        };
    };
}
