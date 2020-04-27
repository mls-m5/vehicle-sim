// Copyright © Mattias Larsson Sköld 2020

#include "shaders.h"

#include <memory>

namespace {

std::unique_ptr<ShaderProgram> plainShader;

const std::string plainVertexCode =
    R"_(
        #version 330

        layout (location = 0) in vec4 vPosition;
        layout (location = 1) in vec3 vNormal;

        out vec3 fNormal;

        uniform mat4 uMVP;
        uniform mat4 uMV;

        void main() {
            gl_Position = uMVP * vPosition;
            fNormal = normalize(mat3(uMV) * vNormal);
        }
)_";

const std::string plainFragmentCode =
    R"_(
        #version 330

        in vec3 fNormal;

        out vec4 fragColor;

        void main() {
            float intensity = .5 + fNormal.y / 2.;
            fragColor = vec4(intensity, intensity, intensity, 1);
        }

        )_";

} // namespace

namespace sim {

ShaderProgram *plainShader() {
    if (!::plainShader) {
        ::plainShader =
            std::make_unique<ShaderProgram>(plainVertexCode, plainFragmentCode);
    }

    return ::plainShader.get();
}

} // namespace sim
