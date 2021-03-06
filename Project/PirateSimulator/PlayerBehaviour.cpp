#include "PlayerBehaviour.h"
#include "InputManager.h"
#include "TimeManager.h"
#include "CameraManager.h"
#include "ShapeComponent.h"

using namespace PirateSimulator;
using namespace DirectX;
using namespace physx;

PlayerBehaviour::PlayerBehaviour() : m_speed{5000.0f}, m_cameraRef{CameraManager::singleton.getMainCameraGO()}, m_tangling{0.0f}, m_smooth{0.05f}
{}

void PlayerBehaviour::move(Move::Translation::Direction direction, float elapsedTime)
{
    //Get Actor shape to move it
    ShapeComponent* boatShape = PhysicsManager::singleton.getVehiculeShape();
	
    // Change the boat forward to match camera forward 
    GameObjectRef camera = CameraManager::singleton.getMainCameraGO();
    const XMVECTOR &boatForward = m_gameObject->m_transform.getForward();
    const XMVECTOR &boatRight = m_gameObject->m_transform.getRight();

    physx::PxVec3 velocity = boatShape->pxActor()->getLinearVelocity();
    switch(direction)
    {
        case PirateSimulator::Move::Translation::FORWARD:
        {
            m_desiredDirection = XMVECTOR{boatForward.vector4_f32[0], 0.0f, boatForward.vector4_f32[2]};
            XMVECTOR tmp = m_desiredDirection * m_speed * elapsedTime;
            physx::PxVec3 dir(tmp.vector4_f32[0], tmp.vector4_f32[1], tmp.vector4_f32[2]);
            if(velocity.magnitudeSquared() < (m_maxSpeed * m_maxSpeed))
                boatShape->pxActor()->addForce(dir * 100.0f);
            break;

        }
        case PirateSimulator::Move::Translation::LEFT:
        {
            m_desiredDirection = XMVECTOR{-boatRight.vector4_f32[0], 0.0f, -boatRight.vector4_f32[2]};
            XMVECTOR tmp = m_desiredDirection * m_speed * elapsedTime;
            physx::PxVec3 dir(tmp.vector4_f32[0], tmp.vector4_f32[1], tmp.vector4_f32[2]);
            boatShape->pxActor()->addForce(dir * 50.0f);
            break;

        }
        case PirateSimulator::Move::Translation::RIGHT:
        {
            m_desiredDirection = XMVECTOR{boatRight.vector4_f32[0], 0.0f, boatRight.vector4_f32[2]};
            XMVECTOR tmp = m_desiredDirection * m_speed * elapsedTime;
            physx::PxVec3 dir(tmp.vector4_f32[0], tmp.vector4_f32[1], tmp.vector4_f32[2]);
            boatShape->pxActor()->addForce(dir * 50.0f);
            break;
        }
        default:
            break;
    }

    m_gameObject->m_transform.setForward(velocity.x, velocity.y, velocity.z);
}

void PlayerBehaviour::rotate(Move::Rotation::Direction direction)
{}

void PlayerBehaviour::anime(float elapsedTime)
{
    CDIManipulateur& rGestionnaireDeSaisie = InputManager::singleton.getManipulator();

    // Add contrary force to stop the boat
    ShapeComponent* boatShape = PhysicsManager::singleton.getVehiculeShape();
    physx::PxVec3 velocity = boatShape->pxActor()->getLinearVelocity();
    velocity.normalize();
    boatShape->pxActor()->addForce(-velocity * 10.0f);

    // Nake the boat look at the movement direction
    if(rGestionnaireDeSaisie.getKey(DIK_W))
    {
        move(Move::Translation::FORWARD, elapsedTime);
    }	
    if(rGestionnaireDeSaisie.getKey(DIK_A))
    {
        move(Move::Translation::LEFT, elapsedTime);
    }
    if(rGestionnaireDeSaisie.getKey(DIK_D))
	{
        move(Move::Translation::RIGHT, elapsedTime);
    }
	
    // Boat tangling
    physx::PxTransform pose = boatShape->pose();
    m_tangling += 0.05f;
    pose.p.y = 0.f;
    boatShape->setPose(pose);
    float angle = sinf(m_tangling);
    m_gameObject->setPosition(pose.p.x, angle * 0.5f, pose.p.z);
}

void PlayerBehaviour::setGameObject(GameObject* parent)
{
    m_gameObject = parent;
}
