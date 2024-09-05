#pragma once
#ifndef BORSH_CPP20_SERIALIZER_H
#define BORSH_CPP20_SERIALIZER_H

namespace borsh
{

enum SerializerDirection
{
    Serialize = 0,
    Deserialize = 1,
};

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
    const SerializerDirection direction;
    std::vector<uint8_t>&     buffer;
    const uint8_t*&           bufferPointerReference;

    /**
     * This handles the execution path for the compiler where a const variable was passed to serialize().
     * Normally, the compiler would not have access to `direction` at compile-time, therefore trying to
     * explore the deserialization code path and returning a variety of errors. This helps prevent that
     * and offloads the undesirable situation to an exception.
     * @tparam T
     * @param value
     */
    template <typename T> void visit(const T& value)
    {
        if (direction == SerializerDirection::Serialize)
        {
            if constexpr (SerializableVector<T>)
            {
                append(buffer, static_cast<int32_t>(value.size()));

                for (auto item : value)
                {
                    if constexpr (SerializableNonScalar<typename T::value_type>)
                    {
                        serialize(item, *this);
                    }
                    else
                    {
                        to_bytes(item, buffer);
                    }
                }
            }
            else if constexpr (ScalarType<T> || ScalarArrayType<T> || ScalarStdArrayType<T>)
            {
                to_bytes(value, buffer);
            }
            else
            {
                serialize(value, *this);
            }
        }
        else [[unlikely]]
        {
            throw std::runtime_error("Cannot deserialize into a const object");
        }
    }

    template <typename T> void visit(T& value)
    {
        if (direction == SerializerDirection::Serialize)
        {
            if constexpr (SerializableVector<T>)
            {
                append(buffer, static_cast<int32_t>(value.size()));

                for (auto item : value)
                {
                    if constexpr (SerializableNonScalar<typename T::value_type>)
                    {
                        serialize(item, *this);
                    }
                    else
                    {
                        to_bytes(item, buffer);
                    }
                }
            }
            else if constexpr (ScalarType<T> || ScalarArrayType<T> || ScalarStdArrayType<T>)
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
            if constexpr (SerializableVector<T>)
            {
                const int32_t length = *reinterpret_cast<const int32_t*>(bufferPointerReference);
                bufferPointerReference += sizeof(int32_t);

                value.clear();
                for (int32_t i = 0; i < length; ++i)
                {
                    typename T::value_type element;
                    if constexpr (SerializableNonScalar<typename T::value_type>)
                    {
                        serialize(element, *this);
                    }
                    else
                    {
                        from_bytes(element, bufferPointerReference);
                    }
                    value.push_back(element);
                }
            }
            else if constexpr (ScalarType<T> || ScalarArrayType<T> || ScalarStdArrayType<T>)
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

} // namespace borsh

#endif