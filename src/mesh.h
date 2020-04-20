// Copyright © Mattias Larsson Sköld 2020

#pragma once

#include <vector>

struct vec4 {
    float x = 0, y = 0, z = 0, w = 1;

    vec4(float x = 0, float y = 0, float z = 0, float w = 1)
        : x(x), y(y), z(z), w(w) {
    }
};

struct Vertex {
    vec4 pos;
    vec4 normal;

    Vertex() = default;
    Vertex(const Vertex &) = default;
    Vertex &operator=(const Vertex &) = default;

    Vertex(decltype(pos) pos, decltype(normal) normal = {})
        : pos(pos), normal(normal) {
    }
};

struct Mesh {
    std::vector<Vertex> vertices;
    std::vector<unsigned> indices;
};
