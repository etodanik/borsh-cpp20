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

#pragma once
#ifndef BORSH_CPP20_TEMPLATES_H
#define BORSH_CPP20_TEMPLATES_H

#include "concepts.h"

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
#include <cmath>
#include <memory>

namespace borsh
{

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

template <typename T, std::size_t N>
auto serialize(std::array<T, N>& value, Serializer& serializer)
    requires Serializable<T>
{
    return serializer(value);
}

template <typename T>
    requires ScalarType<T> || ScalarArrayType<T> || ScalarStdArrayType<T>
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

#endif