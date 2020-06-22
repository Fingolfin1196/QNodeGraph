#pragma once
#include <QtCore/QJsonObject>
#include <QtGui/QTransform>

namespace json_convert {
template<typename T> struct Converter {};
template<> struct Converter<QTransform> {
  static inline QJsonValue to(const QTransform& transform) {
    return QJsonObject{{"m11", transform.m11()}, {"m12", transform.m12()}, {"m13", transform.m13()},
                       {"m21", transform.m21()}, {"m22", transform.m22()}, {"m23", transform.m23()},
                       {"m31", transform.m31()}, {"m32", transform.m32()}, {"m33", transform.m33()}};
  }
  static inline QTransform from(QJsonValue value) {
    auto json{value.toObject()};
    return QTransform(json.value("m11").toDouble(), json.value("m12").toDouble(), json.value("m13").toDouble(),
                      json.value("m21").toDouble(), json.value("m22").toDouble(), json.value("m23").toDouble(),
                      json.value("m31").toDouble(), json.value("m32").toDouble(), json.value("m33").toDouble());
  }
};
template<> struct Converter<QRectF> {
  static inline QJsonValue to(const QRectF& rect) {
    return QJsonObject{{"x", rect.x()}, {"y", rect.y()}, {"width", rect.width()}, {"height", rect.height()}};
  }
  static inline QRectF from(QJsonValue value) {
    auto json{value.toObject()};
    return {json.value("x").toDouble(), json.value("y").toDouble(), json.value("width").toDouble(),
            json.value("height").toDouble()};
  }
};
template<> struct Converter<QPointF> {
  static inline QJsonValue to(const QPointF& point) { return QJsonObject{{"x", point.x()}, {"y", point.y()}}; }
  static inline QPointF from(QJsonValue value) {
    auto json{value.toObject()};
    return {json.value("x").toDouble(), json.value("y").toDouble()};
  }
};
} // namespace json_convert

template<typename T> static inline QJsonValue convertToJson(const T& value) {
  return json_convert::Converter<T>::to(value);
}
template<typename T> static inline T convertFromJson(const QJsonValue& json) {
  return json_convert::Converter<T>::from(json);
}
