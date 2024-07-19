#pragma once

#include <vector>
#include <map>
#include "entity.h"

typedef std::vector<std::shared_ptr<Entity>> EntityVec;
typedef std::map<std::string, EntityVec>     EntityMap;

class EntityManager
{
    EntityVec m_entities = {},
              m_toAdd = {};
    EntityMap m_entityMap;
    size_t    m_totalEntities = 0;

public:
    EntityManager();
    void update();
    std::shared_ptr<Entity> addEntity(const std::string& tag);
    const EntityVec& getEntities();
    const EntityVec& getEntities(const std::string& tag);
    void removeDeadEntities(EntityVec& vec);

};

