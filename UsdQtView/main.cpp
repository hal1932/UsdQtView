#include "stdafx.h"
#include "UsdQtView.h"
#include <QtWidgets/QApplication>
#include <QtGui/QSurfaceFormat>
#include "gl.h"
#include "NodePreview.h"

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
    MainWindow() : QMainWindow(nullptr), nodePreview_(this){}

    void setupUi() {
        connect(&nodePreview_, &NodePreview::loaded, this, [this]() {
            setupOpenGL();

            auto stage = UsdStage::Open(QFileInfo("Kitchen_set/Kitchen_set.usd").absoluteFilePath().toStdString());
            node_.load(stage->GetPrimAtPath(SdfPath("/Kitchen_set/Props_grp/DiningTable_grp/ChairB_1")));

            nodePreview_.setNode(&node_);
        });

        setCentralWidget(&nodePreview_);
    }

    void keyReleaseEvent(QKeyEvent* ev) override {
        switch (ev->key()) {
            case Qt::Key_Escape:
                close();
                return;
        }
    }

private:
    NodePreview nodePreview_;
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
