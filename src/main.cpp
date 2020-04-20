//! Copyright © Mattias Larsson Sköld

#include "matgui/application.h"
#include "matgui/draw.h"
#include "matgui/window.h"

#include "assets.h"
#include "modelobject.h"

#include "box.h"

#include <iostream>

using namespace std;
using namespace Engine;
using namespace MatGui;

int main(int argc, char **argv) {
    Application app(argc, argv);

    Window window("Lasersköld vehicle sim");

    setDepthEnabled(true);

    Application::ContinuousUpdates(true);

    window.frameUpdate.connect([]() {
        static double phase = 0;
        phase += .01;
        renderBox(
            Matrixf::RotationZ(phase).rotate(phase, Vec(1, 1).normalize()),
            Matrixf(),
            Matrixf::Scale(.5));
    });

    app.mainLoop();

    return 0;
}
