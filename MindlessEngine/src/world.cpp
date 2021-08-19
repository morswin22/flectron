#include <MindlessEngine/world.hpp>

#include <MindlessEngine/collisions.hpp>
#include <MindlessEngine/math.hpp>

namespace MindlessEngine 
{

  float World::minBodySize = 0.01f * 0.01f;
  float World::maxBodySize = 64.0f * 64.0f;

  float World::minBodyDensity = 0.5f; // half of water density
  float World::maxBodyDensity = 21.4f; // platinum density

  int World::numCircleVerticies = 64;

  int World::minIterations = 1;
  int World::maxIterations = 128;

  World::World() : bodyList(), gravity(0.0f, -9.81f) {}
  
  void World::addBody(const Ref<Body>& body)
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

  Ref<Body>& World::getBody(int index)
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

  void World::update(float deltaTime, int iterations)
  {
    if (bodyList.empty())
      return;

    iterations = clamp(iterations, minIterations, maxIterations);
    float timeStep = deltaTime / (float)iterations;

    for (int iteration = 0; iteration < iterations; iteration++)
    {
      // Movement
      for (auto it = bodyList.begin(); it != bodyList.end(); ++it)
        (*it)->update(timeStep, gravity);

      // Collision step
      Vector normal;
      float depth;
      auto bodyAPtr = bodyList.begin();
      for (int i = 0; i < bodyList.size() - 1; i++)
      {
        Ref<Body>& bodyA = *bodyAPtr;
        auto bodyBPtr = bodyList.begin();
        std::advance(bodyBPtr, i + 1);
        for (int j = i + 1; j < bodyList.size(); j++)
        {
          Ref<Body>& bodyB = *bodyBPtr;
          if (bodyA->isStatic && bodyB->isStatic)
          {
            std::advance(bodyBPtr, 1);
            continue;
          }

          if (collide(bodyA, bodyB, normal, depth))
          {
            if (bodyA->isStatic)
            {
              bodyB->move(normal * depth);
            }
            else if (bodyB->isStatic)
            {
              bodyA->move(-normal * depth);
            }
            else 
            {
              bodyA->move(-normal * depth * 0.5f);
              bodyB->move(normal * depth * 0.5f);
            }

            resolveCollision(bodyA, bodyB, normal, depth);
          }

          std::advance(bodyBPtr, 1);
        }
        std::advance(bodyAPtr, 1);
      }
    }
  }

  bool World::collide(Ref<Body>& bodyA, Ref<Body>& bodyB, Vector& normal, float& depth)
  {
    if (bodyA->bodyType == BodyType::Box)
    {
      if (bodyB->bodyType == BodyType::Box)
      {
        return intersectPolygons(bodyA->position, bodyA->getTransformedVertices(), bodyA->getNumVertices(), bodyB->position, bodyB->getTransformedVertices(), bodyB->getNumVertices(), normal, depth);
      }
      else if (bodyB->bodyType == BodyType::Circle)
      {
        bool result = intersectCirclePolygon(bodyB->position, bodyB->radius, bodyA->position, bodyA->getTransformedVertices(), bodyA->getNumVertices(), normal, depth);
        if (result)
          normal = -normal;
        return result;
      }
    }
    else if (bodyA->bodyType == BodyType::Circle)
    {
      if (bodyB->bodyType == BodyType::Box)
      {
        return intersectCirclePolygon(bodyA->position, bodyA->radius, bodyB->position, bodyB->getTransformedVertices(), bodyB->getNumVertices(), normal, depth);
      }
      else if (bodyB->bodyType == BodyType::Circle)
      {
        return intersectCircles(bodyA->position, bodyA->radius, bodyB->position, bodyB->radius, normal, depth);
      }
    }
    return false;
  }

  void World::resolveCollision(Ref<Body>& bodyA, Ref<Body>& bodyB, const Vector& normal, float depth)
  {
    Vector relativeVelocity = bodyB->linearVelocity - bodyA->linearVelocity;

    if (dot(relativeVelocity, normal) > 0.0f)
      return;

    float e = std::min(bodyA->resitution, bodyB->resitution);

    float j = -(1.0f + e) * dot(relativeVelocity, normal);

    float invMassSum = bodyA->invMass + bodyB->invMass;
    j /= bodyA->invMass + bodyB->invMass;

    Vector impulse = j * normal;

    bodyA->linearVelocity = bodyA->linearVelocity - (impulse * bodyA->invMass);
    bodyB->linearVelocity = bodyB->linearVelocity + (impulse * bodyB->invMass);
  }
  
};