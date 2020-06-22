#pragma once
#include "Typing.hpp"
#include <QtCore/QJsonObject>
#include <QtWidgets/QLineEdit>

template<typename T> class TypedLineEdit : public QLineEdit {
  void setup() {
    connect(this, &QLineEdit::returnPressed, [this] { updateValue(); });
  }

protected:
  virtual void validValue(T) = 0;
  virtual void invalidValue() = 0;

  void updateValue() {
    std::optional<T> optional{q_string_convert<T>(text())};
    if (optional.has_value())
      validValue(optional.value());
    else
      invalidValue();
  }

  void focusOutEvent(QFocusEvent* event) override {
    updateValue();
    return QLineEdit::focusOutEvent(event);
  }

public:
  virtual QJsonObject toJson() const { return {{"text", this->text()}}; }

  explicit TypedLineEdit(QWidget* parent = nullptr) : QLineEdit{parent} { setup(); }
  TypedLineEdit(const QString& string, QWidget* parent = nullptr) : QLineEdit{string, parent} { setup(); }
  TypedLineEdit(QJsonObject json, QWidget* parent = nullptr) : TypedLineEdit{json.value("text").toString(), parent} {}
};
