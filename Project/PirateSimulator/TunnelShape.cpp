#include "TunnelShape.h"
#include "PhysicsManager.h"
#include "SoundManager.h"
#include "TimeManager.h"
#include "ShapeComponent.h"
#include "ICollisionHandler.h"
#include "GameObject.h"

using namespace PirateSimulator;
using namespace physx;

class CollisionTunnelHandler : public ICollisionHandler
{
    void onContact(const physx::PxContactPair &aContactPair)
    {
        PirateSimulator::SoundManager::singleton.playMusic("PirateSimulator/Collision1Noise.wav");
        TimeManager::GetInstance().increaseTime(-5s);
    }

    void onTrigger(bool triggerEnter, physx::PxShape *actorShape, physx::PxShape *contactShape) override
    {

    }
};

void TunnelShape::setGameObject(GameObject* parent)
{
    PxTransform tunnelPose;
    DirectX::XMVECTOR position = parent->m_transform.getPosition();

    tunnelPose.p.x = position.vector4_f32[0]+52.f;
    tunnelPose.p.y = position.vector4_f32[1];
    tunnelPose.p.z = position.vector4_f32[2] - 210.f;
    tunnelPose.q = PxQuat(0.7f, 0.7f, 0.f, 0.f).getNormalized();
    auto actor = PhysicsManager::singleton.physics().createRigidStatic(tunnelPose);
    m_shape = actor->createShape(PxCapsuleGeometry(11.f,20.f), *m_material);
    PxFilterData filterDataCylindre;
    filterDataCylindre.word0 = EACTORTUNNEL;
    filterDataCylindre.word1 = EACTORVEHICLE;
    m_shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
    actor->userData = parent;
    m_shape->setSimulationFilterData(filterDataCylindre);
    PhysicsManager::singleton.scene().addActor(*actor);

    //Register shape
    setHandler(ICollisionHandlerRef(new CollisionTunnelHandler));
    m_gameObject = parent;

}

