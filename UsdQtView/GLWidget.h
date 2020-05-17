#pragma once
#include <cstdlib>
#include <QOpenGLWidget>
#include <QVector3D>
#include <QtOpenGL>
#include "gl.h"
#include "SceneNode.h"
#include "Camera.h"

class GLWidget : public QOpenGLWidget {
    Q_OBJECT
public:
    GLWidget() : QOpenGLWidget(nullptr) {
        auto pTimer = new QTimer(this);
        connect(pTimer, SIGNAL(timeout()), this, SLOT(update()));
        pTimer->start(1000 / 60.0);
        setFixedSize(640, 480);
    }

    void initializeGL() override;
    void paintGL() override;

    void messageLogged(QOpenGLDebugMessage message);

    void wheelEvent(QWheelEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;

private:
    UsdStageRefPtr stage_;
    SceneNode node_;

    ShaderPipe shaderPipe_;
    Camera camera_;
    UniformBlock<CbVertScene> cbVertScene_;

    QPoint lastMousePos_;

    void updateCamera();
};
