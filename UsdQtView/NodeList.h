#pragma once
#include "NodeListComponents.h"

class SceneNode;

class NodeList : public QWidget {
    Q_OBJECT
public:
    NodeList(QWidget* parent = nullptr);
    void setupUi();
    void add(const SceneNode* pNode);

private:
    QVBoxLayout layout_;
    NodeListWidget nodeList_;
    VariantSetListWidget variantSetList_;

    std::map<QString, const SceneNode*> nodes_;
};

