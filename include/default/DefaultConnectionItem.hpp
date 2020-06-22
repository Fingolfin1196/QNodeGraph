#pragma once
#include "../TypedConnectionItem.hpp"

template<typename StartPort, typename EndPort, typename Derived> class DefaultConnectionItem
    : public TypedConnectionItem<StartPort, EndPort, Derived> {
  using parent_t = TypedConnectionItem<StartPort, EndPort, Derived>;

  QPainterPath path() const {
    QPainterPath path{};
    path.moveTo(this->startPoint());
    path.lineTo(this->endPoint());
    return path;
  }

  static inline QPainterPath stroked(const QPainterPath& path, const qreal width) {
    QPainterPathStroker stroker{};
    stroker.setWidth(width);
    stroker.setCapStyle(Qt::RoundCap);
    return stroker.createStroke(path);
  }

protected:
  qreal connectionWidth() const { return 8; }

  QBrush brush() const { return {QPalette().color(QPalette::ColorRole::Base)}; }
  QPen pen() const { return {QPalette().color(QPalette::ColorRole::WindowText)}; }
  QPen selectedPen() const { return {QPalette().color(QPalette::ColorRole::Highlight)}; }

  QPen currentPen() const { return this->isSelected() ? selectedPen() : pen(); }

  QPainterPath shape() const final { return stroked(path(), currentPen().widthF() + connectionWidth()); }

  QRectF boundingRect() const final { return shape().boundingRect(); }

  void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget* = nullptr) final {
    painter->setPen(currentPen());
    painter->setBrush(brush());
    painter->drawPath(stroked(path(), connectionWidth()));
  }

  DefaultConnectionItem(StartPort* start, EndPort* end, QPointF temp_end, NodeGraphScene* scene,
                        QGraphicsItem* parent = nullptr)
      : parent_t{start, end, temp_end, scene, parent} {}

public:
  QJsonObject toJson() const override { return {{"parent", parent_t::toJson()}}; }

  DefaultConnectionItem(StartPort* start, QPointF temp_end, NodeGraphScene* scene, QGraphicsItem* parent = nullptr)
      : parent_t{start, temp_end, scene, parent} {}
  DefaultConnectionItem(QJsonObject json, const PortStorage& ports, NodeGraphScene* scene,
                        QGraphicsItem* parent = nullptr)
      : parent_t{json.value("parent").toObject(), ports, scene, parent} {}
};
