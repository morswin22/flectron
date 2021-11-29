#include <flectron/grid.hpp>

namespace flectron
{
  
  SpatialHashGrid::SpatialHashGrid(int cellSize)
    : cells(), cellSize(cellSize)
  {}

  void SpatialHashGrid::insert(Ref<Body>& body)
  {
    const AABB& aabb = body->getAABB();

    int minX = (int)floor(aabb.min.x / cellSize);
    int minY = (int)floor(aabb.min.y / cellSize);
    int maxX = (int)floor(aabb.max.x / cellSize);
    int maxY = (int)floor(aabb.max.y / cellSize);

    if (cellIndices.count(body) != 0)
    {
      int lastMinX = cellIndices[body].first.first;
      int lastMinY = cellIndices[body].first.second;
      int lastMaxX = cellIndices[body].second.first;
      int lastMaxY = cellIndices[body].second.second;

      if (lastMinX == minX && lastMinY == minY && lastMaxX == maxX && lastMaxY == maxY)
        return;

      remove(body);
    }

    cellIndices[body] = { {minX, minY}, {maxX, maxY} };

    for (int x = minX; x <= maxX; x++)
    {
      for (int y = minY; y <= maxY; y++)
      {
        cells[getHash(x, y)].insert(body);
      }
    }
  }

  std::unordered_set<Ref<Body>> SpatialHashGrid::getCells(const AABB& aabb)
  {
    int minX = (int)floor(aabb.min.x / cellSize);
    int minY = (int)floor(aabb.min.y / cellSize);
    int maxX = (int)floor(aabb.max.x / cellSize);
    int maxY = (int)floor(aabb.max.y / cellSize);

    std::unordered_set<Ref<Body>> result;
    for (int x = minX; x <= maxX; x++)
    {
      for (int y = minY; y <= maxY; y++)
      {
        result.insert(cells[getHash(x, y)].begin(), cells[getHash(x, y)].end());
      }
    }

    return result;
  }

  void SpatialHashGrid::remove(const Ref<Body>& body)
  {
    int minX = cellIndices[body].first.first;
    int minY = cellIndices[body].first.second;
    int maxX = cellIndices[body].second.first;
    int maxY = cellIndices[body].second.second;

    for (int x = minX; x <= maxX; x++)
    {
      for (int y = minY; y <= maxY; y++)
      {
        cells[getHash(x, y)].erase(body);
      }
    }

    cellIndices.erase(body);
  }

}
