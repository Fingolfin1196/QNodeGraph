#pragma once
#include "../NodeGraphView.hpp"
#include "../PortItem.hpp"
#include "../TypedPortItem.hpp"
#include "DefaultConnectionItem.hpp"
#include <QtGui/QPainter>
#include <QtGui/QPalette>
#include <QtWidgets/QGraphicsObject>

template<PortType Dir, typename Data, typename Connection> class DefaultPortItem
    : public TypedPortItem<Dir, Data, Connection> {
protected:
  virtual qreal radius() const = 0;

  virtual QBrush defaultBrush() const { return {QPalette().color(QPalette::ColorRole::Window)}; }
  virtual QBrush selectedBrush() const { return {QPalette().color(QPalette::ColorRole::Window)}; }
  virtual QPen defaultPen() const { return {QPalette().color(QPalette::ColorRole::WindowText)}; }
  virtual QPen selectedPen() const { return {QPalette().color(QPalette::ColorRole::Highlight)}; }

  QBrush currentBrush() const { return this->isSelected() ? selectedBrush() : defaultBrush(); }
  QPen currentPen() const { return this->isSelected() ? selectedPen() : defaultPen(); }

  void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget* = nullptr) override {
    painter->setPen(currentPen());
    painter->setBrush(currentBrush());
    const qreal current_radius{radius()};
    painter->drawEllipse(this->localCentre(), current_radius, current_radius);
  }

  DefaultPortItem(Data data, QGraphicsItem* parent = nullptr)
      : TypedPortItem<Dir, Data, Connection>(std::move(data), parent) {}

public:
  QPainterPath shape() const override {
    QPainterPath path{};
    const qreal penned_radius{radius() + currentPen().widthF() / 2.};
    path.addEllipse(this->localCentre(), penned_radius, penned_radius);
    return path;
  }
  QRectF boundingRect() const override { return shape().boundingRect(); }

  QJsonObject toJson() const override { return {{"parent", TypedPortItem<Dir, Data, Connection>::toJson()}}; }

  DefaultPortItem(QGraphicsItem* parent = nullptr) : TypedPortItem<Dir, Data, Connection>(parent) {}
  DefaultPortItem(QJsonObject json, QGraphicsItem* parent = nullptr)
      : TypedPortItem<Dir, Data, Connection>(json, parent) {}
};
