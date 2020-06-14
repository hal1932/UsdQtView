#include "stdafx.h"
#include "NodeList.h"
#include "SceneNode.h"


NodeList::NodeList(QWidget* parent)
    : QWidget(parent), nodeList_(this), variantSetList_(this)
{ }

void NodeList::setupUi() {
    nodeList_.setupUi();
    variantSetList_.setupUi();

    layout_.setContentsMargins(0, 0, 0, 0);
    layout_.addWidget(&nodeList_);
    layout_.addWidget(&variantSetList_);
    setLayout(&layout_);

    connect(&nodeList_, &NodeListWidget::itemSelectionChanged, this, [this]() {
        const QString path = nodeList_.currentItem()->text();
        const SceneNode* pNode = nodes_[path];
        const auto& prim = pNode->prim();

        variantSetList_.set(prim);
    });
}

void NodeList::add(const SceneNode* pNode) {
    const QString path(pNode->prim().GetPath().GetString().c_str());
    nodeList_.addItem(path);
    nodes_[path] = pNode;
}
