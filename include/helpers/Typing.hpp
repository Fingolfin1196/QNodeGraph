#pragma once
#include <QtCore/QString>

template<typename T> QString ptrToStr(T* ptr) { return QString::number(reinterpret_cast<std::uintptr_t>(ptr)); }

template<typename T> constexpr static inline std::underlying_type_t<T> to_underlying(const T value) {
  return static_cast<std::underlying_type_t<T>>(value);
}
template<typename T> constexpr static inline T from_underlying(const std::underlying_type_t<T> value) {
  return static_cast<T>(value);
}

static_assert(sizeof(float) == 4);
static_assert(sizeof(double) == 8);
using float32_t = float;
using float64_t = double;

namespace {
template<typename T> struct TypeStringer {
  // static inline QString string() { return T::typeID(); }
};
template<> struct TypeStringer<uint8_t> {
  static inline QString string() { return QStringLiteral("uint8"); }
};
template<> struct TypeStringer<uint16_t> {
  static inline QString string() { return QStringLiteral("uint16"); }
};
template<> struct TypeStringer<uint32_t> {
  static inline QString string() { return QStringLiteral("uint32"); }
};
template<> struct TypeStringer<int8_t> {
  static inline QString string() { return QStringLiteral("int8"); }
};
template<> struct TypeStringer<int16_t> {
  static inline QString string() { return QStringLiteral("int16"); }
};
template<> struct TypeStringer<int32_t> {
  static inline QString string() { return QStringLiteral("int32"); }
};
template<> struct TypeStringer<float32_t> {
  static inline QString string() { return QStringLiteral("float32"); }
};
template<> struct TypeStringer<float64_t> {
  static inline QString string() { return QStringLiteral("float64"); }
};

template<typename T> concept TypeIdentified = requires() {
  { T::typeID() }
  ->std::same_as<QString>;
};
template<TypeIdentified T> struct TypeStringer<T> {
  static inline QString string() { return T::typeID(); }
};
} // namespace

template<typename T> static inline QString typeString() { return TypeStringer<T>::string(); }
template<typename T> static inline QString readableString() { return TypeStringer<T>::string(); }
