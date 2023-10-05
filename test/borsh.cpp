#include "borsh.hpp"
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

auto serialize(Vector2D& data, borsh::Serializer& serializer)
{
    return serializer(data.x, data.y);
}

auto serialize(Line& data, borsh::Serializer& serializer)
{
    return serializer(data.a, data.b, data.name);
}

int main()
{
    using namespace boost::ut;
    using namespace borsh;

    "custom concepts"_test = [] {
        "should all pass asserts"_test = [] {
            static_assert(is_bounded_char_array_v<char[10]>);
            static_assert(is_bounded_char_array_v<const char[15]>);
            static_assert(!is_bounded_char_array_v<char[]>);
            static_assert(!is_bounded_char_array_v<const char[]>);

            static_assert(is_bounded_char_array_v<unsigned char[10]>);
            static_assert(is_bounded_char_array_v<const unsigned char[15]>);
            static_assert(!is_bounded_char_array_v<unsigned char[]>);
            static_assert(!is_bounded_char_array_v<const unsigned char[]>);

            static_assert(StringType<std::string>);

            static_assert(CharArrayType<char[10]>);
            static_assert(CharArrayType<const char[15]>);
            static_assert(!StringType<char[]>);
            static_assert(!StringType<const char[]>);

            static_assert(CharArrayType<unsigned char[10]>);
            static_assert(CharArrayType<const unsigned char[15]>);
            static_assert(!StringType<unsigned char[]>);
            static_assert(!StringType<const unsigned char[]>);

            static_assert(Serializable<int8_t>);
            static_assert(Serializable<int16_t>);
            static_assert(Serializable<int32_t>);
            static_assert(Serializable<int64_t>);
            static_assert(Serializable<bool>);

            static_assert(Serializable<uint8_t>);
            static_assert(Serializable<uint16_t>);
            static_assert(Serializable<uint32_t>);
            static_assert(Serializable<uint64_t>);

            //            static_assert(Serializable<float>);
            //            static_assert(Serializable<double>);
            //            static_assert(Serializable<long double>);

            static_assert(Serializable<std::string>);

            // TODO: Implement character arrays
            //    static_assert(Serializable<char[10]>);
            //    static_assert(Serializable<const char[15]>);
            //    static_assert(!Serializable<char[]>);
            //    static_assert(!Serializable<const char[]>);
            //
            //    static_assert(Serializable<unsigned char[10]>);
            //    static_assert(Serializable<const unsigned char[15]>);
            //    static_assert(!Serializable<unsigned char[]>);
            //    static_assert(!Serializable<const unsigned char[]>);
        };
    };

    "types"_test = [] {
        "integers"_test = [] {
            using namespace borsh;

            int8_t max8 = INT8_MAX;
            int8_t min8 = INT8_MIN;

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
        };

        "boolean"_test = [] {
            using namespace borsh;

            auto serializedTrue = serialize(true);
            auto serializedFalse = serialize(false);

            expect(eq(serializedTrue.size(), sizeof(bool)) and eq(deserialize<bool>(serializedTrue), true)
                and eq(serializedTrue, std::vector<uint8_t>{ 0x00000001 }));
            expect(eq(serializedFalse.size(), sizeof(bool)) and eq(deserialize<bool>(serializedFalse), false)
                and eq(serializedFalse, std::vector<uint8_t>{ 0x00000000 }));
        };

        "string"_test = [] {
            using namespace borsh;

            auto string = std::string("hello 🚀");

            auto serializedString = serialize(string);
            expect(eq(static_cast<int>(serializedString.size()), 14));
            expect(eq(serializedString,
                std::vector<uint8_t>{
                    0b00001010, 0b00000000, 0b00000000, 0b00000000, // int32_t representation of the string length (little endian)
                    0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x20, 0xf0, 0x9f, 0x9a, 0x80 // utf-8 string
                }));

            auto deserializedString = deserialize<std::string>(serializedString);
            expect(eq(deserializedString, string));
        };

        "struct"_test = [] {
            using namespace borsh;

            Vector2D point{ 10, 20 };
            auto     buffer = serialize(point);
            expect(eq(buffer.size(), sizeof(int32_t) * 2));
            auto deserialized = deserialize<Vector2D>(buffer);
            expect(eq(deserialized.x, 10) and eq(deserialized.y, 20));
        };

        "nested struct"_test = [] {
            using namespace borsh;

            Line line{ { 5, 10 }, { 15, 25 }, "my line" };

            auto buffer = serialize(line);
            expect(eq(static_cast<int>(buffer.size()), 27));
            auto deserialized = deserialize<Line>(buffer);
            expect(eq(deserialized.a.x, 5) and eq(deserialized.a.y, 10));
            expect(eq(deserialized.b.x, 15) and eq(deserialized.b.y, 25));
            expect(eq(deserialized.name, std::string("my line")));
        };
    };
}
