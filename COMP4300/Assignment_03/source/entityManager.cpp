#include "entityManager.h"
#include <algorithm>
#include <iostream>

EntityManager::EntityManager() {}

void EntityManager::update()
{
    for (auto e : m_toAdd)
    {
        m_entities.push_back(e);
        m_entityMap[e->tag()].push_back(e);
    }
    m_toAdd.clear();
    removeDeadEntities(m_entities);

    for (auto& i : m_entityMap)
    {
       removeDeadEntities(i.second);
    }

    for (auto& i : m_entityMap)
    {
        if (i.second.empty())
            m_entityMap.erase(i.first);
    }
}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag)
{
    auto entity = std::shared_ptr<Entity>(new Entity(m_totalEntities++, tag));
    m_toAdd.push_back(entity);
    return entity;
}

const EntityVec& EntityManager::getEntities()
{
    return m_entities;
}

const EntityVec& EntityManager::getEntities(const std::string& tag)
{
    return m_entityMap[tag];
}

void EntityManager::removeDeadEntities(EntityVec& vec)
{
    vec.erase(std::remove_if(vec.begin(), vec.end(),
                             [this](std::shared_ptr<Entity> e)
                             { return !e->isActive(); }),
              vec.end());
}
