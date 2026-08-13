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
#include <vector>
#include <cstdint>
#include <algorithm>

namespace borsh
{

constexpr Error ensure_consumed(std::size_t remaining) noexcept
{
    return remaining ? Error::TrailingBytes : Error::None;
}

template <typename T, std::size_t N>
Error serialize(const T (&array)[N], Encoder& encoder)
    requires Serializable<T>
{
    for (const auto& item : array)
    {
        encoder(item);
    }
    return encoder.status();
}

Error serialize(const ScalarType auto& value, Encoder& encoder)
{
    return encoder(value);
}

Error serialize(const SerializableVector auto& value, Encoder& encoder)
{
    return encoder(value);
}

template <typename T, std::size_t N>
Error serialize(const std::array<T, N>& value, Encoder& encoder)
    requires Serializable<T>
{
    for (const auto& item : value)
    {
        encoder(item);
    }
    return encoder.status();
}

template <typename T, std::size_t N>
Error deserialize(T (&array)[N], Decoder& decoder)
    requires Serializable<T>
{
    for (auto& item : array)
    {
        decoder(item);
    }
    return decoder.status();
}

Error deserialize(ScalarType auto& value, Decoder& decoder)
{
    return decoder(value);
}

Error deserialize(SerializableVector auto& value, Decoder& decoder)
{
    return decoder(value);
}

template <typename T, std::size_t N>
Error deserialize(std::array<T, N>& value, Decoder& decoder)
    requires Serializable<T>
{
    for (auto& item : value)
    {
        decoder(item);
    }
    return decoder.status();
}

template <typename T> [[nodiscard]] Error serialize(const T& value, std::vector<uint8_t>& output)
{
    output.clear();
    Encoder encoder(output);
    auto    error = encoder(value);
    if (error != Error::None)
    {
        output.clear();
    }
    return error;
}

template <typename T> [[nodiscard]] Error deserialize(T& output, const std::vector<uint8_t>& buffer)
{
    Decoder decoder(buffer);
    auto    error = decoder(output);
    if (error == Error::None)
    {
        error = ensure_consumed(decoder.remaining_bytes());
    }
    return error;
}

} // namespace borsh

#endif
