#include <MindlessEngine/world.hpp>

#include <MindlessEngine/collisions.hpp>

namespace MindlessEngine 
{

  float World::minBodySize = 0.01f * 0.01f;
  float World::maxBodySize = 64.0f * 64.0f;

  float World::minBodyDensity = 0.5f; // half of water density
  float World::maxBodyDensity = 21.4f; // platinum density

  int World::numCircleVerticies = 64;

  World::World() : bodyList(), gravity(0.0f, -9.81f) {}
  
  void World::addBody(const Body& body)
  {
    bodyList.push_back(body);
  }

  bool World::removeBody(int index)
  {
    if (index < 0 || index >= bodyList.size())
      return false;

    auto it = bodyList.begin();
    std::advance(it, index);
    bodyList.erase(it);
    return true;
  }

  Body& World::getBody(int index)
  {
    if (index < 0 || index >= bodyList.size())
      throw std::runtime_error("Index out of bounds");

    auto front = bodyList.begin();
    std::advance(front, index);

    return *front;
  }

  int World::getBodyCount() const
  {
    return bodyList.size();
  }

  void World::update(float deltaTime)
  {
    // Movement
    for (auto it = bodyList.begin(); it != bodyList.end(); ++it)
    {
      it->update(deltaTime);
    }

    // Collision step
    Vector normal;
    float depth;
    auto bodyA = bodyList.begin();
    for (int i = 0; i < bodyList.size() - 1; i++)
    {
      auto bodyB = bodyList.begin();
      std::advance(bodyB, i + 1);
      for (int j = i + 1; j < bodyList.size(); j++)
      {
        if (collide(*bodyA, *bodyB, normal, depth))
        {
          bodyA->move(-normal * depth * 0.5f);
          bodyB->move(normal * depth * 0.5f);
        }

        std::advance(bodyB, 1);
      }
      std::advance(bodyA, 1);
    }
  }

  bool World::collide(Body& bodyA, Body& bodyB, Vector& normal, float& depth)
  {
    if (bodyA.bodyType == BodyType::Box)
    {
      if (bodyB.bodyType == BodyType::Box)
      {
        return intersectPolygons(bodyA.getTransformedVertices(), bodyA.getNumVertices(), bodyB.getTransformedVertices(), bodyB.getNumVertices(), normal, depth);
      }
      else if (bodyB.bodyType == BodyType::Circle)
      {
        bool result = intersectCirclePolygon(bodyB.position, bodyB.radius, bodyA.getTransformedVertices(), bodyA.getNumVertices(), normal, depth);
        if (result)
          normal = -normal;
        return result;
      }
    }
    else if (bodyA.bodyType == BodyType::Circle)
    {
      if (bodyB.bodyType == BodyType::Box)
      {
        return intersectCirclePolygon(bodyA.position, bodyA.radius, bodyB.getTransformedVertices(), bodyB.getNumVertices(), normal, depth);
      }
      else if (bodyB.bodyType == BodyType::Circle)
      {
        return intersectCircles(bodyA.position, bodyA.radius, bodyB.position, bodyB.radius, normal, depth);
      }
    }
    return false;
  }
  
};