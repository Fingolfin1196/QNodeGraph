#pragma once
#include "../BackgroundItem.hpp"
#include "../PortType.hpp"
#include "../helpers/Interval.hpp"
#include <KColorScheme>
#include <QtGui/QBrush>
#include <QtGui/QPainter>
#include <QtGui/QPalette>
#include <QtGui/QPen>

template<typename Node> class DefaultBackgroundItem : public BackgroundItem {
  constexpr static qreal half_{.5}, one_{1}, two_{2};

  Node* node_;

  std::size_t inputNumber() const { return node_->inputNumber(); }
  std::size_t outputNumber() const { return node_->outputNumber(); }

  Interval<qreal> inputYInterval() const {
    return Interval<qreal>::fromCentredSize(node_->widgetRect().center().y(),
                                            inputNumber() * (two_ * inPortRadius() + inPortDistance()));
  }
  Interval<qreal> outputYInterval() const {
    return Interval<qreal>::fromCentredSize(node_->widgetRect().center().y(),
                                            outputNumber() * (two_ * outPortRadius() + outPortDistance()));
  }

protected:
  virtual qreal xPadding() const { return 12; }
  virtual qreal yPadding() const { return 6; }
  virtual qreal roundingRadius() const { return 6; }
  virtual qreal inPortRadius() const { return 6; }
  virtual qreal inPortDistance() const { return 8; }
  virtual qreal outPortRadius() const { return 6; }
  virtual qreal outPortDistance() const { return 8; }

  virtual QPen defaultPen() const { return {QPalette().color(QPalette::ColorRole::WindowText)}; }
  virtual QPen selectedPen() const { return {QPalette().color(QPalette::ColorRole::Highlight)}; }
  virtual QPen errorPen() const { return {KColorScheme().foreground(KColorScheme::NegativeText).color()}; }

  virtual QBrush defaultBrush() const { return {QPalette().color(QPalette::ColorRole::Window)}; }
  virtual QBrush selectedBrush() const { return {QPalette().color(QPalette::ColorRole::Window)}; }
  virtual QBrush errorBrush() const { return KColorScheme().background(KColorScheme::NegativeBackground); }

  QPen currentPen() const { return isSelected() ? selectedPen() : defaultPen(); }
  QBrush currentBrush() const { return isSelected() ? selectedBrush() : defaultBrush(); }

  QRectF rectangle() const {
    const QRectF rect{node_->widgetRect()};
    Interval<qreal> y_interval{rect.y(), rect.bottom()};
    y_interval.unify(inputYInterval()).unify(outputYInterval());
    const auto x_padding{xPadding()}, y_padding{yPadding()};
    return QRectF{rect.x(), y_interval.min(), rect.width(), y_interval.difference()}.adjusted(-x_padding, -y_padding,
                                                                                              x_padding, y_padding);
  }

  void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget* = nullptr) final {
    prepareGeometryChange();
    painter->setPen(currentPen());
    painter->setBrush(currentBrush());
    const auto rounding_radius{roundingRadius()};
    painter->drawRoundedRect(rectangle(), rounding_radius, rounding_radius);
  }

public:
  QPainterPath shape() const final {
    QPainterPath path{};
    const auto rounding_radius{roundingRadius()};
    path.addRoundedRect(rectangle(), rounding_radius, rounding_radius);
    return QPainterPathStroker(currentPen()).createStroke(path);
  }
  QRectF boundingRect() const final {
    const qreal pen_radius{currentPen().widthF() / two_};
    return rectangle().adjusted(-pen_radius, -pen_radius, pen_radius, pen_radius);
  }

  template<PortType Dir> qreal portRadius(const std::size_t) const {
    static_assert(Dir == PortType::INPUT or Dir == PortType::OUTPUT);
    if constexpr (Dir == PortType::INPUT)
      return inPortRadius();
    else
      return outPortRadius();
  }

  QPointF inPortCentre(const std::size_t index) const {
    const QRectF rect{node_->widgetRect()};
    const auto centre{rect.center().y() +
                      (index + half_ * (one_ - inputNumber())) * (two_ * inPortRadius() + inPortDistance())};
    return {rect.x() - xPadding(), centre};
  }
  QPointF outPortCentre(const std::size_t index) const {
    const QRectF rect{node_->widgetRect()};
    const auto centre{rect.center().y() +
                      (index + half_ * (one_ - outputNumber())) * (two_ * outPortRadius() + outPortDistance())};
    return {rect.right() + xPadding(), centre};
  }
  template<PortType Dir> QPointF portCentre(const std::size_t index) const {
    static_assert(Dir == PortType::INPUT or Dir == PortType::OUTPUT);
    if constexpr (Dir == PortType::INPUT)
      return inPortCentre(index);
    else
      return outPortCentre(index);
  }

  QJsonObject toJson() const override { return {{"parent", BackgroundItem::toJson()}}; }

  DefaultBackgroundItem(NodeState state, Node* node, QGraphicsItem* parent = nullptr)
      : BackgroundItem{state, parent}, node_{node} {}
  DefaultBackgroundItem(QJsonObject json, Node* node, QGraphicsItem* parent = nullptr)
      : BackgroundItem{json.value("parent").toObject(), parent}, node_{node} {}
};
