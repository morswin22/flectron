#pragma once

#include <list>
#include <MindlessEngine/memory.hpp>
#include <MindlessEngine/body.hpp>

namespace MindlessEngine 
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
    Vector gravity;

  public:
    World();

    void addBody(const Ref<Body>& body);
    bool removeBody(int index);
    Ref<Body>& getBody(int index);
    int getBodyCount() const;

    void update(float deltaTime, int iterations);

    bool collide(Ref<Body>& bodyA, Ref<Body>& bodyB, Vector& normal, float& depth);
    void resolveCollision(Ref<Body>& bodyA, Ref<Body>& bodyB, const Vector& normal, float depth);

  };

};