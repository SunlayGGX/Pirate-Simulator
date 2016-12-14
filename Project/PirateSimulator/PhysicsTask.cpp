#include "PhysicsTask.h"
#include "GameObjectManager.h"
#include "RendererManager.h"
#include "LightManager.h"
#include "PhysicsManager.h"

using namespace PirateSimulator;

void PhysicsTask::init()
{
    PhysicsManager::singleton.initialize();
    GameObjectManager::singleton.init();
    
}

void PhysicsTask::update()
{
    float elapsedTime = TimeManager::GetInstance().getElapsedTimeFrame();

    // On pr�pare la prochaine image
    GameObjectManager::singleton.animAllGameObject(elapsedTime);

    LightManager::singleton.update(elapsedTime);
	
    PhysicsManager::singleton.update();
}

void PhysicsTask::cleanup()
{
    GameObjectManager::singleton.reset();
    PhysicsManager::singleton.reset();
}
