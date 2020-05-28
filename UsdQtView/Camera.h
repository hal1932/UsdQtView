#pragma once
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera final {
public:
    void setPosition(glm::vec3 pos) { pos_ = pos; }
    void setSubject(glm::vec3 subject) { subject_ = subject; }
    void setUp(glm::vec3 up) { up_ = up; }
    void setFov(float fov) { fov_ = fov; }
    void setScreen(float width, float height) { width_ = width; height_ = height; }
    void setClip(float nearZ, float farZ) { near_ = nearZ; far_ = farZ; }

    const glm::vec3& position() { return pos_; }
    const glm::vec3& subject() { return subject_; }
    const glm::vec3& up() { return up_; }
    float fov() { return fov_; }

    glm::mat4x4 view() {
        return glm::lookAtRH(pos_, subject_, up_);
    }

    glm::mat4x4 proj() {
        return glm::perspectiveFovRH(glm::radians(fov_), width_, height_, near_, far_);
    }

    void spin(float x, float y) {
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

    void track(float x, float y) {
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

    void dolly(float value) {
        auto nextPos = pos_;

        const auto front = glm::normalize(nextPos - subject_);
        const auto delta = -front * static_cast<float>(value);
        nextPos += delta;

        pos_ = nextPos;
    }

private:
    glm::vec3 pos_;
    glm::vec3 subject_;
    glm::vec3 up_;
    float fov_;
    float width_;
    float height_;
    float near_;
    float far_;
};
