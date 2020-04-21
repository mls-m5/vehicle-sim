// Copyright © Mattias Larsson Sköld 2020

#pragma once

#include "matrix.h"

namespace sim {

void renderBox(const Matrixf &model,
               const Matrixf &view,
               const Matrixf &projection);
}
