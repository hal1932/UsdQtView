#pragma once
#include <unordered_set>
#include "gl.h"
#include "cbuffers.h"

class Material;
class Camera;

class MaterialVariation final {
public:
    MaterialVariation() = default;
    MaterialVariation(Material* pMaterial, uint variation);

    uint32_t hash();
    void bind();
    void bindTexture(Texture* pTexture, GLenum slot, const char* samplerName);

    template<class TResource>
    void bindUniformBlock(UniformBlock<TResource>* pBlock, const char* blockName) {
        pMaterial_->bindUniformBlock(variation_, pBlock, blockName);
    }

    void beginScene();
    void endScene();

private:
    Material* pMaterial_;
    uint variation_;
};

class Material final {
public:
    void create();
    void destroy();

    void defineKeyword(const char* keyword);
    void beginKeywordVariation();
    MaterialVariation endKeywordVariation();

    void enableKeyword(const char* keyword);
    void disableKeyword(const char* keyword);

    void compile(GLenum type, const char* filePath);
    void link();

    void setCamera(Camera* pCamera) { pCamera_ = pCamera; }

    GLuint handle(uint variation);

    void beginScene(uint variation);
    void endScene(uint variation);

    void bind(uint variation);
    void bindTexture(uint variation, Texture* pTexture, GLenum slot, const char* samplerName);

    template<class TResource>
    void bindUniformBlock(uint variation, UniformBlock<TResource>* pBlock, const char* blockName) {
        const auto program = handle(variation);
        pBlock->bind(program, blockName);
    }

private:
    std::vector<std::string> keywords_;
    std::map<std::string, uint> keyword_hashes_;
    std::unordered_set<uint> variations_;
    uint shaderHash_;

    std::map<uint, GLuint> programs_;
    std::map<GLenum, std::map<uint, GLuint>> shaders_;

    Camera* pCamera_;
    UniformBlock<CbVertScene> cbVertScene_;

    void clearShaders();
    std::map<uint, GLuint> compileShader(GLenum type, const char* filePath);
    std::string readFile(const char* filePath);
    void checkShaderError(GLuint handle, GLenum param);
    void checkProgramError(GLuint handle, GLenum param);
};
