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

    void spin(float x, float y);
    void track(float x, float y);
    void dolly(float value);

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
