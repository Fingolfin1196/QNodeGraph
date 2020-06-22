#pragma once
#include <QtCore/QString>

namespace {
template<typename T> struct QStringConverter {};

template<> struct QStringConverter<double> {
  static inline double convert(const QString& source, bool& ok) { return source.toDouble(&ok); }
};
template<> struct QStringConverter<int> {
  static inline int convert(const QString& source, bool& ok, int base = 0) { return source.toInt(&ok, base); }
};
template<> struct QStringConverter<unsigned> {
  static inline unsigned convert(const QString& source, bool& ok, int base = 0) { return source.toUInt(&ok, base); }
};

template<> struct QStringConverter<unsigned long> {
  static inline unsigned long convert(const QString& source, bool& ok, int base = 0) {
    return source.toULong(&ok, base);
  }
};
} // namespace

template<typename T, typename... Args>
static inline std::optional<T> q_string_convert(const QString& source, Args&&... args) {
  bool ok{false};
  T value{QStringConverter<T>::convert(source, ok, std::forward<Args>(args)...)};
  return ok ? std::make_optional(value) : std::nullopt;
}
