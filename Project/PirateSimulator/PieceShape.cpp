#include "PieceShape.h"
#include "PhysicsManager.h"
#include "ShapeComponent.h"
#include "Piece.h"
#include "GameObjectManager.h"
#include "ICollisionHandler.h"

using namespace PirateSimulator;
using namespace physx;

class CollisionPieceHandler : public ICollisionHandler
{
    void onContact(const physx::PxContactPair &aContactPair) override
    {
        GameObject* actor0 = static_cast<GameObject*>(aContactPair.shapes[0]->getActor()->userData);
        GameObject* actor1 = static_cast<GameObject*>(aContactPair.shapes[1]->getActor()->userData);

        

        if (actor1->getComponent<ShapeComponent>()->isPiece())

        {
            GameObjectManager::singleton.getPieceAdministrator()->addScore();
            //unspawn the piece UNCOMMENT WHEN FONCTIONS WORKS

            //remove Mesh
            //actor1->getComponent<ShapeComponent>()->getPiece()->destroyPiece();
            //remove physX actor from scene
            PhysicsManager::singleton.scene().removeActor(actor1->getComponent<ShapeComponent>()->pxActor());
            
        }


        else if (actor0->getComponent<ShapeComponent>()->isPiece()) //the piece is not actor1, so it is actor0

        {

            //actor0->getComponent<ShapeComponent>()->getPiece()->destroyPiece();
            PhysicsManager::singleton.scene().removeActor(actor1->getComponent<ShapeComponent>()->pxActor());
        }
    }

    void onTrigger(bool triggerEnter, physx::PxShape *actorShape, physx::PxShape *contactShape) override
    {
        auto actor0 = static_cast<GameObject*>(contactShape->getActor()->userData);
        auto actor1 = static_cast<GameObject*>(actorShape->getActor()->userData);

        if(actor1->getComponent<ShapeComponent>()->getPiece() != nullptr)
        {
            if(triggerEnter)
            {
                GameObjectManager::singleton.getPieceAdministrator()->addScore();                
            }
            //unspawn the piece
            //actor1->getComponent<ShapeComponent>()->getPiece()->destroyPiece();
        }
        else if(actor0->getComponent<ShapeComponent>()->getPiece() != nullptr) //the piece is not actor1, so it is actor0
        {

            //actor0->getComponent<ShapeComponent>()->getPiece()->destroyPiece();
        }
    }
};


void PieceShape::setGameObject(GameObject* parent)
{
    m_gameObject = parent;

    PxTransform parentTransform = physx::PxTransform(PxVec3(
        parent->m_transform.getPosition().vector4_f32[0],
        parent->m_transform.getPosition().vector4_f32[1],
        parent->m_transform.getPosition().vector4_f32[2]));
    m_actor = PhysicsManager::singleton.physics().createRigidDynamic(parentTransform);

    m_shape = m_actor->createShape(physx::PxBoxGeometry(10.f, 25.f, 20.f), *m_material);
    PhysicsManager::singleton.scene().addActor(*m_actor);
    m_shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
    m_shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);

    PxFilterData filterData;
    filterData.word0 = EACTORPIECE;
    filterData.word1 = EACTORVEHICLE;
    m_shape->setSimulationFilterData(filterData);

    PhysicsManager::singleton.registerNewComponent(this);

    setHandler(ICollisionHandlerRef(new CollisionPieceHandler));

    m_actor->setMass(0.0001f);
    m_actor->userData = parent;
}

