#pragma once
#include "ConnectionItem.hpp"
#include "NodeGraphScene.hpp"
#include <QtCore/QJsonObject>
#include <QtWidgets/QAction>
#include <QtWidgets/QGraphicsProxyWidget>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMenu>
#include <set>

class ConnectionItem;
class NodeItemGroup;
class NodeGraphView : public QGraphicsView {
  Q_OBJECT
  using node_gen_t = std::function<NodeItemGroup*()>;
  using node_regen_t = NodeGraphScene::node_creator_t;
  using connection_regen_t = NodeGraphScene::connection_creator_t;

  struct NodeMenuInfo {
    QString type_name;
    mutable node_gen_t generator;

    QAction* toAction(NodeGraphScene* scene, QPointF scene_pos) const;

    NodeMenuInfo(QString type_name, node_gen_t generator)
        : type_name{std::move(type_name)}, generator{std::move(generator)} {}

    bool operator<(const NodeMenuInfo& other) const { return type_name.localeAwareCompare(other.type_name) < 0; }
    bool operator==(const NodeMenuInfo& other) const { return type_name == other.type_name; }
  };
  struct NodeMenuInfoTree {
    QString name;
    mutable std::set<NodeMenuInfo> infos;
    mutable std::set<NodeMenuInfoTree> children;

    QMenu* toMenu(NodeGraphScene* scene, QPointF scene_pos) const;

    NodeMenuInfoTree(QString name, std::set<NodeMenuInfo> infos, std::set<NodeMenuInfoTree> children)
        : name{std::move(name)}, infos{std::move(infos)}, children{std::move(children)} {}
    NodeMenuInfoTree(QString name) : NodeMenuInfoTree(name, {}, {}) {}

    bool operator<(const NodeMenuInfoTree& other) const { return name.localeAwareCompare(other.name) < 0; }
    bool operator==(const NodeMenuInfoTree& other) const { return name == other.name; }
  };

  NodeGraphScene* scene_;
  QAction *clear_action_, *remove_action_;
  QPointF last_click_;
  const qreal max_scale_{5}, scaling_factor_{1.2}, inv_scaling_factor_{1. / scaling_factor_};
  ConnectionItem* current_connection_{nullptr};
  NodeMenuInfoTree node_info_tree_{"Add Node"};

  static QAction* createClearAction(NodeGraphView* view);

  static QAction* createRemoveAction(NodeGraphView* view);

protected:
  void contextMenuEvent(QContextMenuEvent* event) override;

  void wheelEvent(QWheelEvent* event) override;

  void showEvent(QShowEvent* event) override;

  void keyPressEvent(QKeyEvent* event) override {
    if (event->key() == Qt::Key_Shift) setDragMode(QGraphicsView::RubberBandDrag);
    QGraphicsView::keyPressEvent(event);
  }

  void keyReleaseEvent(QKeyEvent* event) override {
    if (event->key() == Qt::Key_Shift) setDragMode(QGraphicsView::ScrollHandDrag);
    QGraphicsView::keyReleaseEvent(event);
  }

  void mousePressEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;

  void addNodeType(const QString& type_id, const QString& type_name, QStringList categories, node_gen_t generator,
                   node_regen_t regenerator);
  void addConnectionType(const QString& type_id, connection_regen_t regenerator) {
    scene_->registerConnectionType(type_id, regenerator);
  }

public:
  void addConnection(ConnectionItem* connection);

  NodeGraphScene* nodeGraphScene() const { return scene_; }

  QJsonDocument toJson() const;
  void fromJson(QJsonDocument document);

  explicit NodeGraphView(NodeGraphScene* scene, QWidget* parent = nullptr)
      : QGraphicsView(scene, parent), scene_{scene}, clear_action_{createClearAction(this)},
        remove_action_{createRemoveAction(this)} {
    setDragMode(QGraphicsView::ScrollHandDrag);
    setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);

    // setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  }
};