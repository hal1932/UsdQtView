#include "stdafx.h"
#include "Camera.h"

void Camera::spin(float x, float y) {
    auto nextPos = pos_;
    auto nextUp = up_;

    const auto toSubject = subject() - nextPos;
    auto front = glm::normalize(toSubject);
    const auto right = glm::cross(front, nextUp);

    nextPos += toSubject;
    front = glm::rotate(front, y, right);
    nextUp = glm::normalize(glm::cross(right, front));
    front = glm::rotate(front, x, nextUp);
    nextPos -= front * glm::length(toSubject);

    nextUp = glm::normalize(glm::cross(right, front));

    pos_ = nextPos;
    up_ = nextUp;
}

void Camera::track(float x, float y) {
    auto nextPos = pos_;
    auto nextSubject = subject_;

    const auto front = glm::normalize(nextPos - nextSubject);
    auto right = glm::cross(up_, front);
    const auto up = glm::cross(front, right);
    right = glm::cross(up, front);

    const auto deltaRight = x * right;
    const auto deltaUp = y * up;
    nextPos += deltaRight + deltaUp;
    nextSubject += deltaRight + deltaUp;

    pos_ = nextPos;
    subject_ = nextSubject;
}

void Camera::dolly(float value) {
    auto nextPos = pos_;

    const auto front = glm::normalize(nextPos - subject_);
    const auto delta = -front * static_cast<float>(value);
    nextPos += delta;

    pos_ = nextPos;
}
