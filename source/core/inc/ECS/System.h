#pragma once

#include <bitset>
#include <vector>
#include "ComponentMask.h"

namespace Core
{
    namespace ECS
    {
        class World;
        class Entity;

        class System
        {
        private:

        protected:
            // Specifies which components our system cares about — its size should = the number of different components
            ComponentMask signature;

            // These entities fit the systemSignature and should be iterated upon for any functionality
            std::vector<Entity*> registeredEntities;

            // Reference to our parent world
            World* worldObj;

        public:
            // Initialize the System — This happens *before* the game starts but *after* the world has been registered.
            virtual void Init() {};

            virtual void DeInit() {};

            // Called every game update
            virtual void Update(float dt) {};

            // Called every game render
            virtual void Render(float dt) {};

            // Add a reference to the parent world
            void RegisterWorld(World* world);

            // This entity fits our current requirements
            virtual void RegisterEntity(Entity* entity);

            // This entity has stopped fitting our current requirements
            virtual void UnRegisterEntity(Entity* entity);
            ComponentMask GetSignature() { return signature; }

            const std::vector<Entity*>& GetRegisteredEntities() const
            {
                return registeredEntities;
            }
        };
    }
}