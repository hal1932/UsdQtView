#include "stdafx.h"
#include "UsdQtView.h"
#include <QtWidgets/QApplication>
#include <QtGui/QSurfaceFormat>
#include "gl.h"
#include "GLWidget.h"

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
    void keyReleaseEvent(QKeyEvent* ev) {
        switch (ev->key()) {
            case Qt::Key_Escape:
                close();

                return;
        }
    }
};


int main(int argc, char *argv[]) {
    Console _;

    QApplication app(argc, argv);

    MainWindow window;
    GLWidget w;
    window.setCentralWidget(&w);
    window.show();

    //Stage stage;
    //stage.LoadFromFile(QFileInfo("Kitchen_set/Kitchen_set.usd").absoluteFilePath().toStdString());
    //for (auto path : stage.TraverseGPrims()) {
    //    qDebug() << path.GetString().c_str();
    //}

    return app.exec();
}
