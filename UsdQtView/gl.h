#pragma once
#include <QOpenGLFunctions_4_5_Core>
#include <limits>
#include <fstream>
#include <GL/glpng.h>

extern QOpenGLFunctions_4_5_Core gl;


inline void setupOpenGL() {
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setVersion(4, 6);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setSwapInterval(1);
    QSurfaceFormat::setDefaultFormat(format);

    gl.initializeOpenGLFunctions();
}


const auto _GLUINT_INVALID = std::numeric_limits<GLuint>::max();


template<class TBase>
class GLObject {
public:
    GLObject() = default;
    ~GLObject() { destroy(); }

    GLObject(GLObject&) = delete;
    GLObject(const GLObject&) = delete;
    GLObject& operator=(GLObject&) = delete;
    GLObject& operator=(const GLObject&) = delete;

    virtual TBase& operator=(TBase&&) = 0;
    virtual void create() {}
    virtual void destroy() {}
};


template<class TElem>
class BindableBuffer final : GLObject<BindableBuffer<TElem>> {
    friend class BindableBufferPool;

public:
    BindableBuffer(GLenum target)
        : target_(target) {
    }

    BindableBuffer<TElem>& operator=(BindableBuffer<TElem>&& other) override {
        handle_ = std::move(other.handle_);
        target_ = std::move(other.target_);
        resource_ = std::move(other.resource_);
        return *this;
    }

    void create() override {
        gl.glCreateBuffers(1, &handle_);
    }

    void destroy() override {
        if (handle_ != _GLUINT_INVALID) {
            gl.glDeleteBuffers(1, &handle_);
            handle_ = _GLUINT_INVALID;
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
    GLuint handle_ = _GLUINT_INVALID;
    GLuint target_;
    std::vector<TElem> resource_;
};


template<class TVertex>
class InputLayout final : GLObject<InputLayout<TVertex>> {
public:
    InputLayout(const BindableBuffer<TVertex>* pVertices)
        : pVertices_(pVertices) {
    }

    InputLayout& operator=(InputLayout&& other) override {
        handle_ = std::move(other.handle_);
        pVertices_ = std::move(other.pVertices_);
        return *this;
    }

    void create() override {
        gl.glCreateVertexArrays(1, &handle_);
        bind();
        pVertices_->bind();
    }

    void destroy() override {
        if (handle_ != _GLUINT_INVALID) {
            gl.glDeleteVertexArrays(1, &handle_);
            handle_ = _GLUINT_INVALID;
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
    GLuint handle_ = _GLUINT_INVALID;
    const BindableBuffer<TVertex>* pVertices_;
};


GLint getUniqueIndex();


template<class TResource>
class UniformBlock final : GLObject<UniformBlock<TResource>> {
public:
    UniformBlock<TResource>& operator=(UniformBlock<TResource>&& other) override {
        resource_ = std::move(other.resource_);
        handle_ = std::move(other.handle_);
        location_ = std::move(other.location_);
        lastBoundName_ = std::move(other.lastBoundName_);
        return *this;
    }

    void create() override {
        gl.glGenBuffers(1, &handle_);
    }

    void destroy() override {
        if (handle_ != _GLUINT_INVALID) {
            gl.glDeleteBuffers(1, &handle_);
            handle_ = _GLUINT_INVALID;
        }
    }

    void upload(GLenum usage) {
        gl.glBindBuffer(GL_UNIFORM_BUFFER, handle_);
        gl.glBufferData(GL_UNIFORM_BUFFER, sizeof(TResource), &resource_, usage);
    }

    void bind(GLuint program, const char* blockName) {
        if (blockName != lastBoundName_) {
            const auto blockIndex = gl.glGetUniformBlockIndex(program, blockName);
            //location_ = getUniqueIndex();
            location_ = blockIndex;
            gl.glUniformBlockBinding(program, blockIndex, location_);
            lastBoundName_ = blockName;
        }
        gl.glBindBufferBase(GL_UNIFORM_BUFFER, location_, handle_);
    }

    TResource& resource() { return resource_; }
    GLuint handle() { return handle_; }
    GLint location() { return location_; }

private:
    TResource resource_;
    GLuint handle_ = _GLUINT_INVALID;
    GLint location_;
    const char* lastBoundName_;
};


class Texture final : GLObject<Texture> {
public:
    Texture& operator=(Texture&& other) override {
        handle_ = std::move(other.handle_);
        sampler_ = std::move(other.sampler_);
        samplerLocation_ = std::move(other.samplerLocation_);
        lastBoundName_ = std::move(other.lastBoundName_);
        return *this;
    }

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
        gl.glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, info.Width, info.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, info.Data);

        gl.glTexParameteri(sampler_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        gl.glTexParameteri(sampler_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
    GLuint handle_ = _GLUINT_INVALID;
    GLuint sampler_ = _GLUINT_INVALID;
    GLint samplerLocation_;
    const char* lastBoundName_;
};


class ShaderPipe final : GLObject<ShaderPipe> {
public:
    ShaderPipe& operator=(ShaderPipe&& other) override {
        program_ = std::move(other.program_);
        shaders_ = std::move(other.shaders_);
        return *this;
    }

    void create() override {
        program_ = gl.glCreateProgram();
    }

    void destroy() override {
        gl.glDeleteProgram(program_);

        for (auto shader : shaders_) {
            gl.glDeleteShader(shader);
        }
        shaders_.clear();
    }

    void compile(GLenum type, const char* filePath) {
        auto shader = gl.glCreateShader(type);
        compileShader(shader, filePath);
        shaders_.push_back(shader);
        gl.glAttachShader(program_, shader);
    }

    void link() {
        gl.glLinkProgram(program_);
        logError(program_, GL_LINK_STATUS);

        for (auto shader : shaders_) {
            gl.glDeleteShader(shader);
        }
        shaders_.clear();
    }

    void bind() {
        gl.glUseProgram(program_);
    }

    template<class TResource>
    void bindUniformBlock(UniformBlock<TResource>* pBlock, const char* blockName) {
        pBlock->bind(program_, blockName);
    }

    void bindTexture(Texture* pTexture, GLenum slot, const char* samplerName) {
        pTexture->bind(program_, slot, samplerName);
    }

private:
    GLuint program_;
    std::vector<GLuint> shaders_;

    void compileShader(GLuint shader, const char* filePath) {
        std::ifstream file(filePath);

        std::string code = std::string(
            std::istreambuf_iterator<char>(file),
            std::istreambuf_iterator<char>()
        );

        auto len = static_cast<GLint>(code.length());
        auto pCode = code.c_str();
        gl.glShaderSource(shader, 1, &pCode, &len);
        gl.glCompileShader(shader);

        logError(shader, GL_COMPILE_STATUS);
    }

    void logError(GLuint handle, GLenum param){
        if (gl.glGetError() != 0) {
            auto success = 0;
            gl.glGetProgramiv(handle, param, &success);

            if (success == GL_FALSE) {
                auto infoLen = 0;
                gl.glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &infoLen);

                if (infoLen > 1) {
                    auto info = new GLchar[infoLen];
                    gl.glGetProgramInfoLog(handle, infoLen, NULL, info);
                    std::cerr << info << std::endl;
                    delete[] info;
                }
            }
        }
    }
};
