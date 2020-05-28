#pragma once
#include <cstdlib>
#include <QOpenGLWidget>
#include <QVector3D>
#include <QtOpenGL>
#include "gl.h"
#include "SceneNode.h"
#include "Camera.h"

class NodeRenderer;

class NodePreview : public QOpenGLWidget {
    Q_OBJECT
signals:
    void loaded();

public:
    NodePreview(QWidget* parent) : QOpenGLWidget(parent) {
        auto pTimer = new QTimer(this);
        connect(pTimer, SIGNAL(timeout()), this, SLOT(update()));
        pTimer->start(1000 / 60.0);
        setFixedSize(640, 480);
        qApp->installEventFilter(this);
    }

    void setNode(SceneNode* pNode) { pNode_ = pNode; }

    void initializeGL() override;
    void paintGL() override;

    void messageLogged(QOpenGLDebugMessage message);

    void wheelEvent(QWheelEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    bool eventFilter(QObject* watched, QEvent* e) override;

private:
    SceneNode* pNode_;

    ShaderPipe shaderPipe_;
    Camera camera_;
    UniformBlock<CbVertScene> cbVertScene_;

    QPoint lastMousePos_;
    QKeyEvent* pLastKeyEvent_ = nullptr;

    void updateCamera();
};
