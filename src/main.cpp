//! Copyright © Mattias Larsson Sköld

#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"

#include "matgui/application.h"
#include "matgui/draw.h"
#include "matgui/window.h"

#include "assets.h"
#include "modelobject.h"

#include "box.h"
#include "cylinder.h"

#include <iostream>

using namespace std;
using namespace Engine;
using namespace MatGui;

int main(int argc, char **argv) {
    Application app(argc, argv);

    Window window("Lasersköld vehicle sim", 640, 400);

    setDepthEnabled(true);

    Application::ContinuousUpdates(true);

    // ---------------- physics ------------------------

    auto dispatcher = make_unique<btDefaultCollisionConfiguration>();

    // -------------------------------------------------

    window.frameUpdate.connect([]() {
        static double phase = 0;
        phase += .01;
        sim::renderBox(
            Matrixf::RotationZ(phase).rotate(phase, Vec(1, 1).normalize()),
            Matrixf(),
            Matrixf::Scale(.5));

        sim::renderCylinder(
            Matrixf::RotationZ(phase).rotate(-phase, Vec(1, 1).normalize()),
            Matrixf(),
            Matrixf::Scale(.5));
    });

    app.mainLoop();

    return 0;
}
