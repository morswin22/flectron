#pragma once

#include <flectron/body.hpp>
#include <unordered_set>
#include <string>

namespace flectron 
{

  using ULL = unsigned long long;
  using UL = unsigned long;
  using ClientIndices = std::pair<std::pair<int,int>, std::pair<int,int>>;

  class SpatialHashGrid 
  {
  private:
    std::unordered_map<ULL, std::unordered_set<Ref<Body>>> cells;
    std::unordered_map<Ref<Body>, ClientIndices> clientIndices;
    std::unordered_map<Ref<Body>, int> clientQuery;
    int cellSize;
    int queryIdentifier;

  public:
    SpatialHashGrid(int cellSize);

    void insert(Ref<Body>& body);
    void remove(const Ref<Body>& body);
    
    std::vector<Ref<Body>> getCells(const AABB& aabb);

  private:
    inline ULL getHash(int a, int b) { return (ULL)a << 32 | (UL)b; };
  };

}
