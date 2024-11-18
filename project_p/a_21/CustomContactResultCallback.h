#pragma once
#include "include/btBulletCollisionCommon.h"
#include "include/btBulletDynamicsCommon.h"
#include <iostream>

// �浹 ����� ������ Custom Callback
class CustomContactResultCallback : public btCollisionWorld::ContactResultCallback {
public:
    bool hitDetected = false;                // �浹 ���� �÷���
    btVector3 collisionPointA;               // �浹 ���� A
    btVector3 collisionPointB;               // �浹 ���� B

    // �浹�� �߻��� ������ ȣ��Ǵ� �Լ�
    virtual btScalar addSingleResult(btManifoldPoint& cp,
        const btCollisionObjectWrapper* colObj0Wrap,
        int partId0, int index0,
        const btCollisionObjectWrapper* colObj1Wrap,
        int partId1, int index1) override
    {
        // �浹 ���� �� hitDetected�� true�� ����
        hitDetected = true;

        // �浹 ���� A�� B�� ���
        collisionPointA = cp.getPositionWorldOnA();
        collisionPointB = cp.getPositionWorldOnB();

        // �ֿܼ� �浹 ��ġ�� ���
        std::cout << "Collision detected at: "
            << "Point A(" << collisionPointA.getX() << ", " << collisionPointA.getY() << ", " << collisionPointA.getZ() << ") - "
            << "Point B(" << collisionPointB.getX() << ", " << collisionPointB.getY() << ", " << collisionPointB.getZ() << ")"
            << std::endl;

        return 0; // ��ȯ���� Ư���� �ǹ̰� �����Ƿ� 0 ��ȯ
    }

    // �浹 ���θ� �����ϴ� �Լ�
    void reset() {
        hitDetected = false;
    }
};