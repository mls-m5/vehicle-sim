// Copyright © Mattias Larsson Sköld 2020

#pragma once

#include "BulletCollision/CollisionShapes/btCollisionShape.h"
#include "BulletDynamics/ConstraintSolver/btHingeConstraint.h"
#include "BulletDynamics/Dynamics/btDynamicsWorld.h"
#include "BulletDynamics/Dynamics/btRigidBody.h"
#include "matrix.h"

#include <memory>
#include <vector>

namespace sim {

class Vehicle1 {
    struct Wheel;

public:
    struct Vehicle1Settings {
        double wheelRadius = 1.5;
        double wheelHalfWidth = .5;
        double bodyHalfWidth = 1.5;
        double bodyHalfHeight = 1;
        double rearBodyHalfLength = 2;
        double frontBodyHalfLength = 1.5;

        double centerJointOffset = 1.5;

        double axisZOffset = -.5;
        double rearAxisYOffset = 1;
        double frontAxisYOffset = 1;

        double frontWheight = 1;
        double rearWheight = 1;
        double wheelWheigt = .2;

        double throttleScaling = 4;
        double steeringScaling = 2;
    };

    Vehicle1(btDynamicsWorld *,
             btTransform center,
             struct Vehicle1Settings settings);

    ~Vehicle1();

    void render(Matrixf view, Matrixf projection);

    void steering(double value);

    void throttle(double value);

    std::unique_ptr<btRigidBody> frontBody;
    std::unique_ptr<btRigidBody> rearBody;

    std::unique_ptr<btHingeConstraint> waistJoint;

    std::vector<std::unique_ptr<Wheel>> wheels;
    std::vector<std::unique_ptr<btCollisionShape>> shapes;

    Vehicle1Settings settings;
};

} // namespace sim
