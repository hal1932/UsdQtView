#pragma once

struct CbVertScene {
    glm::mat4x4 viewProj;
};

struct CbVertObj {
    glm::mat4x4 transform;
};

struct CBFlagObj {
    glm::vec4 displayColor;
};
