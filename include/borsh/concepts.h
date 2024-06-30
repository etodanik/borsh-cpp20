#pragma once
#ifndef BORSH_CPP20_CONCEPTS_H
#define BORSH_CPP20_CONCEPTS_H

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

#include "int128.h"

namespace borsh
{

class Serializer;

template <typename T>
#if (defined(__GNUC__) && !defined(__clang__) && !defined(__INTEL_COMPILER))
concept IntegralType = std::is_integral_v<T> || std::ranges::__detail::__is_int128<T>;
#else
concept IntegralType = std::is_integral_v<T>;
#endif

template <typename T>
#ifdef BORSH_HAVE_INTRINSIC_INT128
concept FloatType = std::is_floating_point_v<T>;
#else
concept FloatType = std::is_same_v<T, float> || std::is_same_v<T, double>;
#endif

template <typename T>
concept NumericType = IntegralType<T> || FloatType<T>;

template <typename T>
concept StringType = std::is_same_v<T, std::string>;

template <typename T, typename = void> struct IsScalar : std::false_type
{
};

template <typename T>
concept ScalarType = IsScalar<T>::value || NumericType<T> || StringType<T>;

template <typename T>
concept is_bounded_array_v = std::rank_v<T> == 1 && std::extent_v<T> != 0;

template <typename T> using remove_extent_and_cv_t = std::remove_extent_t<std::remove_cv_t<T>>;

template <typename T, typename C>
concept is_same_remove_extent_v = std::is_same_v<remove_extent_and_cv_t<T>, C>;

template <typename T>
concept CharArrayType = is_bounded_array_v<T> && (is_same_remove_extent_v<T, char> || is_same_remove_extent_v<T, unsigned char>);

template <typename T>
concept IntegralArrayType = is_bounded_array_v<T> && IntegralType<remove_extent_and_cv_t<T>>;

template <typename T>
concept FloatArrayType = is_bounded_array_v<T> && FloatType<remove_extent_and_cv_t<T>>;

template <typename T>
concept NumericArrayType = IntegralArrayType<T> || FloatArrayType<T>;

template <typename T>
concept ScalarArrayType = is_bounded_array_v<T> && ScalarType<remove_extent_and_cv_t<T>>;

template <typename T>
concept NonScalarArrayType = is_bounded_array_v<T> && !ScalarType<remove_extent_and_cv_t<T>>;

template <typename T>
concept ArrayType = ScalarArrayType<T> || NonScalarArrayType<T>;

template <typename T>
concept SerializableElement = requires(T t, Serializer& s) { serialize(t, s); };

template <typename T>
concept SerializableArray =
    requires(T (&array)[], Serializer& s) { serialize(array, s); } && SerializableElement<remove_extent_and_cv_t<T>>;

template <typename T>
concept SerializableVector = requires(T t) {
    requires std::same_as<std::remove_cv_t<T>, std::vector<typename T::value_type>>;
    requires SerializableElement<std::remove_cv_t<typename T::value_type>> || SerializableArray<std::remove_cv_t<typename T::value_type>>;
};

template <typename T>
concept SerializableVectorVector = requires(T t) {
    requires std::same_as<std::remove_cv_t<T>, std::vector<typename T::value_type>>;
    requires SerializableVector<std::remove_cv_t<typename T::value_type>>;
};

template <typename T>
concept Serializable = SerializableElement<T> || SerializableArray<T> || SerializableVector<T> || SerializableVectorVector<T>;

template <typename T>
concept SerializableNonScalar = SerializableElement<T> && !ScalarType<T>;

template <typename T>
concept SerializableNonScalarArray = SerializableArray<T> && !ScalarType<remove_extent_and_cv_t<T>>;

template <typename T>
concept SerializableScalar = SerializableElement<T> && ScalarType<T>;

template <typename T>
concept Swappable = IntegralType<T> && std::has_unique_object_representations_v<T>;

} // namespace borsh

#endif