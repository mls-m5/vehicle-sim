// Copyright © Mattias Larsson Sköld 2020

#include "box.h"
#include "matgui/matgl.h"
#include "mesh.h"
#include "shaders.h"

#include <memory>

using namespace std;

namespace {

Mesh createBoxMesh() {
    Mesh mesh;

    auto &vertices = mesh.vertices;
    auto &indices = mesh.indices;

    for (float i : {1, -1}) {
        const auto start = static_cast<unsigned>(vertices.size());
        vec4 n = {0, 0, i};
        vertices.emplace_back(vec4{-1, -1, i}, n);
        vertices.emplace_back(vec4{1, -1, i}, n);
        vertices.emplace_back(vec4{1, 1, i}, n);
        vertices.emplace_back(vec4{-1, 1, i}, n);

        indices.insert(indices.begin(), {start, start + 1, start + 2});
        indices.insert(indices.begin(), {start, start + 2, start + 3});
    }

    for (float i : {1, -1}) {
        const auto start = static_cast<unsigned>(vertices.size());
        vec4 n = {0, i, 0};
        vertices.emplace_back(vec4{-1, i, -1}, n);
        vertices.emplace_back(vec4{1, i, -1}, n);
        vertices.emplace_back(vec4{1, i, 1}, n);
        vertices.emplace_back(vec4{-1, i, 1}, n);

        indices.insert(indices.begin(), {start, start + 1, start + 2});
        indices.insert(indices.begin(), {start, start + 2, start + 3});
    }

    for (float i : {1, -1}) {
        const auto start = static_cast<unsigned>(vertices.size());
        vec4 n = {i, 0, 0};
        vertices.emplace_back(vec4{i, -1, -1}, n);
        vertices.emplace_back(vec4{i, 1, -1}, n);
        vertices.emplace_back(vec4{i, 1, 1}, n);
        vertices.emplace_back(vec4{i, -1, 1}, n);

        indices.insert(indices.begin(), {start, start + 1, start + 2});
        indices.insert(indices.begin(), {start, start + 2, start + 3});
    }

    return mesh;
}

class BoxModel {
public:
    void render(const Matrixf &mvTransform, const Matrixf &projection) {
        auto mvpTransform = projection * mvTransform;

        boxVao.bind();
        program->use();
        glUniformMatrix4fv(mvpUniform, 1, false, mvpTransform);
        glUniformMatrix4fv(mvUniform, 1, false, mvTransform);
        glCall(glDrawElements(GL_TRIANGLES,
                              static_cast<int>(boxMesh.indices.size()),
                              GL_UNSIGNED_INT,
                              nullptr));
    }

    Mesh boxMesh = createBoxMesh();

    GL::VertexArrayObject boxVao;

    GL::VertexBufferObject boxPos =
        GL::VertexBufferObject(&boxMesh.vertices.front().pos.x,
                               boxMesh.vertices.size() * 4 * 2,
                               0,
                               4,
                               8 * sizeof(float));

    GL::VertexBufferObject boxNormals =
        GL::VertexBufferObject(&boxMesh.vertices.front().normal.x,
                               boxMesh.vertices.size() * 4 * 2,
                               1,
                               4,
                               8 * sizeof(float));

    GL::VertexBufferObject boxIndices = GL::VertexBufferObject(boxMesh.indices);

    ShaderProgram *program = sim::plainShader();

    int mvpUniform = program->getUniform("uMVP");
    int mvUniform = program->getUniform("uMV");

    Matrixf location;
};

std::unique_ptr<BoxModel> boxModel;

} // namespace

namespace sim {

void renderBox(const Matrixf &model,
               const Matrixf &view,
               const Matrixf &projection) {

    if (!boxModel) {
        boxModel = make_unique<BoxModel>();
    }

    boxModel->render(view * model, projection);
}

} // namespace sim
