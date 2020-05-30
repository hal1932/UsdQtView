#pragma once
#include <QOpenGLFunctions_4_5_Core>
#include <limits>
#include <fstream>
#include <vector>
#include <map>
#include <GL/glpng.h>

extern QOpenGLFunctions_4_5_Core gl;


void setupOpenGL();


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
        bindingPoint_ = std::move(other.bindingPoint_);
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
    GLuint handle() { return handle_; }
    GLint location() { return bindingPoint_; }

private:
    TResource resource_;
    GLuint handle_ = _GLUINT_INVALID;
    GLint bindingPoint_;
    std::string lastBoundName_;
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


class Material final {
public:
    void create() {}

    void destroy() {
        for (auto& [_, program] : programs_) {
            gl.glDeleteProgram(program);
        }
        clearShaders();
    }

    void defineKeyword(const char* keyword) {
        if (keywords_.size() > 31) {
            std::cerr << "[ERROR] max number of shader keywords (31) exceeded, " << keyword << " is ignored" << std::endl;
            return;
        }
        keywords_.push_back(keyword);
        keyword_hashes_[keyword] = 0b01 << keywords_.size();
    }

    void beginKeywordVariation() {
        shaderHash_ = 0U;
    }

    void endKeywordVariation() {}

    void enableKeyword(const char* keyword) {
        shaderHash_ |= keyword_hashes_[keyword];
    }

    void disableKeyword(const char* keyword) {
        shaderHash_ &= ~keyword_hashes_[keyword];
    }

    void compile(GLenum type, const char* filePath) {
        shaders_[type] = compileShader(type, filePath);
    }

    void link() {
        for (auto& [_, hash] : keyword_hashes_) {
            auto program = gl.glCreateProgram();
            for (auto& [type, shaders] : shaders_) {
                gl.glAttachShader(program, shaders[hash]);
            }

            gl.glLinkProgram(program);
            checkProgramError(program, GL_LINK_STATUS);

            programs_[hash] = program;
        }
    }

    GLuint handle() { return programs_[shaderHash_]; }

    void bind() {
        const auto program = handle();
        gl.glUseProgram(program);
    }

    template<class TResource>
    void bindUniformBlock(UniformBlock<TResource>* pBlock, const char* blockName) {
        const auto program = handle();
        pBlock->bind(program, blockName);
    }

    void bindTexture(Texture* pTexture, GLenum slot, const char* samplerName) {
        const auto program = handle();
        pTexture->bind(program, slot, samplerName);
    }

private:
    std::vector<std::string> keywords_;
    std::map<std::string, uint> keyword_hashes_;
    uint shaderHash_;

    std::map<uint, GLuint> programs_;
    std::map<GLenum, std::map<uint, GLuint>> shaders_;

    void clearShaders() {
        for (auto& [_, shaders] : shaders_) {
            for (auto& [_, shader] : shaders) {
                gl.glDeleteShader(shader);
            }
        }
        shaders_.clear();
    }

    std::map<uint, GLuint> compileShader(GLenum type, const char* filePath) {
        std::map<uint, GLuint> shaders;
        const auto sourceCode = readFile(filePath);

        auto compileShaderImpl = [this, type, &shaders, &sourceCode](const std::string& keywords) {
            std::cout << keywords << std::endl;

            const GLchar* sources[] = { keywords.c_str(), sourceCode.c_str() };
            const GLint length[] = { static_cast<GLint>(keywords.length()), static_cast<GLint>(sourceCode.length()) };

            auto shader = gl.glCreateShader(type);
            gl.glShaderSource(shader, 2, sources, length);
            gl.glCompileShader(shader);
            checkShaderError(shader, GL_COMPILE_STATUS);
            return shader;
        };

        for (auto i = 0; i < keywords_.size(); ++i) {
            std::string keywords("#version 460\n");

            keywords += "#define ";
            keywords += keywords_[i];
            keywords += "\n";

            auto hash = keyword_hashes_[keywords_[i]];
            shaders[hash] = compileShaderImpl(keywords);

            if (i + 1 < keywords_.size()) {
                for (auto j = i + 1; j < keywords_.size(); ++j) {
                    keywords += "#define ";
                    keywords += keywords_[j];
                    keywords += "\n";
                    hash |= keyword_hashes_[keywords_[j]];
                }
                shaders[hash] = compileShaderImpl(keywords);
            }
        }

        shaders[0U] = compileShaderImpl("#version 460\n");

        return std::move(shaders);
    }

    std::string readFile(const char* filePath) {
        std::ifstream file(filePath);
        std::string code = std::string(
            std::istreambuf_iterator<char>(file),
            std::istreambuf_iterator<char>()
        );
        return std::move(code);
    }

    void checkShaderError(GLuint handle, GLenum param) {
        auto success = 0;
        gl.glGetShaderiv(handle, param, &success);

        if (success == GL_FALSE) {
            auto infoLen = 0;
            gl.glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &infoLen);

            if (infoLen > 1) {
                auto info = std::make_unique<char>(infoLen);
                gl.glGetShaderInfoLog(handle, infoLen, NULL, info.get());
                std::cerr << info << std::endl;
            }
        }
    }

    void checkProgramError(GLuint handle, GLenum param){
        auto success = 0;
        gl.glGetProgramiv(handle, param, &success);

        if (success == GL_FALSE) {
            auto infoLen = 0;
            gl.glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &infoLen);

            if (infoLen > 1) {
                auto info = std::make_unique<char>(infoLen);
                gl.glGetProgramInfoLog(handle, infoLen, NULL, info.get());
                std::cerr << info << std::endl;
            }
        }
    }
};
