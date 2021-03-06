#include "TerrainShape.h"
#include "ICollisionHandler.h"
#include "GameObjectManager.h"
#include "SoundManager.h"
#include "TimeManager.h"
#include "Terrain.h"
#include "../PetitMoteur3D/PetitMoteur3D/Config/Config.hpp"

using namespace physx;


namespace PirateSimulator
{
    class CollisionTerrainHandler: public ICollisionHandler
    {

        void onContact(const physx::PxContactPair &aContactPair) override
        {
            
            PirateSimulator::SoundManager::singleton.playMusic("PirateSimulator/Collision1Noise.wav");
            TimeManager::GetInstance().increaseTime(-5s);
        }

        void onTrigger(bool triggerEnter, physx::PxShape *actorShape, physx::PxShape *contactShape) override
        {
            

        }
    };

    void TerrainShape::setGameObject(GameObject* parent)
    {
   
        //Get Terrain Data
        Terrain* terrainData = GameObjectManager::singleton.getGameObjectByName("terrain")->getComponent<Terrain>();
        int width = terrainData->getWidth();
        int height = terrainData->getHeight();
        float scale = terrainData->getScale();
        auto config = Config::getInstance();
        int verticeCount = width*height;
        std::vector<CSommetBloc> vertexArray = terrainData->getSommetsArray();

        //Get Terrain Points
        
        _heightMap = std::make_unique<physx::PxHeightFieldSample[]>(height*width);

        for (int z = 0; z < height; ++z)
        {
            for (int x = 0; x < width; ++x)
            {
                CSommetBloc ptTerrain = terrainData->getSommetsArray()[z*width + x];
                float getPointy = vertexArray.at(z*width+x).getPosition().y;
                _heightMap[x*height + z].height = getPointy;
                _heightMap[x*height + z].materialIndex0 = 0;
                _heightMap[x*height + z].materialIndex1 = 0;
                _heightMap[x*height + z].clearTessFlag();
            }
        }
  
        //HeightFieldData
        
        PxHeightFieldDesc heightMapDesc;
        heightMapDesc.format = PxHeightFieldFormat::eS16_TM;
        heightMapDesc.nbColumns = width;
        heightMapDesc.nbRows = height;
        heightMapDesc.samples.data = _heightMap.get();
        heightMapDesc.samples.stride = sizeof(PxHeightFieldSample);
        
         _heightField = physx::unique_ptr<physx::PxHeightField>(
            PhysicsManager::singleton.physics().createHeightField(heightMapDesc));

                
		//Cr�ation Shape

        PxRigidStatic &m_actor = *PhysicsManager::singleton.physics().createRigidStatic(parent->m_transform.getPose());
        
        PxShape *_shape = m_actor.createShape(PxHeightFieldGeometry(_heightField.get(), PxMeshGeometryFlag::eDOUBLE_SIDED, scale, scale, scale),
             *m_material);
         m_shape = _shape;
       
        PxFilterData filterData;

        filterData.word0 = EACTORTERRAIN;
        filterData.word1 = EACTORVEHICLE;
        m_shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
        
        m_shape->setSimulationFilterData(filterData);
        PhysicsManager::singleton.scene().addActor(m_actor);
        
        //Register shape
        setHandler(ICollisionHandlerRef(new CollisionTerrainHandler));
        m_actor.userData = parent;
        PhysicsManager::singleton.registerNewComponent(this);
        m_gameObject = parent;        
    }

}