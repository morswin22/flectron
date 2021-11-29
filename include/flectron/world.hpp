#pragma once

#include <list>
#include <flectron/memory.hpp>
#include <flectron/body.hpp>
#include <flectron/grid.hpp>

namespace flectron 
{

  class World
  {
  public:
    static float minBodySize;
    static float maxBodySize;

    static float minBodyDensity; // g/cm^3
    static float maxBodyDensity;

    static int numCircleVerticies;

    static int minIterations;
    static int maxIterations;

  private:
    std::list<Ref<Body>> bodyList;
  public:
    SpatialHashGrid grid;
  private:
    Vector gravity;

  public:
    World();

    void addBody(Ref<Body>& body);
    bool removeBody(size_t index);
    Ref<Body>& getBody(size_t index);
    size_t getBodyCount() const;

    void update(float deltaTime, int iterations);

    bool collide(Ref<Body>& bodyA, Ref<Body>& bodyB, Vector& normal, float& depth);
    void resolveCollision(Ref<Body>& bodyA, Ref<Body>& bodyB, const Vector& normal, float depth);

  };

}
