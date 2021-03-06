/*
Created by Sun-lay Gagneux
*/
#ifndef GAMEOBJECT_H_INCLUDED
#define GAMEOBJECT_H_INCLUDED


#include "Transform.h"
#include "IBehaviour.h"
#include "Mesh.h"

#include <vector>
#include <string>
#include <memory>


namespace PirateSimulator
{
    class GameObject;

    using ComponentRef = std::shared_ptr<Component>;
    using GameObjectRef = std::shared_ptr<GameObject>;

    class GameObject
    {
    private:
        void (GameObject::* m_pSetMatrix)(const DirectX::XMMATRIX& world);
        void (GameObject::* m_pAnim)(float elapsedTime);


    public:
        Transform m_transform;
        std::string m_name;


    protected:
        std::vector<ComponentRef> m_attachedComponent;

        IBehaviour* m_behaviour;
        IMesh* m_mesh;


    public:
        GameObject(const Transform& transform, const std::string& name);


    public:
        template<class ComponentAttribute>
        void addComponent(ComponentAttribute* component)
        {
            static_assert(std::is_convertible<ComponentAttribute*, Component*>::value &&
                !std::is_null_pointer<ComponentAttribute*>::value, "You want to attach a component that is not!");

            if (component)
            {
                m_attachedComponent.push_back(ComponentRef(component));
                m_attachedComponent[m_attachedComponent.size() - 1]->setGameObject(this);
            }
        }

        template<>
        void addComponent<IBehaviour>(IBehaviour* component)
        {
            if (component)
            {
                m_attachedComponent[0] = ComponentRef(component);
                m_behaviour = component;
                m_behaviour->setGameObject(this);
                m_pAnim = &GameObject::animSomething;
            }
        }

        template<>
        void addComponent<IMesh>(IMesh* component)
        {
            if (component)
            {
                m_attachedComponent.push_back(ComponentRef(component));
                m_mesh = component;
                m_mesh->setGameObject(this);
                m_pSetMatrix = &GameObject::setWorldMatrixWhenHaving;
            }
        }

        template<class ComponentAttribute>
        ComponentAttribute* getComponent()
        {
            static_assert(std::is_convertible<ComponentAttribute*, Component*>::value, "You want to get something that is not component!");

            size_t size = m_attachedComponent.size();
            if (size > 0 && size < 0xFFF) //4095 is a huge number. just a dirty bug fix
            {
                for (auto iter = m_attachedComponent.begin(); iter != m_attachedComponent.end(); ++iter)
                {
                    if (Component::sameTypeIdAs((*iter)->getTypeId(), ComponentAttribute::typeId()))
                    {
                        return (*iter)->as<ComponentAttribute>();
                    }
                }
            }

            return nullptr;
        }

        template<>
        IBehaviour* getComponent<IBehaviour>()
        {
            return m_behaviour;
        }

        template<>
        IMesh* getComponent<IMesh>()
        {
            return m_mesh;
        }


        const std::string& getName() const noexcept
        {
            return m_name;
        }

        void setName(const std::string& newName)
        {
            m_name = newName;
        }

        bool compareName(const std::string& newName)
        {
            return m_name.size() == newName.size() && m_name == newName;
        }


        virtual void anime(float elapsedTime)
        {
            (this->*m_pAnim)(elapsedTime);
        }
        void draw()
        {
            m_mesh->Draw();
        }

        void setPosition(float x, float y, float z);
        void setPosition(const DirectX::XMVECTOR &newPos);
        void translate(float x, float y, float z);
        void translate(const DirectX::XMVECTOR &dir);
        void rotate(float angle, const DirectX::XMVECTOR &axis);

        void setWorldMatrix(const DirectX::XMMATRIX& world)
        {
            (this->*m_pSetMatrix)(world);
        }

        void cleanUp();

    private:
        void setWorldMatrixWhenHaving(const DirectX::XMMATRIX& world)
        {
            m_mesh->setWorldMatrix(world);
        }

        void setWorldMatrixWhenNotHavingAMesh(const DirectX::XMMATRIX& world)
        {}

        void animSomething(float elapsedTime)
        {
            m_behaviour->anime(elapsedTime);
        }

        void animNothing(float elapsedTime)
        {}
    };
}


#endif //!GAMEOBJECT_H_INCLUDED