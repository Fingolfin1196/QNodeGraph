#include "PortItem.hpp"
#include "NodeGraphView.hpp"
#include "NodeItemGroup.hpp"

NodeGraphView* __detail::view(NodeItemGroup* node) { return node->view(); }
void PortItem<PortType::OUTPUT>::mousePressEvent(QGraphicsSceneMouseEvent* event) {
  view()->addConnection(this->createConnection(event));
}
