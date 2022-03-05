#pragma once

#include <glm/glm.hpp>
#include <flectron/physics/vector.hpp>

namespace flectron
{

  struct Constraints
  {
    float left;
    float right;
    float top;
    float bottom;

    Constraints(float left, float right, float top, float bottom);
  };

  Constraints operator+(const Constraints& constraints, const glm::vec3& offset);
  
  class Camera
  {
  private:
    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;
    glm::mat4 viewProjectionMatrix;

    glm::vec3 position;
    float rotation;
  
    Constraints constraints;
    float scale;

    void recalculateViewMatrix();

  public:
    Camera(float left, float right, float bottom, float top);

    const glm::vec3& getPosition() const;
    float getRotation() const;

    void setProjection(float left, float right, float bottom, float top);
    void setPosition(const glm::vec3& position);
    void setRotation(float rotation);
    void setScale(float scale);

    void moveTo(const Vector& position);
    void move(const Vector& amount);

    const glm::mat4& getProjectionMatrix() const;
    const glm::mat4& getViewMatrix() const;
    const glm::mat4& getViewProjectionMatrix() const;

    Constraints getConstraints() const;
    float getScale() const;

    void handleWASD();
    void handleScroll();
  };

}
