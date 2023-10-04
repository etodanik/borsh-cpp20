#include "borsh.hpp"
#include "boost/ut.hpp"

struct Point
{
    int32_t x;
    int32_t y;
};

struct Sample
{
    int32_t     x;
    bool        flag;
    std::string name;
    Point       subSample;
};

auto serialize(Point& data, borsh::Serializer& serializer)
{
    return serializer(data.x, data.y);
}

auto serialize(Sample& sample, borsh::Serializer& serializer)
{
    return serializer(sample.x, sample.flag, sample.name, sample.subSample);
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
            auto   serializedMax8 = serialize(max8);
            expect(eq(serializedMax8.size(), sizeof(int8_t)));
            expect(eq(deserialize<int8_t>(serializedMax8), INT8_MAX));
            auto serializedMin8 = serialize(min8);
            expect(eq(serializedMin8.size(), sizeof(int8_t)));
            expect(eq(deserialize<int8_t>(serializedMin8), INT8_MIN));

            int16_t max16 = INT16_MAX;
            int16_t min16 = INT16_MIN;
            auto    serializedMax16 = serialize(max16);
            expect(eq(serializedMax16.size(), sizeof(int16_t)));
            expect(eq(deserialize<int16_t>(serializedMax16), INT16_MAX));
            auto serializedMin16 = serialize(min16);
            expect(eq(serializedMin16.size(), sizeof(int16_t)));
            expect(eq(deserialize<int16_t>(serializedMin16), INT16_MIN));

            int32_t max32 = INT32_MAX;
            int32_t min32 = INT32_MIN;
            auto    serializedMax32 = serialize(max32);
            expect(eq(serializedMax32.size(), sizeof(int32_t)));
            expect(eq(deserialize<int32_t>(serializedMax32), INT32_MAX));
            auto serializedMin32 = serialize(min32);
            expect(eq(serializedMin32.size(), sizeof(int32_t)));
            expect(eq(deserialize<int32_t>(serializedMin32), INT32_MIN));

            int64_t max64 = INT64_MAX;
            int64_t min64 = INT64_MIN;
            auto    serializedMax64 = serialize(max64);
            expect(eq(serializedMax64.size(), sizeof(int64_t)));
            expect(eq(deserialize<int64_t>(serializedMax64), INT64_MAX));
            auto serializedMin64 = serialize(min64);
            expect(eq(serializedMin64.size(), sizeof(int64_t)));
            expect(eq(deserialize<int64_t>(serializedMin64), INT64_MIN));
        };

        "boolean"_test = [] {
            using namespace borsh;

            auto serializedTrue = serialize(true);
            auto serializedFalse = serialize(false);
            expect(eq(serializedTrue.size(), sizeof(bool)) and eq(deserialize<bool>(serializedTrue), true));
            expect(eq(serializedFalse.size(), sizeof(bool)) and eq(deserialize<bool>(serializedFalse), false));
        };

        "string"_test = [] {
            using namespace borsh;
            auto string = std::string("hello");
            auto serializedString = serialize(string);
            expect(eq(static_cast<int>(serializedString.size()), 9));
            auto deserializedString = deserialize<std::string>(serializedString);
            expect(eq(deserializedString, string));
        };

        "struct"_test = [] {
            using namespace borsh;

            Point point{ 10, 20 };
            auto  buffer = serialize(point);
            expect(eq(buffer.size(), sizeof(int32_t) * 2));
            auto deserialized = deserialize<Point>(buffer);
            expect(eq(deserialized.x, 10) and eq(deserialized.y, 20));
        };
    };
}

//
// TEST_CASE("integers") {

//}
//
// TEST_CASE("boolean") {

//}
//
////TEST_CASE("string") {

////}
//
// TEST_CASE("struct") {
//    using namespace borsh;
//
//    SubSample ss{10, 20};
//    auto buffer = serialize(ss);
//    auto deserialized = deserialize<SubSample>(buffer);
//    expect(deserialized.x == 10);
//    expect(deserialized.y == 20);
//}
//
// TEST_CASE("integers") {
////    auto buffer = serialize(42);
////    std::cout << "Serialized int size: " << buffer.size() << std::endl;
//
//    using namespace borsh;
//
//    SubSample ss{10, 20};
//    Sample s{42, true, "hello", ss};
//
//    auto buffer = serialize(s);
//
////    auto deserialized = deserialize<Sample>(buffer);
////
////    expect(deserialized.x == 42);
////    expect(deserialized.flag == true);
////    expect(deserialized.name == "hello");
//
//    std::cout << "Serialized struct size: " << buffer.size() << std::endl;
//
//    // ... Use buffer as needed.
//}
