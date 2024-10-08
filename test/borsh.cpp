#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <vector>
#include <string>

#include "borsh.h"

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

auto serialize(Vector2D& data, borsh::Serializer& serializer)
{
    return serializer(data.x, data.y);
}

auto serialize(Line& data, borsh::Serializer& serializer)
{
    return serializer(data.a, data.b, data.name);
}

auto serialize(Box& data, borsh::Serializer& serializer)
{
    return serializer(data.dimensions, data.name);
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

            auto serializedMax8 = serialize(max8);
            expect(eq(serializedMax8.size(), sizeof(int8_t)));
            expect(eq(serializedMax8, std::vector<uint8_t>{ 0b01111111 }));
            expect(eq(deserialize<int8_t>(serializedMax8), INT8_MAX));

            auto serializedMin8 = serialize(min8);
            expect(eq(serializedMin8.size(), sizeof(int8_t)));
            expect(eq(serializedMin8, std::vector<uint8_t>{ 0b10000000 }));
            expect(eq(deserialize<int8_t>(serializedMin8), INT8_MIN));

            int16_t max16 = INT16_MAX;
            int16_t min16 = INT16_MIN;

            auto serializedMax16 = serialize(max16);
            expect(eq(serializedMax16.size(), sizeof(int16_t)));
            expect(eq(serializedMax16, std::vector<uint8_t>{ 0b11111111, 0b01111111 }));
            expect(eq(deserialize<int16_t>(serializedMax16), INT16_MAX));

            auto serializedMin16 = serialize(min16);
            expect(eq(serializedMin16.size(), sizeof(int16_t)));
            expect(eq(serializedMin16, std::vector<uint8_t>{ 0b00000000, 0b10000000 }));
            expect(eq(deserialize<int16_t>(serializedMin16), INT16_MIN));

            int32_t max32 = INT32_MAX;
            int32_t min32 = INT32_MIN;

            auto serializedMax32 = serialize(max32);
            expect(eq(serializedMax32.size(), sizeof(int32_t)));
            expect(eq(serializedMax32, std::vector<uint8_t>{ 0b11111111, 0b11111111, 0b11111111, 0b01111111 }));
            expect(eq(deserialize<int32_t>(serializedMax32), INT32_MAX));

            auto serializedMin32 = serialize(min32);
            expect(eq(serializedMin32.size(), sizeof(int32_t)));
            expect(eq(serializedMin32, std::vector<uint8_t>{ 0b00000000, 0b00000000, 0b00000000, 0b10000000 }));
            expect(eq(deserialize<int32_t>(serializedMin32), INT32_MIN));

            int64_t max64 = INT64_MAX;
            int64_t min64 = INT64_MIN;

            auto serializedMax64 = serialize(max64);
            expect(eq(serializedMax64.size(), sizeof(int64_t)));
            expect(eq(serializedMax64,
                std::vector<uint8_t>{ 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b01111111 }));
            expect(eq(deserialize<int64_t>(serializedMax64), INT64_MAX));

            auto serializedMin64 = serialize(min64);
            expect(eq(serializedMin64.size(), sizeof(int64_t)));
            expect(eq(serializedMin64,
                std::vector<uint8_t>{ 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b10000000 }));
            expect(eq(deserialize<int64_t>(serializedMin64), INT64_MIN));

#ifdef BORSH_HAVE_INTRINSIC_INT128
            int128_t max128 = INT128_MAX;
            int128_t min128 = INT128_MIN;

            auto serializedMax128 = serialize(max128);
            expect(serializedMax128.size() == sizeof(int128_t));
            expect(serializedMax128
                == std::vector<uint8_t>{ 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111,
                    0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b01111111 });
            expect(deserialize<int128_t>(serializedMax128) == INT128_MAX);

            auto serializedMin128 = serialize(min128);
            expect(serializedMin128.size() == sizeof(int128_t));
            expect(serializedMin128
                == std::vector<uint8_t>{ 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
                    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b10000000 });
            expect(deserialize<int128_t>(serializedMin128) == INT128_MIN);
#endif
        };

        "float"_test = [] {
            static_assert(Serializable<float>);
            float floatValue = 3.1415927f;
            auto  serializedFloat = serialize(floatValue);
            expect(eq(serializedFloat.size(), sizeof(float)));
            expect(eq(serializedFloat, std::vector<uint8_t>{ 0b11011011, 0b00001111, 0b01001001, 0b01000000 }));
            auto deserializedFloat = deserialize<float>(serializedFloat);
            expect(eq(deserializedFloat, floatValue));

            static_assert(Serializable<double>);
            double doubleValue = 3.141592653589793;
            auto   serializedDouble = serialize(doubleValue);
            expect(eq(serializedDouble.size(), sizeof(double)));
            expect(eq(serializedDouble,
                std::vector<uint8_t>{ 0b00011000, 0b00101101, 0b01000100, 0b01010100, 0b11111011, 0b00100001, 0b00001001, 0b01000000 }));
            auto deserializedDouble = deserialize<double>(serializedDouble);
            expect(eq(deserializedDouble, doubleValue));

#ifdef BORSH_HAVE_INTRINSIC_INT128
            static_assert(Serializable<long double>);
            long double longDoubleValue = 3.1415926535897932385L;
            auto        serializedLongDouble = serialize(longDoubleValue);
            expect(eq(serializedLongDouble.size(), sizeof(long double)));
            expect(eq(serializedDouble,
                std::vector<uint8_t>{ 0b00011000, 0b00101101, 0b01000100, 0b01010100, 0b11111011, 0b00100001, 0b00001001, 0b01000000 }));
            auto deserializedLongDouble = deserialize<long double>(serializedLongDouble);
            expect(eq(deserializedLongDouble, longDoubleValue));
#endif
        };

        "bool"_test = [] {
            static_assert(Serializable<bool>);

            auto serializedTrue = serialize(true);
            auto serializedFalse = serialize(false);

            expect(eq(serializedTrue.size(), sizeof(bool)) and eq(deserialize<bool>(serializedTrue), true)
                and eq(serializedTrue, std::vector<uint8_t>{ 0x00000001 }));
            expect(eq(serializedFalse.size(), sizeof(bool)) and eq(deserialize<bool>(serializedFalse), false)
                and eq(serializedFalse, std::vector<uint8_t>{ 0x00000000 }));
        };

        "string"_test = [] {
            static_assert(Serializable<std::string>);

            auto string = std::string("hello 🚀");

            auto serializedString = serialize(string);
            expect(eq(serializedString.size(), static_cast<size_t>(14)));
            expect(eq(serializedString,
                std::vector<uint8_t>{ //
                    // int32_t representation of the string length (little endian)
                    0b00001010, 0b00000000, 0b00000000, 0b00000000,
                    // utf-8 string
                    0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x20, 0xf0, 0x9f, 0x9a, 0x80 }));

            auto deserializedString = deserialize<std::string>(serializedString);
            expect(eq(deserializedString, string));
        };

        "struct"_test = [] {
            static_assert(Serializable<Vector2D>);

            Vector2D point{ 10, 20 };
            auto     buffer = serialize(point);
            expect(eq(buffer.size(), sizeof(int32_t) * 2));
            auto deserialized = deserialize<Vector2D>(buffer);
            expect(eq(deserialized.x, 10) and eq(deserialized.y, 20));
        };

        "struct with std::array"_test = [] {
            static_assert(Serializable<Box>);

            Box point{ {10, 20}, "my box" };
            auto     buffer = serialize(point);
            expect(eq(buffer.size(), (sizeof(int32_t) * 2) + 10));
            auto deserialized = deserialize<Box>(buffer);
            expect(eq(deserialized.dimensions.at(0), 10) and eq(deserialized.dimensions.at(1), 20));
            expect(eq(deserialized.name, std::string("my box")));
        };

        "nested struct"_test = [] {
            static_assert(Serializable<Line>);

            Line line{ { 5, 10 }, { 15, 25 }, "my line" };

            auto buffer = serialize(line);
            expect(eq(static_cast<int>(buffer.size()), 27));
            auto deserialized = deserialize<Line>(buffer);
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

            auto serializedArray = serialize(array);
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
            deserialize(deserializedArray, serializedArray);
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

            auto serializedArray = serialize(array);
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
            auto deserializedArray = deserialize<std::array<int32_t, 5>>(serializedArray);
            expect(std::equal(std::begin(array), std::end(array), std::begin(deserializedArray)));
        };

        "vector of integers"_test = [] {
            static_assert(Serializable<std::vector<int32_t>>);
            static_assert(Serializable<const std::vector<int32_t>>);

            const std::vector<int32_t> vector = { 15, -20, 10, 3435, -4011 };

            auto serializedVector = serialize(vector);
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

            auto deserializedVector = deserialize<std::vector<int32_t>>(serializedVector);
            expect(std::equal(vector.begin(), vector.end(), deserializedVector.begin()));
        };

        "vector of structs"_test = [] {
            static_assert(Serializable<std::vector<Line>>);

            const std::vector<Line> vector = { { { 5, 10 }, { 15, 25 }, "hello 🚀" }, { { 25, 30 }, { 45, 75 }, "olleh 🚀" } };

            auto serializedVector = serialize(vector);
            expect(eq(serializedVector.size(),
                static_cast<size_t>(
                    // the result should be the raw length of the types with prepended length
                    sizeof(uint32_t)          // length
                    + sizeof(Vector2D) * 2    // two Vector structs
                    + static_cast<size_t>(14) // string
                    + sizeof(Vector2D) * 2    // two Vector structs
                    + static_cast<size_t>(14) // string
                    )));

            auto deserializedVector = deserialize<std::vector<Line>>(serializedVector);

            expect(eq(deserializedVector.at(0).a.x, 5) and eq(deserializedVector.at(0).a.y, 10));
            expect(eq(deserializedVector.at(0).b.x, 15) and eq(deserializedVector.at(0).b.y, 25));
            expect(eq(deserializedVector.at(0).name, std::string("hello 🚀")));
            expect(eq(deserializedVector.at(1).a.x, 25) and eq(deserializedVector.at(1).a.y, 30));
            expect(eq(deserializedVector.at(1).b.x, 45) and eq(deserializedVector.at(1).b.y, 75));
            expect(eq(deserializedVector.at(1).name, std::string("olleh 🚀")));
        };
    };
}
