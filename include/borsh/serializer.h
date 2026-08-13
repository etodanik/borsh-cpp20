#pragma once
#ifndef BORSH_CPP20_SERIALIZER_H
#define BORSH_CPP20_SERIALIZER_H

#include <utility>

namespace borsh
{

class Encoder
{
public:
    explicit Encoder(std::vector<uint8_t>& output) : buffer(output) {}

    template <typename... Args> Error operator()(const Args&... args)
    {
        (visit(args), ...);
        return error;
    }

    [[nodiscard]] constexpr Error status() const noexcept { return error; }

private:
    std::vector<uint8_t>& buffer;
    Error                 error = Error::None;

    template <typename T> void visit(const T& value)
    {
        if (error != Error::None)
        {
            return;
        }
        if constexpr (SerializableVector<T>)
        {
            error = to_bytes_size(value.size(), buffer);
            for (const auto& element : value)
            {
                if (error != Error::None)
                {
                    return;
                }
                if constexpr (SerializableNonScalar<typename T::value_type>)
                {
                    error = serialize(element, *this);
                }
                else
                {
                    error = to_bytes(element, buffer);
                }
            }
        }
        else if constexpr (ScalarType<T> || ScalarArrayType<T> || ScalarStdArrayType<T>)
        {
            error = to_bytes(value, buffer);
        }
        else
        {
            error = serialize(value, *this);
        }
    }
};

class Decoder
{
public:
    explicit Decoder(const std::vector<uint8_t>& input) : buffer(input.data()), remaining(input.size()) {}

    template <typename... Args> Error operator()(Args&... args)
    {
        (visit(args), ...);
        return error;
    }

    [[nodiscard]] constexpr std::size_t remaining_bytes() const noexcept { return remaining; }
    [[nodiscard]] constexpr Error       status() const noexcept { return error; }

private:
    const uint8_t* buffer;
    std::size_t    remaining;
    Error          error = Error::None;

    template <typename T> void visit(T& value)
    {
        if (error != Error::None)
        {
            return;
        }
        if constexpr (SerializableVector<T>)
        {
            uint32_t length;
            error = from_bytes(length, buffer, remaining);
            if (error != Error::None || length > remaining)
            {
                error = Error::UnexpectedEnd;
                return;
            }

            value.clear();
            for (uint32_t i = 0; i < length; ++i)
            {
                typename T::value_type element{};
                if constexpr (SerializableNonScalar<typename T::value_type>)
                {
                    error = deserialize(element, *this);
                }
                else
                {
                    error = from_bytes(element, buffer, remaining);
                }
                if (error != Error::None)
                {
                    return;
                }
                value.push_back(std::move(element));
            }
        }
        else if constexpr (ScalarType<T> || ScalarArrayType<T> || ScalarStdArrayType<T>)
        {
            error = from_bytes(value, buffer, remaining);
        }
        else
        {
            error = deserialize(value, *this);
        }
    }
};

} // namespace borsh

#endif
