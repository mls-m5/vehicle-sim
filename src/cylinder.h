// Copyright © Mattias Larsson Sköld 2020

#include "matrix.h"

namespace sim {

void renderCylinder(const Matrixf &model,
                    const Matrixf &view,
                    const Matrixf &projection);

//! Render a cylinder that is rotated so that the x axis is the center
void renderCylinderX(const Matrixf &model,
                     const Matrixf &view,
                     const Matrixf &projection);

//! Render a cylinder that is rotated so that the y axis is the center
void renderCylinderY(const Matrixf &model,
                     const Matrixf &view,
                     const Matrixf &projection);

} // namespace sim
