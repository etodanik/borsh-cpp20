# Borsh for C++20

`borsh-cpp20` is an implementation of the borsh serialization specification for C++20.

## Motivation

Basically, at the time of writing there was no feature complete borsh serializer / deserializer implementation available
for C++ at all, so this code is an attempt to fill that gap.

## Features

* Copy-free serialization and deserialization.
* No dependency on exceptions.
* Tests cover supported types, malformed input, and trailing bytes.
* CI builds and tests the library with GCC, LLVM Clang, Apple Clang, and MSVC.

## Usage

Scalar types serialize and deserialize by simply calling `borsh::serialize` or `borsh::deserialize`:
```c++
#include <borsh.h>

#include <cstdint>
#include <vector>

std::int32_t value_to_serialize{35};
std::vector<std::uint8_t> bytes;
borsh::Error serialization_error = borsh::serialize(value_to_serialize, bytes);
std::int32_t deserialized_value{};
borsh::Error deserialization_error = borsh::deserialize(deserialized_value, bytes);
```

Custom struct types will require some unavoidable boilerplate in the form of enumerating the struct fields for the custom type:
```c++
#include <borsh.h>

#include <cstdint>

struct Vector2D
{
    std::int32_t x;
    std::int32_t y;
};

// the following boilerplate is necessary for the compiler 
// to be aware of the fields in your struct
auto serialize(const Vector2D& data, borsh::Encoder& encoder)
{
    return encoder(data.x, data.y);
}

auto deserialize(Vector2D& data, borsh::Decoder& decoder)
{
    return decoder(data.x, data.y);
}
```

This is because C++20 lacks the ability to introspect on struct fields.

## Error handling

Upon a failed deserialization, the borsh serialization/deserialization functions will return one of the following errors:
```c++
enum class Error : uint8_t
{
    None,
    UnexpectedEnd,
    TrailingBytes,
    InvalidBool,
    InvalidFloat,
    LengthOverflow,
};
```

Keep in mind that because the serialization and deserialization happen directly to the user provided buffer in a copy-free manner, a failed operation will leave that buffer in an invalid state that should not be read. 

## Current state

The library isn't ready for production, and the code is published just for building in public. Please don't use it until
it is.

Below is a list of types specified in the Rust specification, with the ones implemented checked. Every checked type is
tested to be binary compatible with the borsh specification:

- [x] 
  Integers (`int8_t`, `int16_t`, `int32_t`, `int64_t`, `__int128`, `uint8_t`, `uint16_t`, `uint32_t`, `uint64_t`, `unsigned __int128`,
  `bool`)
- [x] Bool
- [x] Floats (`float`, `double`)
- [ ] Unit (`std::monostate`), a noop in Borsh
- [x] Fixed sized arrays (`C-style array[]`, `std::array`)
- [x] Dynamically sized array (`std::vector`)
- [x] Non-scalar array types
- [x] Struct
- [x] Named fields
- [ ] Enum
- [ ] HashMap (`std::unordered_map`)
- [ ] HashSet (`std::unordered_set`)
- [ ] Option (`std::optional`)
- [x] String (`std::string`)

The following types don't have a direct equivalent in C++:

- Unnamed fields
