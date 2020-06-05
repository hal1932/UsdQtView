#pragma once
#include <QOpenGLFunctions_4_5_Core>
#include <limits>
#include <fstream>
#include <vector>
#include <map>
#include <GL/glpng.h>

extern QOpenGLFunctions_4_5_Core gl;


void setupOpenGL();


const auto _INVALID_HANDLE = std::numeric_limits<GLuint>::max();


class GLObject : boost::noncopyable {
public:
    GLObject() = default;
    virtual ~GLObject() { destroy(); }

    virtual void create() {}
    virtual void destroy() {}

    virtual GLuint handle() { return handle_; }
    bool created() { return handle() != _INVALID_HANDLE; }

protected:
    GLuint handle_ = _INVALID_HANDLE;
};


template<class TElem>
class BindableBuffer final : public GLObject {
    friend class BindableBufferPool;

public:
    BindableBuffer(GLenum target)
        : target_(target)
    { }

    void create() override {
        gl.glCreateBuffers(1, &handle_);
    }

    void destroy() override {
        if (handle_ != _INVALID_HANDLE) {
            gl.glDeleteBuffers(1, &handle_);
            handle_ = _INVALID_HANDLE;
        }
    }

    void bind() {
        gl.glBindBuffer(target_, handle_);
    }

    void bind() const {
        gl.glBindBuffer(target_, handle_);
    }

    void upload(GLenum usage) {
        bind();
        gl.glBufferData(target_, resource_.size() * sizeof(TElem), resource_.data(), usage);
    }

    size_t size() { return resource_.size(); }
    std::vector<TElem>& resource() { return resource_; }

private:
    GLuint target_;
    std::vector<TElem> resource_;
};


template<class TVertex>
class InputLayout final : public GLObject {
public:
    InputLayout(const BindableBuffer<TVertex>* pVertices)
        : pVertices_(pVertices)
    { }

    void create() override {
        gl.glCreateVertexArrays(1, &handle_);
        bind();
        pVertices_->bind();
    }

    void destroy() override {
        if (handle_ != _INVALID_HANDLE) {
            gl.glDeleteVertexArrays(1, &handle_);
            handle_ = _INVALID_HANDLE;
        }
    }

    void set(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, size_t offset) {
        gl.glEnableVertexAttribArray(index);
        gl.glVertexAttribPointer(index, size, type, normalized, stride, reinterpret_cast<const void*>(offset));
    }

    void bind() {
        gl.glBindVertexArray(handle_);
    }

private:
    const BindableBuffer<TVertex>* pVertices_;
};


GLint getUniqueIndex();


template<class TResource>
class UniformBlock final : public GLObject {
public:
    void create() override {
        gl.glGenBuffers(1, &handle_);
    }

    void destroy() override {
        if (handle_ != _INVALID_HANDLE) {
            gl.glDeleteBuffers(1, &handle_);
            handle_ = _INVALID_HANDLE;
        }
    }

    void upload(GLenum usage) {
        gl.glBindBuffer(GL_UNIFORM_BUFFER, handle_);
        gl.glBufferData(GL_UNIFORM_BUFFER, sizeof(TResource), &resource_, usage);
    }

    void bind(GLuint program, const std::string& blockName) {
        if (blockName != lastBoundName_) {
            const auto blockIndex = gl.glGetUniformBlockIndex(program, blockName.c_str());
            //bindingPoint_ = getUniqueIndex();
            bindingPoint_ = blockIndex;
            gl.glUniformBlockBinding(program, blockIndex, bindingPoint_);
            lastBoundName_ = blockName;
        }
        gl.glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint_, handle_);
    }

    TResource& resource() { return resource_; }
    GLint location() { return bindingPoint_; }

private:
    TResource resource_;
    GLint bindingPoint_;
    std::string lastBoundName_;
};


class Texture final : public GLObject {
public:
    void create() override {
        gl.glGenTextures(1, &handle_);
        gl.glGenSamplers(1, &sampler_);
    }

    void destroy() override {
        gl.glDeleteTextures(1, &handle_);
        gl.glDeleteSamplers(1, &sampler_);
    }

    void load(const char* filePath) {
        pngRawInfo info;
        pngLoadRaw(filePath, &info);

        gl.glGenTextures(1, &handle_);
        gl.glBindTexture(GL_TEXTURE_2D, handle_);
        gl.glPixelStorei(GL_UNPACK_ALIGNMENT, info.Components);
        gl.glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
        gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        gl.glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, info.Width, info.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, info.Data);

        gl.glSamplerParameteri(sampler_, GL_TEXTURE_WRAP_S, GL_REPEAT);
        gl.glSamplerParameteri(sampler_, GL_TEXTURE_WRAP_T, GL_REPEAT);

        delete[] info.Data;
    }

    void bind(GLuint program, GLenum texture, const char* samplerName) {
        if (samplerName != lastBoundName_) {
            samplerLocation_ = gl.glGetUniformLocation(program, samplerName);
        }

        gl.glActiveTexture(texture);
        gl.glBindTexture(GL_TEXTURE_2D, handle_);

        gl.glBindSampler(samplerLocation_, 0);
    }

private:
    GLuint sampler_ = _INVALID_HANDLE;
    GLint samplerLocation_;
    const char* lastBoundName_;
};
