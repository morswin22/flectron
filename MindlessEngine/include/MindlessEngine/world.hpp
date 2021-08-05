#pragma once

#include <list>
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

  private:
    std::list<Body> bodyList;
    Vector gravity;

  public:
    World();

    void addBody(const Body& body);
    bool removeBody(int index);
    Body& getBody(int index);
    int getBodyCount() const;

    void update(float deltaTime);

    bool collide(Body& bodyA, Body& bodyB, Vector& normal, float& depth);
    void resolveCollision(Body& bodyA, Body& bodyB, const Vector& normal, float depth);

  };

};