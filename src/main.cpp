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
#include "vehicle1.h"

#include <iostream>

using namespace std;
using namespace Engine;
using namespace MatGui;

std::unique_ptr<btRigidBody> createRigidBody(btScalar mass,
                                             btCollisionShape *shape) {
    bool isDynamic = mass != 0.;

    btVector3 localInertia(0, 0, 0);
    if (isDynamic) {
        shape->calculateLocalInertia(mass, localInertia);
    }

    return make_unique<btRigidBody>(mass, nullptr, shape, localInertia);
}

int main(int argc, char **argv) {
    Application app(argc, argv);

    const int width = 600 * 2, height = 400 * 2;

    Window window("Lasersköld vehicle sim", width, height);

    setDepthEnabled(true);

    Application::ContinuousUpdates(true);

    // ---------------- physics ------------------------

    auto collisionConfiguration =
        make_unique<btDefaultCollisionConfiguration>();

    auto dispatcher =
        make_unique<btCollisionDispatcher>(collisionConfiguration.get());

    auto broadphase = make_unique<btDbvtBroadphase>();

    auto solver = make_unique<btSequentialImpulseConstraintSolver>();

    auto dynamicsWorld =
        make_unique<btDiscreteDynamicsWorld>(dispatcher.get(),
                                             broadphase.get(),
                                             solver.get(),
                                             collisionConfiguration.get());

    dynamicsWorld->setGravity(btVector3(0, 0, -10));

    // -- shopes etc

    // ground

    auto groundShape = std::make_unique<btBoxShape>(
        btVector3(btScalar(50.), btScalar(50.), btScalar(50.)));

    btTransform groundTransform;
    groundTransform.setIdentity();
    groundTransform.setOrigin(btVector3(0, 0, -50));

    auto groundBody = createRigidBody(0, groundShape.get());
    groundBody->setWorldTransform(groundTransform);

    dynamicsWorld->addRigidBody(groundBody.get());

    const bool enableBasicTestShapes = false;

    // test shape

    auto testShape = std::make_unique<btBoxShape>(btVector3(1, 1, 1));

    btTransform testTransform;
    testTransform.setIdentity();

    // rb1
    testTransform.setOrigin(btVector3(0 + 10, 0, 0));

    auto testBody = createRigidBody(1, testShape.get());

    // rb2

    testTransform.setOrigin(btVector3(.5f + 10, 0, 1.f));

    auto testBody2 = createRigidBody(1, testShape.get());
    testBody2->setWorldTransform(testTransform);

    // cyl

    auto cylinderShape = make_unique<btCylinderShape>(btVector3(1, 1, 1));
    testTransform.setOrigin(btVector3(-.8f + 10, 0, 1));
    auto testBody3 = createRigidBody(1, cylinderShape.get());
    testBody3->setWorldTransform(testTransform);

    // constraint

    if (enableBasicTestShapes) {
        auto constraint1 = make_unique<btHingeConstraint>(*testBody,
                                                          *testBody2,
                                                          btVector3(.5, 0, 0),
                                                          btVector3(0, 0, -.5),
                                                          btVector3(0, 1, 0),
                                                          btVector3(0, 1, 0));

        constraint1->enableAngularMotor(true, 1, 10);

        dynamicsWorld->addRigidBody(testBody.get());
        dynamicsWorld->addRigidBody(testBody2.get());
        dynamicsWorld->addRigidBody(testBody3.get());
        dynamicsWorld->addConstraint(constraint1.get(), true);
    }

    // -- Vehicle ----------

    btTransform vehicleTransform;
    vehicleTransform.setIdentity();
    vehicleTransform.setOrigin({0, 0, -3});
    sim::Vehicle1::Vehicle1Settings settings;
    sim::Vehicle1 vehicle(dynamicsWorld.get(), vehicleTransform, settings);

    // -------------------------------------------------

    auto projection =
        Matrixf::Scale(.5, .5, .5) * Matrixf::Translation(0, 0, -.1f) *
        Matrixf::Scale(
            static_cast<float>(height) / static_cast<float>(width), 1, 1);

    projection.w3 = .5;

    double x = 0, y = 0;
    double scale = 2;

    window.frameUpdate.connect([&](double t) {
        static double phase = 0;

        dynamicsWorld->stepSimulation(static_cast<btScalar>(t));

        phase += .01;

        Matrixd transform;
        testBody->getWorldTransform().getOpenGLMatrix(&transform.x1);

        auto viewTransform = Matrixf::RotationX(pi / 2. + .8 + y) *
                             Matrixf::RotationZ(x * 2) *
                             Matrixf::Scale(.05f * scale); // *
        //                             Matrixf::Translation(-transform.row(3));

        groundBody->getWorldTransform().getOpenGLMatrix(&transform.x1);
        sim::renderBox(transform.scale(50, 50, 50), viewTransform, projection);

        vehicle.render(viewTransform, projection);

        if (enableBasicTestShapes) {
            sim::renderBox(transform, viewTransform, projection);

            testBody2->getWorldTransform().getOpenGLMatrix(&transform.x1);
            sim::renderBox(transform, viewTransform, projection);

            testBody3->getWorldTransform().getOpenGLMatrix(&transform.x1);
            transform = transform * Matrixf::RotationX(pi / 2.);
            sim::renderCylinder(transform, viewTransform, projection);
            //        sim::renderBox(
            //            Matrixf::RotationZ(phase).rotate(phase, Vec(1,
            //            1).normalize()), Matrixf(), Matrixf::Scale(.5));

            //        sim::renderCylinder(
            //            Matrixf::RotationZ(phase).rotate(-phase, Vec(1,
            //            1).normalize()), Matrixf(), Matrixf::Scale(.5));
        }

        auto mouseBoxTransform =
            Matrixf::Identity() * Matrixf::Translation(x * 20., y * 20., -1);
        sim::renderBox(mouseBoxTransform, viewTransform, projection);

        cout << transform.z4 << endl;
    });

    window.pointerMoved.connect([&](View::PointerArgument arg) {
        x = arg.x / width * 2 - 1;
        y = arg.y / height * 2 - 1;
    });

    window.scroll.connect(
        [&](View::ScrollArgument arg) { scale *= (arg.y / 10 + 1); });

    app.mainLoop();

    return 0;
}
