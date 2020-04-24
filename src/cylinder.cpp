// Copyright © Mattias Larsson Sköld 2020

#include "cylinder.h"

#include "matgui/constants.h"
#include "matgui/matgl.h"
#include "mesh.h"
#include "shaders.h"

#include <cmath>
#include <memory>

using namespace MatGui;

namespace {

Mesh createCylinderMesh() {
    const unsigned numPoints = 40;

    Mesh mesh;

    auto &vertices = mesh.vertices;
    auto &indices = mesh.indices;

    for (float z : {1, -1}) {
        const unsigned circleStart = static_cast<unsigned>(vertices.size()) + 1;

        auto n = vec4{0, 0, z};
        vertices.push_back({{0, 0, z}, n});

        for (size_t i = 0; i < numPoints; ++i) {
            auto angle = pi2f / numPoints * i;
            vertices.push_back({{sinf(angle), cosf(angle), z}, n});
        }

        for (unsigned i = 1; i < numPoints; ++i) {
            indices.push_back(circleStart);
            indices.push_back(circleStart + i);
            indices.push_back(circleStart + i - 1);
        }

        indices.push_back(0);
        indices.push_back(circleStart);
        indices.push_back(circleStart + numPoints - 1);
    }

    // ---- Walls -------

    unsigned wallStart = static_cast<unsigned>(vertices.size());

    for (unsigned int i = 0; i < numPoints; ++i) {
        auto angle = pi2f / numPoints * i;
        auto s = sinf(angle);
        auto c = cosf(angle);
        auto n = vec4{s, c};
        vertices.push_back({{s, c, 1}, n});
        vertices.push_back({{s, c, -1}, n});
    }

    for (unsigned i = 1; i < numPoints; ++i) {
        indices.push_back(wallStart + i * 2);
        indices.push_back(wallStart + i * 2 + 1);
        indices.push_back(wallStart + (i - 1) * 2 + 1);

        indices.push_back(wallStart + i * 2);
        indices.push_back(wallStart + (i - 1) * 2);
        indices.push_back(wallStart + (i - 1) * 2 + 1);
    }

    indices.push_back(wallStart);
    indices.push_back(wallStart + 1);
    indices.push_back(wallStart + (numPoints - 1) * 2 + 1);

    indices.push_back(wallStart);
    indices.push_back(wallStart + (numPoints - 1) * 2);
    indices.push_back(wallStart + (numPoints - 1) * 2 + 1);

    return mesh;
}

class CylinderModel {
public:
    CylinderModel() {
        cylVao.unbind();
    }

    void render(const Matrixf &mvTransform, const Matrixf &projection) {
        auto mvpTransform = projection * mvTransform;
        cylVao.bind();
        program->use();

        glUniformMatrix4fv(mvpUniform, 1, false, mvpTransform);
        glUniformMatrix4fv(mvUniform, 1, false, mvTransform);
        glCall(glDrawElements(GL_TRIANGLES,
                              static_cast<int>(cylMesh.indices.size()),
                              GL_UNSIGNED_INT,
                              nullptr));
    }

    Mesh cylMesh = createCylinderMesh();

    GL::VertexArrayObject cylVao;
    GL::VertexBufferObject cylVboPos =
        GL::VertexBufferObject(&cylMesh.vertices.front().pos.x,
                               cylMesh.vertices.size() * 4 * 2,
                               0,
                               4,
                               8 * sizeof(float));

    GL::VertexBufferObject cylVboNormals =
        GL::VertexBufferObject(&cylMesh.vertices.front().normal.x,
                               cylMesh.vertices.size() * 4 * 2,
                               1,
                               4,
                               8 * sizeof(float));

    GL::VertexBufferObject cylIndices = GL::VertexBufferObject(cylMesh.indices);

    ShaderProgram *program = sim::plainShader();

    int mvpUniform = program->getUniform("uMVP");
    int mvUniform = program->getUniform("uMV");
};

std::unique_ptr<CylinderModel> cylinderModel;

// clang-format off
const Matrixf centerXRotation (
        0, 0,-1, 0,
        0, 1, 0, 0,
        1, 0, 0, 0,
        0, 0, 0, 1
        );

const Matrixf centerYRotation (
        1, 0, 0, 0,
        0, 0, 1, 0,
        0,-1, 0, 0,
        0, 0, 0, 1
        );
// clang-format on

} // namespace

namespace sim {
void renderCylinder(const Matrixf &model,
                    const Matrixf &view,
                    const Matrixf &projection) {
    if (!cylinderModel) {
        cylinderModel = std::make_unique<CylinderModel>();
    }

    cylinderModel->render(view * model, projection);
}

void renderCylinderY(const Matrixf &model,
                     const Matrixf &view,
                     const Matrixf &projection) {
    auto nModel = model * centerYRotation;
    renderCylinder(nModel, view, projection);
}

void renderCylinderX(const Matrixf &model,
                     const Matrixf &view,
                     const Matrixf &projection) {
    auto nModel = model * centerXRotation;
    renderCylinder(nModel, view, projection);
}
} // namespace sim
