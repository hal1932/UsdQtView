#include "stdafx.h"
#include "NodePreview.h"
#include <fstream>
#include <QFileInfo>

void NodePreview::initializeGL() {
    emit loaded();

    gl.glClearColor(0.4f, 0.4f, 0.4f, 1.f);
    gl.glClearDepth(1.0);
    gl.glEnable(GL_DEPTH_TEST);

    shaderPipe_.create();
    shaderPipe_.defineKeyword("ENABLE_TEXTURE_0");
    shaderPipe_.defineKeyword("ENABLE_DISPLAY_COLOR");
    shaderPipe_.compile(GL_VERTEX_SHADER, QFileInfo("simple.vert").absoluteFilePath().toStdString().c_str());
    shaderPipe_.compile(GL_FRAGMENT_SHADER, QFileInfo("simple.frag").absoluteFilePath().toStdString().c_str());
    shaderPipe_.link();

    camera_.setPosition(glm::vec3(0.f, 0.f, 1000.f));
    camera_.setFocus(glm::vec3(0, 0, 0));
    camera_.setUp(glm::vec3(0, 1, 0));
    camera_.setFov(45.f);
    camera_.setScreen(640.f, 480.f);
    camera_.setClip(0.1f, 10000.f);

    cbVertScene_.create();
    cbVertScene_.resource().viewProj = camera_.proj() * camera_.view();
    cbVertScene_.upload(GL_DYNAMIC_DRAW);

    setMouseTracking(true);
}

void NodePreview::paintGL() {
    gl.glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shaderPipe_.beginKeywordVariation();
    shaderPipe_.enableKeyword("ENABLE_DISPLAY_COLOR");
    shaderPipe_.endKeywordVariation();

    shaderPipe_.bind();
    shaderPipe_.bindUniformBlock(&cbVertScene_, "CbVertScene");

    pNode_->render(&shaderPipe_);
}

void NodePreview::wheelEvent(QWheelEvent* e) {
    const auto stepSize = 2.f;
    const auto value = e->angleDelta().y() / 8;
    camera_.dolly(value);
    updateCamera();
}

void NodePreview::mouseMoveEvent(QMouseEvent* e) {
    auto mouseDelta = e->pos() - lastMousePos_;

    if (pLastKeyEvent_ != nullptr && pLastKeyEvent_->modifiers().testFlag(Qt::AltModifier)) {
        const auto buttons = e->buttons();
        if (buttons.testFlag(Qt::LeftButton)) {
            const auto stepSize = 0.5f;
            const auto x = -glm::radians(static_cast<float>(mouseDelta.x()) * stepSize);
            const auto y = -glm::radians(static_cast<float>(mouseDelta.y()) * stepSize);
            camera_.spin(x, y);
            updateCamera();
        } else if (buttons.testFlag(Qt::MiddleButton)) {
            const auto stepSize = 2.f;
            const auto x = -static_cast<float>(mouseDelta.x()) * stepSize;
            const auto y = static_cast<float>(mouseDelta.y()) * stepSize;
            camera_.pan(x, y);
            updateCamera();
        } else if (buttons.testFlag(Qt::RightButton)) {
            const auto stepSize = 1.f;
            const auto delta = std::abs(mouseDelta.x()) > std::abs(mouseDelta.y()) ? mouseDelta.x() : mouseDelta.y();
            const auto value = static_cast<float>(delta) * stepSize;
            camera_.dolly(value);
            updateCamera();
        }
    }

    lastMousePos_ = e->pos();
}

bool NodePreview::eventFilter(QObject* watched, QEvent* e) {
    switch (e->type()) {
        case QEvent::KeyPress:
            pLastKeyEvent_ = static_cast<QKeyEvent*>(e);
            break;

        case QEvent::KeyRelease:
            pLastKeyEvent_ = nullptr;
            break;
    }

    return false;
}

void NodePreview::updateCamera() {
    cbVertScene_.resource().viewProj = camera_.proj() * camera_.view();
    cbVertScene_.upload(GL_DYNAMIC_DRAW);
}
