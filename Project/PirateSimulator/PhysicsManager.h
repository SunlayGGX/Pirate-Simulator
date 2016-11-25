#ifndef PHYSICS_MANAGER_H
#define PHYSICS_MANAGER_H
#include "../PetitMoteur3D/PetitMoteur3D/PhysX/Include/PxPhysicsAPI.forward.h"
#include <memory>
#include <vector>

namespace physx
{
    class PxFoundation;
    class PxProfileZoneManager;
}

namespace PirateSimulator
{
    class ShapeComponent;

    class PhysicsManager
    {
    public:
        static PhysicsManager singleton;


    private:
        physx::unique_ptr<physx::PxFoundation> _foundation = nullptr;
        physx::unique_ptr<physx::PxProfileZoneManager> _profileZoneManager = nullptr;
        physx::unique_ptr<physx::PxPhysics> _physics = nullptr;
        physx::unique_ptr<physx::PxScene> _scene = nullptr;
        physx::unique_ptr<physx::PxDefaultCpuDispatcher> _cpuDispatcher;
        physx::unique_ptr<physx::PxCudaContextManager> _cudaContextManager;
        physx::unique_ptr<physx::debugger::comm::PvdConnection> _visualDebuggerConnection;
        std::vector<ShapeComponent*> m_components;
    private:
        PhysicsManager() :_foundation(nullptr), _profileZoneManager(nullptr), _physics(nullptr),
            _scene(nullptr), _cpuDispatcher(nullptr), _cudaContextManager(nullptr),
            _visualDebuggerConnection(nullptr)
        {

        }
        PhysicsManager(const PhysicsManager&) = delete;
        PhysicsManager& operator=(const PhysicsManager&) = delete;


    public:
        virtual physx::PxPhysics& physics();
        virtual physx::PxScene& scene();

    public:
        void update();
        void initialize();
        void registerNewComponent(ShapeComponent* component)
        {
            m_components.push_back(component);
        }
        void reset()
        {
            for (auto shape : m_components)
            {
                shape->cleanUp();
                shape->~ShapeComponent();
                //m_components.pop_back();
            }
            m_components.clear();
            _foundation = nullptr;
            _profileZoneManager = nullptr;
            _physics = nullptr;
            _scene = nullptr;
            _cpuDispatcher = nullptr;
            _cudaContextManager = nullptr;
            _visualDebuggerConnection = nullptr;
        }
    };
}

#endif

