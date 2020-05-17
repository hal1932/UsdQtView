#pragma once

struct CbVertScene {
    glm::mat4x4 viewProj;
};

struct CbVertObj {
    glm::mat4x4 transform;
    //glm::mat4x4 viewProj;
};

struct CBFlagObj {
    glm::vec4 displayColor;
};
