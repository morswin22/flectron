#include <flectron/scene/grid.hpp>

namespace flectron
{

  SpatialHashGrid::SpatialHashGrid(int cellSize, entt::registry& registry)
    : cells(), cellSize(cellSize), queryIdentifier(0), registry(registry)
  {}

  void SpatialHashGrid::insert(entt::entity entity)
  {
    const AABB& aabb = registry.get<VertexComponent>(entity).getAABB(registry.get<PositionComponent>(entity));

    int minX = (int)floor(aabb.min.x / cellSize);
    int minY = (int)floor(aabb.min.y / cellSize);
    int maxX = (int)floor(aabb.max.x / cellSize);
    int maxY = (int)floor(aabb.max.y / cellSize);

    if (registry.all_of<SpatialHashGridComponent>(entity))
    {
      auto& shgc = registry.get<SpatialHashGridComponent>(entity);
      int lastMinX = shgc.clientIndices.first.first;
      int lastMinY = shgc.clientIndices.first.second;
      int lastMaxX = shgc.clientIndices.second.first;
      int lastMaxY = shgc.clientIndices.second.second;

      if (lastMinX == minX && lastMinY == minY && lastMaxX == maxX && lastMaxY == maxY)
        return;

      remove(entity);
    }

    registry.emplace_or_replace<SpatialHashGridComponent>(entity, std::make_pair<std::pair<int, int>, std::pair<int, int>>({minX, minY}, {maxX, maxY}), -1);

    for (int x = minX; x <= maxX; x++)
      for (int y = minY; y <= maxY; y++)
        cells[getHash(x, y)].insert(entity);
  }

  std::vector<entt::entity> SpatialHashGrid::getCells(const AABB& aabb)
  {
    int minX = (int)floor(aabb.min.x / cellSize);
    int minY = (int)floor(aabb.min.y / cellSize);
    int maxX = (int)floor(aabb.max.x / cellSize);
    int maxY = (int)floor(aabb.max.y / cellSize);

    std::vector<entt::entity> result;
    const int id = queryIdentifier++;

    for (int x = minX; x <= maxX; x++)
    {
      for (int y = minY; y <= maxY; y++)
      {
        for (entt::entity entity : cells[getHash(x, y)])
        {
          auto& shgc = registry.get<SpatialHashGridComponent>(entity);
          if (shgc.clientQuery != id)
          {
            shgc.clientQuery = id;
            result.push_back(entity);
          }
        }
      }
    }

    return result;
  }

  void SpatialHashGrid::remove(entt::entity entity)
  {
    auto& shgc = registry.get<SpatialHashGridComponent>(entity);
    int minX = shgc.clientIndices.first.first;
    int minY = shgc.clientIndices.first.second;
    int maxX = shgc.clientIndices.second.first;
    int maxY = shgc.clientIndices.second.second;

    for (int x = minX; x <= maxX; x++)
    {
      for (int y = minY; y <= maxY; y++)
      {
        cells[getHash(x, y)].erase(entity);
      }
    }

    registry.remove<SpatialHashGridComponent>(entity);
  }

}