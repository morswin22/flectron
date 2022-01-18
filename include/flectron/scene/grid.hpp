#pragma once

#include <unordered_set>
#include <unordered_map>
#include <entt/entt.hpp>
#include <flectron/physics/aabb.hpp>
#include <flectron/scene/components.hpp>

namespace flectron 
{

  using ULL = unsigned long long;
  using UL = unsigned long;
  using ClientIndices = std::pair<std::pair<int,int>, std::pair<int,int>>;

  class SpatialHashGrid
  {
  private:
    std::unordered_map<ULL, std::unordered_set<entt::entity>> cells;
    int cellSize;
    int queryIdentifier;
    entt::registry& registry; // TODO try to seperate this from the scene

  public:
    SpatialHashGrid(int cellSize, entt::registry& registry);

    void insert(entt::entity entity);
    void remove(entt::entity entity);
    
    std::vector<entt::entity> getCells(const AABB& aabb);

  private:
    inline ULL getHash(int a, int b) { return (ULL)a << 32 | (UL)b; };
  };

}
