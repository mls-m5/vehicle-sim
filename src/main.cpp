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

std::unique_ptr<btRigidBody> createRigidBody(float mass,
                                             btCollisionShape *shape) {
    bool isDynamic = mass != 0.f;

    btVector3 localInertia(0, 0, 0);
    if (isDynamic) {
        shape->calculateLocalInertia(mass, localInertia);
    }

    return make_unique<btRigidBody>(mass, nullptr, shape, localInertia);
}

int main(int argc, char **argv) {
    Application app(argc, argv);

    const int width = 600, height = 400;

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
    groundTransform.setOrigin(btVector3(0, 0, -70));

    auto groundBody = createRigidBody(0, groundShape.get());
    groundBody->setWorldTransform(groundTransform);

    dynamicsWorld->addRigidBody(groundBody.get());

    // test shape

    auto testShape = std::make_unique<btBoxShape>(btVector3(1, 1, 1));

    btTransform testTransform;
    testTransform.setIdentity();

    // rb1
    testTransform.setOrigin(btVector3(0, 0, 0));

    auto testBody = createRigidBody(1, testShape.get());

    dynamicsWorld->addRigidBody(testBody.get());

    // rb2

    testTransform.setOrigin(btVector3(.5f, 0, 1.f));

    auto testBody2 = createRigidBody(1, testShape.get());
    testBody2->setWorldTransform(testTransform);

    dynamicsWorld->addRigidBody(testBody2.get());

    // -------------------------------------------------

    auto projection =
        Matrixf::Scale(.5, .5, .5) * Matrixf::Translation(0, 0, -.8f) *
        Matrixf::Scale(
            static_cast<float>(height) / static_cast<float>(width), 1, 1);

    projection.w3 = .5;

    window.frameUpdate.connect([&](double t) {
        static double phase = 0;

        dynamicsWorld->stepSimulation(static_cast<float>(t));

        phase += .01;

        Matrixf transform;
        testBody->getWorldTransform().getOpenGLMatrix(&transform.x1);

        auto viewTransform =
            Matrixf::RotationX(pi / 2.).scale(.05f, .05f, .05f) *
            Matrixf::Translation(-transform.row(3));

        sim::renderBox(transform, viewTransform, projection);

        testBody2->getWorldTransform().getOpenGLMatrix(&transform.x1);
        sim::renderBox(transform, viewTransform, projection);

        groundBody->getWorldTransform().getOpenGLMatrix(&transform.x1);
        sim::renderBox(transform.scale(50, 50, 50), viewTransform, projection);

        //        sim::renderBox(
        //            Matrixf::RotationZ(phase).rotate(phase, Vec(1,
        //            1).normalize()), Matrixf(), Matrixf::Scale(.5));

        //        sim::renderCylinder(
        //            Matrixf::RotationZ(phase).rotate(-phase, Vec(1,
        //            1).normalize()), Matrixf(), Matrixf::Scale(.5));

        cout << transform.z4 << endl;
    });

    app.mainLoop();

    return 0;
}
