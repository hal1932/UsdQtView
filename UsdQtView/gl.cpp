#include "stdafx.h"
#include "gl.h"

namespace {
    GLint lastIndex_ = 0;
}

QOpenGLFunctions_4_5_Core gl;


GLint getUniqueIndex() {
    return lastIndex_++;
}
