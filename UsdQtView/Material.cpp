#include "stdafx.h"
#include "Material.h"
#include "Camera.h"


MaterialVariation::MaterialVariation(Material* pMaterial, uint variation)
    : pMaterial_(pMaterial), variation_(variation) {
}

uint32_t MaterialVariation::hash() {
    auto ptr = reinterpret_cast<intptr_t>(pMaterial_) & 0xFFFF;
    auto var = variation_ & 0xFFFF;
    return (ptr << 16) | var;
}

void MaterialVariation::bind() {
    pMaterial_->bind(variation_);
}

void MaterialVariation::bindTexture(Texture* pTexture, GLenum slot, const char* samplerName) {
    const auto program = pMaterial_->handle(variation_);
    pTexture->bind(program, slot, samplerName);
}

void MaterialVariation::beginScene() {
    pMaterial_->beginScene(variation_);
}

void MaterialVariation::endScene() {
    pMaterial_->endScene(variation_);
}


void Material::create() {
    cbVertScene_.create();
}

void Material::destroy() {
    for (auto& [_, program] : programs_) {
        gl.glDeleteProgram(program);
    }
    clearShaders();
    cbVertScene_.destroy();
}

void Material::defineKeyword(const char* keyword) {
    if (keywords_.size() > 31) {
        std::cerr << "[ERROR] max number of shader keywords (31) exceeded, " << keyword << " is ignored" << std::endl;
        return;
    }
    keywords_.push_back(keyword);
    keyword_hashes_[keyword] = 0b01 << keywords_.size();
}

void Material::beginKeywordVariation() {
    shaderHash_ = 0U;
}

MaterialVariation Material::endKeywordVariation() {
    return MaterialVariation(this, shaderHash_);
}

void Material::enableKeyword(const char* keyword) {
    shaderHash_ |= keyword_hashes_[keyword];
}

void Material::disableKeyword(const char* keyword) {
    shaderHash_ &= ~keyword_hashes_[keyword];
}

void Material::compile(GLenum type, const char* filePath) {
    shaders_[type] = compileShader(type, filePath);
}

void Material::link() {
    for (auto variation : variations_) {
        auto program = gl.glCreateProgram();

        for (auto& [_, shaders] : shaders_) {
            gl.glAttachShader(program, shaders[variation]);
        }
        gl.glLinkProgram(program);
        checkProgramError(program, GL_LINK_STATUS);

        programs_[variation] = program;
    }
}

GLuint Material::handle(uint variation) {
    return programs_[variation];
}

void Material::beginScene(uint variation) {
    bind(variation);

    cbVertScene_.resource().viewProj = pCamera_->proj() * pCamera_->view();
    cbVertScene_.upload(GL_DYNAMIC_DRAW);
    bindUniformBlock(variation, &cbVertScene_, "CbVertScene");
}

void Material::endScene(uint variation) {}

void Material::bind(uint variation) {
    const auto program = handle(variation);
    gl.glUseProgram(program);
}

void Material::bindTexture(uint variation, Texture* pTexture, GLenum slot, const char* samplerName) {
    const auto program = handle(variation);
    pTexture->bind(program, slot, samplerName);
}

void Material::clearShaders() {
    for (auto& [_, shaders] : shaders_) {
        for (auto& [_, shader] : shaders) {
            gl.glDeleteShader(shader);
        }
    }
    shaders_.clear();
}

std::map<uint, GLuint> Material::compileShader(GLenum type, const char* filePath) {
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
        variations_.insert(hash);

        if (i + 1U < keywords_.size()) {
            for (auto j = i + 1; j < keywords_.size(); ++j) {
                keywords += "#define ";
                keywords += keywords_[j];
                keywords += "\n";
                hash |= keyword_hashes_[keywords_[j]];
            }
            shaders[hash] = compileShaderImpl(keywords);
            variations_.insert(hash);
        }
    }

    shaders[0U] = compileShaderImpl("#version 460\n");
    variations_.insert(0U);

    return std::move(shaders);
}

std::string Material::readFile(const char* filePath) {
    std::ifstream file(filePath);
    std::string code = std::string(
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    );
    return std::move(code);
}

void Material::checkShaderError(GLuint handle, GLenum param) {
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

void Material::checkProgramError(GLuint handle, GLenum param) {
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

