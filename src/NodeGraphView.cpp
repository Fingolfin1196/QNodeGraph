#include "NodeGraphView.hpp"
#include "ConnectionItem.hpp"
#include "NodeItemGroup.hpp"
#include "helpers/Conversions.hpp"
#include <QtCore/QJsonDocument>

QAction* NodeGraphView::NodeMenuInfo::toAction(NodeGraphScene* scene, QPointF scene_pos) const {
  auto* action{new QAction(type_name)};
  action->connect(action, &QAction::triggered, [this, scene, scene_pos] {
    auto* item{generator()};
    item->setPos(scene_pos.x(), scene_pos.y());
    scene->addItem(item);
  });
  return action;
}

QMenu* NodeGraphView::NodeMenuInfoTree::toMenu(NodeGraphScene* scene, QPointF scene_pos) const {
  auto* menu{new QMenu(name)};
  for (const NodeMenuInfoTree& child : children) menu->addMenu(child.toMenu(scene, scene_pos));
  for (const NodeMenuInfo& info : infos) menu->addAction(info.toAction(scene, scene_pos));
  return menu;
}

QAction* NodeGraphView::createClearAction(NodeGraphView* view) {
  auto* clear_action{new QAction("Clear Selection", view)};
  clear_action->setShortcut(Qt::Key_Escape);
  connect(clear_action, &QAction::triggered, view->nodeGraphScene(), &NodeGraphScene::clearSelection);
  view->addAction(clear_action);
  return clear_action;
}

QAction* NodeGraphView::createRemoveAction(NodeGraphView* view) {
  auto* delete_action = new QAction("Delete Selection", view);
  delete_action->setShortcut(Qt::Key_Delete);
  connect(delete_action, &QAction::triggered, view->nodeGraphScene(), &NodeGraphScene::deleteSelection);
  view->addAction(delete_action);
  return delete_action;
}

void NodeGraphView::contextMenuEvent(QContextMenuEvent* event) {
  const auto pos{mapToScene(event->pos())};
  if (not scene_->isTypeHit<NodeItemGroup>(pos)) node_info_tree_.toMenu(scene_, pos)->exec(event->globalPos());
}

void NodeGraphView::wheelEvent(QWheelEvent* event) {
  const int delta{event->angleDelta().y()};
  if (delta == 0)
    event->ignore();
  else if (delta > 0) {
    if (transform().m11() <= max_scale_) scale(scaling_factor_, scaling_factor_);
  } else
    scale(inv_scaling_factor_, inv_scaling_factor_);
}

void NodeGraphView::showEvent(QShowEvent* event) {
  scene_->setSceneRect(rect());
  QGraphicsView::showEvent(event);
}

void NodeGraphView::mousePressEvent(QMouseEvent* event) {
  QGraphicsView::mousePressEvent(event);
  if (event->button() == Qt::LeftButton) last_click_ = mapToScene(event->pos());
}
void NodeGraphView::mouseMoveEvent(QMouseEvent* event) {
  const auto pos{mapToScene(event->pos())};
  if (current_connection_)
    current_connection_->mouseMove(pos);
  else {
    QGraphicsView::mouseMoveEvent(event);
    if (not scene()->mouseGrabberItem() and event->buttons() == Qt::LeftButton and
        not(event->modifiers() & Qt::ShiftModifier))
      setSceneRect(sceneRect().translated(last_click_ - pos));
  }
}
void NodeGraphView::mouseReleaseEvent(QMouseEvent* event) {
  QGraphicsView::mouseReleaseEvent(event);
  if (current_connection_) {
    const bool is_handled{current_connection_->mouseRelease(mapToScene(event->pos()))};
    assert(is_handled);
    current_connection_ = nullptr;
  }
}

void NodeGraphView::addNodeType(const QString& type_id, const QString& type_name, QStringList categories,
                                node_gen_t generator, node_regen_t regenerator) {
  const NodeMenuInfoTree* tree{&node_info_tree_};
  for (const QString& cat : categories) tree = &*tree->children.emplace(cat).first;
  tree->infos.emplace(std::move(type_name), std::move(generator));
  scene_->registerNodeType(type_id, std::move(regenerator));
}

void NodeGraphView::addConnection(ConnectionItem* connection) {
  current_connection_ = connection;
  scene_->addItem(connection);
}

QJsonDocument NodeGraphView::toJson() const {
  return QJsonDocument{QJsonObject{{"scene", scene_->toJson()},
                                   {"transform", convertToJson(transform())},
                                   {"scene_rect", convertToJson(sceneRect())}}};
}

void NodeGraphView::fromJson(QJsonDocument document) {
  QJsonObject json{document.object()};
  QJsonValue scene{json.value("scene")};
  std::cout << "is scene an object: " << scene.isObject() << std::endl;
  auto scene_object{scene.toObject()};
  std::cout << "scene object keys: ";
  for (auto key : scene_object.keys()) std::cout << key.toStdString() << " ";
  std::cout << std::endl;
  scene_->fromJson(scene_object);
  setTransform(convertFromJson<QTransform>(json.value("transform").toObject()));
  setSceneRect(convertFromJson<QRectF>(json.value("scene_rect").toObject()));
}
