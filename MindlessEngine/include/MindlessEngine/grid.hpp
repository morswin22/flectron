#pragma once

#include <MindlessEngine/body.hpp>
#include <unordered_set>
#include <string>

namespace MindlessEngine 
{

  using ULL = unsigned long long;
  using UL = unsigned long;

  class SpatialHashGrid 
  {
  private:
    std::unordered_map<ULL, std::unordered_set<Ref<Body>>> cells;
    std::unordered_map<Ref<Body>, std::pair<std::pair<int,int>, std::pair<int,int>>> cellIndices;
    int cellSize;

  public:
    SpatialHashGrid(int cellSize);

    void insert(Ref<Body>& body);
    void remove(const Ref<Body>& body);
    
    std::unordered_set<Ref<Body>> getCells(const AABB& aabb);

  private:
    inline ULL getHash(int a, int b) { return (ULL)a << 32 | (UL)b; };
  };

};
