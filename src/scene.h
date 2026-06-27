#pragma once
#include "camera.h"
#include "entity.h"

#include <vector>
#include <utility>

class Scene {
public:
    Scene(const Camera& camera)
        : m_camera(camera)
    {}

    void addEntity(Entity&& entity)
    {
        m_entities.push_back(std::move(entity));
    }

    std::vector<Entity>& getEntities()
    {
        return m_entities;
    }

    const std::vector<Entity>& getEntities() const
    {
        return m_entities;
    }

    Camera& getCamera()
    {
        return m_camera;
    }

    const Camera& getCamera() const
    {
        return m_camera;
    }

private:
    std::vector<Entity> m_entities;
    Camera m_camera;
};
