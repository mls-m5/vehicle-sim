// Copyright © Mattias Larsson Sköld 2020

#include "cylinder.h"

#include "matgui/constants.h"
#include "mesh.h"

#include <cmath>

using namespace MatGui;

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
