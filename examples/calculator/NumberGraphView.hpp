#pragma once

#include "NodeGraphView.hpp"
#include "NumberInputNode.hpp"
#include "NumberOutputNode.hpp"
#include <QtCore/QJsonDocument>

class NumberGraphView : public NodeGraphView {
  Q_OBJECT

  template<typename T> void addTypedNode(QString type_name, QStringList categories) {
    addNodeType(
        T::typeID(), type_name, categories, [this] { return new T(this); },
        [this](QJsonObject object, PortRegistration& registration) { return new T(object, registration, this); });
  }

  template<typename T> void addTypedConnection() {
    addConnectionType(T::typeID(), [this](QJsonObject object, const PortStorage& storage) {
      std::string string{QJsonDocument{object}.toJson().toStdString()};
      return new T(object, storage, this->nodeGraphScene());
    });
  }

public:
  template<typename... Args> NumberGraphView(Args&&... args) : NodeGraphView(std::forward<Args>(args)...) {
    addTypedNode<NumberInputNode<double>>("Floating Point Input", QStringList() << "Input");
    addTypedNode<NumberInputNode<int>>("Signed Integer Input", QStringList() << "Input");
    addTypedNode<NumberInputNode<unsigned>>("Unsigned Integer Input", QStringList() << "Input");

    addTypedNode<NumberOutputNode<double>>("Floating Point Output", QStringList() << "Output");
    addTypedNode<NumberOutputNode<int>>("Signed Integer Output", QStringList() << "Output");
    addTypedNode<NumberOutputNode<unsigned>>("Unsigned Integer Output", QStringList() << "Output");

    addTypedConnection<NumberConnection<double>>();
    addTypedConnection<NumberConnection<int>>();
    addTypedConnection<NumberConnection<unsigned>>();
  }
};
