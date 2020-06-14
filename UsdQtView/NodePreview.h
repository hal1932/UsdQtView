#pragma once
#include "gl.h"
#include "Camera.h"
#include "RenderQueue.h"

class SceneNode;
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
        qApp->installEventFilter(this);
    }

    void setNode(SceneNode* pNode) {
        pNode_ = pNode;
    }
    void updateRenderable();

    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void messageLogged(QOpenGLDebugMessage message);

    void wheelEvent(QWheelEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    bool eventFilter(QObject* watched, QEvent* e) override;

private:
    SceneNode* pNode_ = nullptr;

    Material material_;
    Camera camera_;

    RenderQueue renderQueue_;

    QPoint lastMousePos_;
    QKeyEvent* pLastKeyEvent_ = nullptr;
};
