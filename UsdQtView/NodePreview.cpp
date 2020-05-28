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
    const auto degree = e->angleDelta().y() / 8;

    auto pos = camera_.position();
    auto focus = camera_.focus();
    const auto front = glm::normalize(pos - focus);

    const auto deltaFront = front * static_cast<float>(degree) * stepSize;
    pos -= deltaFront;
    focus -= deltaFront;

    camera_.setPosition(pos);
    //camera_.setFocus(focus);

    updateCamera();
}

void NodePreview::mouseMoveEvent(QMouseEvent* e) {
    auto mouseDelta = e->pos() - lastMousePos_;

    if (e->buttons() & Qt::LeftButton) {
        const auto stepSize = 0.5f;

        auto pos = camera_.position();
        const auto toOrigin = camera_.focus() - pos;
        auto front = glm::normalize(toOrigin);

        auto up = camera_.up();
        auto right = glm::cross(front, up);

        pos += toOrigin;
        front = glm::rotate(front, -glm::radians(static_cast<float>(mouseDelta.y()) * stepSize), right);
        up = glm::normalize(glm::cross(right, front));
        front = glm::rotate(front, -glm::radians(static_cast<float>(mouseDelta.x()) * stepSize), up);
        pos -= front * glm::length(toOrigin);

        up = glm::cross(right, front);

        camera_.setPosition(pos);
        camera_.setUp(glm::normalize(up));
        updateCamera();
    } else if (e->buttons() & Qt::RightButton) {
        const auto stepSize = 2.f;

        auto pos = camera_.position();
        auto focus = camera_.focus();

        const auto front = glm::normalize(pos - focus);
        auto right = glm::cross(camera_.up(), front);
        const auto up = glm::cross(front, right);
        right = glm::cross(up, front);

        const auto deltaRight = -right * static_cast<float>(mouseDelta.x()) * stepSize;
        const auto deltaUp = up * static_cast<float>(mouseDelta.y()) * stepSize;
        pos += deltaRight + deltaUp;
        focus += deltaRight + deltaUp;

        camera_.setPosition(pos);
        camera_.setFocus(focus);

        updateCamera();
    }

    lastMousePos_ = e->pos();
}

void NodePreview::updateCamera() {
    cbVertScene_.resource().viewProj = camera_.proj() * camera_.view();
    cbVertScene_.upload(GL_DYNAMIC_DRAW);
}
