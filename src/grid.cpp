#include <flectron/grid.hpp>

namespace flectron
{
  
  SpatialHashGrid::SpatialHashGrid(int cellSize)
    : cells(), cellSize(cellSize), queryIdentifier(0)
  {}

  void SpatialHashGrid::insert(Ref<Body>& body)
  {
    const AABB& aabb = body->getAABB();

    int minX = (int)floor(aabb.min.x / cellSize);
    int minY = (int)floor(aabb.min.y / cellSize);
    int maxX = (int)floor(aabb.max.x / cellSize);
    int maxY = (int)floor(aabb.max.y / cellSize);

    if (clientIndices.count(body) != 0)
    {
      const ClientIndices& indices = clientIndices[body];
      int lastMinX = indices.first.first;
      int lastMinY = indices.first.second;
      int lastMaxX = indices.second.first;
      int lastMaxY = indices.second.second;

      if (lastMinX == minX && lastMinY == minY && lastMaxX == maxX && lastMaxY == maxY)
        return;

      remove(body);
    }

    clientIndices[body] = { {minX, minY}, {maxX, maxY} };
    clientQuery[body] = -1;

    for (int x = minX; x <= maxX; x++)
    {
      for (int y = minY; y <= maxY; y++)
      {
        cells[getHash(x, y)].insert(body);
      }
    }
  }

  std::vector<Ref<Body>> SpatialHashGrid::getCells(const AABB& aabb)
  {
    int minX = (int)floor(aabb.min.x / cellSize);
    int minY = (int)floor(aabb.min.y / cellSize);
    int maxX = (int)floor(aabb.max.x / cellSize);
    int maxY = (int)floor(aabb.max.y / cellSize);

    std::vector<Ref<Body>> result;
    const int id = queryIdentifier++;

    for (int x = minX; x <= maxX; x++)
    {
      for (int y = minY; y <= maxY; y++)
      {
        for (const Ref<Body>& body : cells[getHash(x, y)])
        {
          int& clientQueryId = clientQuery[body];
          if (clientQueryId != id)
          {
            clientQueryId = id;
            result.push_back(body);
          }
        }
      }
    }

    return result;
  }

  void SpatialHashGrid::remove(const Ref<Body>& body)
  {
    const ClientIndices& indices = clientIndices[body];
    int minX = indices.first.first;
    int minY = indices.first.second;
    int maxX = indices.second.first;
    int maxY = indices.second.second;

    for (int x = minX; x <= maxX; x++)
    {
      for (int y = minY; y <= maxY; y++)
      {
        cells[getHash(x, y)].erase(body);
      }
    }

    clientIndices.erase(body);
    clientQuery.erase(body);
  }

}
