#pragma once
#include "NodeState.hpp"
#include "helpers/Typing.hpp"
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtWidgets/QGraphicsProxyWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <optional>

class NodeProxy : public QGraphicsProxyWidget {
  Q_OBJECT

  NodeState state_;
  QWidget* central_{nullptr};
  QLabel *title_, *status_;
  QVBoxLayout* layout_;

  QLabel* makeTitle() const {
    auto* label{new QLabel};
    label->setAlignment(Qt::AlignCenter);
    return label;
  }
  QLabel* makeStatus() const {
    auto* label{new QLabel};
    label->setAlignment(Qt::AlignCenter);
    return label;
  }

  QRectF rectangle() const {
    auto rect{boundingRect()};
    rect.moveTo(pos());
    return rect;
  }

  QVBoxLayout* makeLayout() const;

  QWidget* makeWrapper() const {
    auto* container{new QWidget};
    container->setLayout(layout_);
    return container;
  }
  void setStatus(const QString& status) { status_->setText(status); }
  void updateState(std::optional<QString> message = std::nullopt);

public:
  void setCentralWidget(QWidget* widget) {
    central_ = widget;
    layout_ = makeLayout();
    setWidget(makeWrapper());
  }

  void setTitle(const QString& title) { title_->setText(title); }

  virtual void setState(NodeState state, std::optional<QString> message = std::nullopt) {
    state_ = state;
    updateState(std::move(message));
  }

  virtual void preGeometryChange() { prepareGeometryChange(); }

  virtual QJsonObject toJson() const {
    return {{"title", title_->text()}, {"state", int{to_underlying(state_)}}, {"status", status_->text()}};
  }

  NodeProxy(NodeState initial_state, QGraphicsItem* parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags())
      : QGraphicsProxyWidget(parent, flags), state_{initial_state}, title_{makeTitle()}, status_{makeStatus()},
        layout_{makeLayout()} {
    updateState();
    setWidget(makeWrapper());
  }
  NodeProxy(QJsonObject json, QGraphicsItem* parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags())
      : QGraphicsProxyWidget(parent, flags), state_{from_underlying<NodeState>(json.value("state").toInt())},
        title_{makeTitle()}, status_{makeStatus()}, layout_{makeLayout()} {
    setTitle(json.value("title").toString());
    setStatus(json.value("status").toString());
    updateState();
    setWidget(makeWrapper());
  }
};
