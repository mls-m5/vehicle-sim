// Copyright © Mattias Larsson Sköld 2020

#include "vehicle1.h"
#include "box.h"
#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"
#include "cylinder.h"

using namespace std;

namespace {

std::pair<unique_ptr<btRigidBody>, unique_ptr<btCollisionShape>> createBox(
    btTransform transform, btVector3 halfDims, btScalar mass) {

    auto shape = make_unique<btBoxShape>(halfDims);

    btVector3 localInertia;
    shape->calculateLocalInertia(mass, localInertia);

    auto body =
        make_unique<btRigidBody>(mass, nullptr, shape.get(), localInertia);

    body->setWorldTransform(transform);

    return {move(body), move(shape)};
}

} // namespace

namespace sim {

struct Vehicle1::Wheel {
    Wheel(btDynamicsWorld *world,
          btVector3 center,
          btRigidBody &mainBody,
          double mass,
          double radius,
          double width)
        : world(world)
        , shape({width, radius, radius})
        , body(mass, nullptr, &shape, calculateInertia(mass))
        , constraint(body,
                     mainBody,
                     btVector3(0, 0, 0),
                     center - mainBody.getWorldTransform().getOrigin(),
                     btVector3(-1, 0, 0),
                     btVector3(-1, 0, 0))
        , radius(radius)
        , width(width) {
        btTransform transform;
        transform.setIdentity();
        transform.setOrigin(center);
        body.setWorldTransform(transform);

        world->addRigidBody(&body);
        world->addConstraint(&constraint);

        body.setFriction(10);
        body.setActivationState(DISABLE_DEACTIVATION);
    }

    void throttle(double value) {
        constraint.enableAngularMotor(true, value, 1);
    }

    ~Wheel() {
        world->removeConstraint(&constraint);
        world->removeRigidBody(&body);
    }

    btVector3 calculateInertia(double mass) {
        btVector3 inertia;

        shape.calculateLocalInertia(mass, inertia);

        return inertia;
    }

    void render(const Matrixf &view, const Matrixf &projection) {
        Matrix<btScalar> model;
        body.getWorldTransform().getOpenGLMatrix(&model.x1);
        model *= Matrixd::Scale(width, radius, radius);
        renderCylinderX(model, view, projection);
    }

    btDynamicsWorld *world;
    btCylinderShapeX shape;
    btRigidBody body;
    btHingeConstraint constraint;
    double radius;
    double width;
};

Vehicle1::Vehicle1(btDynamicsWorld *world,
                   btTransform centerGround,
                   Vehicle1::Vehicle1Settings s)
    : settings(s) {

    auto centerPosition = centerGround.getOrigin() +
                          btVector3(0, 0, s.axisZOffset + s.wheelRadius);

    {
        auto transform = btTransform(
            centerGround.getBasis(),
            centerPosition +
                btVector3(0, s.centerJointOffset + s.frontBodyHalfLength, 0));
        auto [body, shape] = createBox(
            transform,
            btVector3(s.bodyHalfWidth, s.frontBodyHalfLength, s.bodyHalfHeight),
            s.frontWheight);
        frontBody = move(body);
        shapes.push_back(move(shape));
        world->addRigidBody(frontBody.get());
    }

    {
        auto transform = btTransform(
            centerGround.getBasis(),
            centerPosition +
                btVector3(0, -s.centerJointOffset - s.rearBodyHalfLength, 0));
        auto [body, shape] = createBox(
            transform,
            btVector3(s.bodyHalfWidth, s.rearBodyHalfLength, s.bodyHalfHeight),
            s.frontWheight);
        rearBody = move(body);
        shapes.push_back(move(shape));
        world->addRigidBody(rearBody.get());
    }

    {
        waistJoint = make_unique<btHingeConstraint>(
            *frontBody,
            *rearBody,
            btVector3{0, -s.centerJointOffset - s.frontBodyHalfLength, 0},
            btVector3{0, s.centerJointOffset + s.rearBodyHalfLength, 0},
            btVector3(0, 0, 1),
            btVector3(0, 0, 1));

        world->addConstraint(waistJoint.get());
    }

    for (int i : {-1, 1}) {
        wheels.push_back(make_unique<Wheel>(
            world,
            centerPosition +
                btVector3((s.bodyHalfWidth + s.wheelHalfWidth) * i,
                          s.centerJointOffset + s.frontBodyHalfLength +
                              s.frontAxisYOffset,
                          s.axisZOffset),
            *frontBody,
            s.wheelWheigt,
            s.wheelRadius,
            s.wheelHalfWidth));

        wheels.push_back(make_unique<Wheel>(
            world,
            centerPosition +
                btVector3((s.bodyHalfWidth + s.wheelHalfWidth) * i,
                          -s.centerJointOffset - s.rearBodyHalfLength +
                              s.rearAxisYOffset,
                          s.axisZOffset),
            *rearBody,
            s.wheelWheigt,
            s.wheelRadius,
            s.wheelHalfWidth));
    }

    frontBody->setActivationState(DISABLE_DEACTIVATION);
    rearBody->setActivationState(DISABLE_DEACTIVATION);
}

Vehicle1::~Vehicle1() {
}

void Vehicle1::render(Matrixf view, Matrixf projection) {
    Matrixd transform;
    frontBody->getWorldTransform().getOpenGLMatrix(&transform.x1);
    transform *= Matrixd::Scale(settings.bodyHalfWidth,
                                settings.frontBodyHalfLength,
                                settings.bodyHalfHeight);
    renderBox(transform, view, projection);

    rearBody->getWorldTransform().getOpenGLMatrix(&transform.x1);
    transform *= Matrixd::Scale(settings.bodyHalfWidth,
                                settings.rearBodyHalfLength,
                                settings.bodyHalfHeight);
    renderBox(transform, view, projection);

    for (auto &wheel : wheels) {
        wheel->render(view, projection);
    }
}

void Vehicle1::steering(double value) {
    waistJoint->enableAngularMotor(true, value * settings.steeringScaling, 10);
}

void Vehicle1::throttle(double value) {
    for (auto &wheel : wheels) {
        wheel->throttle(value * settings.throttleScaling);
    }
}

} // namespace sim
