#pragma once
#include "ConnectionItem.hpp"
#include "LoadRegistry.hpp"
#include "helpers/Typing.hpp"
#include <sstream>

template<typename StartPort, typename EndPort, typename Derived> class TypedConnectionItem : public ConnectionItem {
  StartPort* start_;
  EndPort* end_;

protected:
  template<typename... Args> TypedConnectionItem(StartPort* start, EndPort* end, Args&&... args)
      : ConnectionItem{std::forward<Args>(args)...}, start_{start}, end_{end} {}

public:
  void setStartItem(PortItem<PortType::OUTPUT>* start) override {
    assert(dynamic_cast<StartPort*>(start));
    auto* cast_ptr{static_cast<StartPort*>(start)};
    if (cast_ptr != start_) {
      if (start_) start_->removeConnection(this);
      start_ = cast_ptr;
      if (start_) start_->addConnection(this);
    }
  }
  void setEndItem(PortItem<PortType::INPUT>* end) override {
    assert(dynamic_cast<EndPort*>(end));
    auto* cast_ptr{static_cast<EndPort*>(end)};
    if (cast_ptr != end_) {
      if (end_) end_->removeConnection(this);
      end_ = cast_ptr;
      if (end_) end_->addConnection(this);
    }
  }
  void removeEndItem() override {
    if (end_) end_->removeConnection(this);
    end_ = nullptr;
  }

  void preparePropagation() {
    assert(start_ and end_);
    end_->preparePropaganda();
  }
  void propagate() {
    assert(start_ and end_);
    end_->acceptPropaganda(start_->value());
  }

  StartPort* typedStartItem() const { return start_; }
  EndPort* typedEndItem() const { return end_; }
  PortItem<PortType::OUTPUT>* startItem() const override { return typedStartItem(); }
  PortItem<PortType::INPUT>* endItem() const override { return typedEndItem(); }

  QJsonObject toJson() const override {
    return {{"parent", ConnectionItem::toJson()}, {"start", ptrToStr(start_)}, {"end", ptrToStr(end_)}};
  }
  QString typeIdentification() const final { return Derived::typeID(); }

  TypedConnectionItem(StartPort* start, QPointF temp_end, NodeGraphScene* scene, QGraphicsItem* parent = nullptr)
      : TypedConnectionItem{start, nullptr, temp_end, scene, parent} {}
  TypedConnectionItem(QJsonObject json, const PortStorage& ports, NodeGraphScene* scene,
                      QGraphicsItem* parent = nullptr)
      : TypedConnectionItem{dynamic_cast<StartPort*>(ports.retrieveOutput(json.value("start").toString())),
                            dynamic_cast<EndPort*>(ports.retrieveInput(json.value("end").toString())),
                            json.value("parent").toObject(), scene, parent} {
    if (start_) start_->addTypedConnection(static_cast<Derived*>(this));
    if (end_) end_->addTypedConnection(static_cast<Derived*>(this));
  }
};
