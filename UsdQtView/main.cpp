#include "stdafx.h"
#include <QtWidgets/QApplication>
#include <QtGui/QSurfaceFormat>
#include "gl.h"
#include "SceneNode.h"
#include "NodePreview.h"
#include "NodeList.h"

#include <cstdio>
#include <iostream>


class Console final {
public:
    Console() {
#ifdef _WIN32
        AllocConsole();
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
#endif
    }

    ~Console() {
#ifdef _WIN32
        FreeConsole();
#endif
    }
};


class MainWindow : public QMainWindow {
public:
    MainWindow()
        : QMainWindow(nullptr), nodePreview_(this), nodeList_(this)
    { }

    void setupUi() {
        connect(&nodePreview_, &NodePreview::loaded, this, [this]() {
            setupOpenGL();

            stage_ = UsdStage::Open(QFileInfo("Kitchen_set/Kitchen_set.usd").absoluteFilePath().toStdString());
            node_.load(stage_->GetPrimAtPath(SdfPath("/Kitchen_set/Props_grp/DiningTable_grp/ChairB_1")));
            //stage_ = UsdStage::Open(QFileInfo("simpleShading.usda").absoluteFilePath().toStdString());
            //node_.load(stage_->GetPseudoRoot());

            nodePreview_.setNode(&node_);
            nodePreview_.updateRenderable();

            SceneNode::walk(&node_, [this](auto pNode) {
                nodeList_.add(pNode);
                return true;
            });
        });

        nodePreview_.setMinimumSize(640, 480);
        nodeList_.setupUi();

        centralLayout_.addWidget(&nodePreview_);
        centralLayout_.addWidget(&nodeList_);
        centralWidget_.setLayout(&centralLayout_);

        setCentralWidget(&centralWidget_);
    }

    void keyReleaseEvent(QKeyEvent* ev) override {
        switch (ev->key()) {
            case Qt::Key_Escape:
                close();
                return;
        }
    }

private:
    QWidget centralWidget_;
    QHBoxLayout centralLayout_;
    NodePreview nodePreview_;
    NodeList nodeList_;

    UsdStageRefPtr stage_;
    SceneNode node_;
};


int main(int argc, char *argv[]) {
    Console _;

    QApplication app(argc, argv);

    MainWindow window;
    window.setupUi();
    window.show();

    return app.exec();
}
