#include <flectron/application/camera.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <flectron/utils/input.hpp>
#include <flectron/physics/math.hpp>

namespace flectron
{

  Constraints::Constraints(float left, float right, float top, float bottom)
    : left(left), right(right), top(top), bottom(bottom)
  {}

  Constraints operator+(const Constraints& constraints, const glm::vec3& offset)
  {
    return { constraints.left + offset.x, constraints.right + offset.x, constraints.top + offset.y, constraints.bottom + offset.y };
  }

  Camera::Camera(float left, float right, float bottom, float top)
    : projectionMatrix(glm::ortho(left, right, bottom, top, -1.0f, 1.0f)), viewMatrix(1.0f), position(0.0f, 0.0f, 0.0f), rotation(0.0f),
      constraints(left, right, bottom, top), scale(1.0f)
  {
    viewProjectionMatrix = projectionMatrix * viewMatrix;
  }

  void Camera::recalculateViewMatrix()
  {
    glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);
    transform = glm::rotate(transform, rotation, glm::vec3(0.0, 0.0, 1.0f));

    viewMatrix = glm::inverse(transform);
    viewProjectionMatrix = projectionMatrix * viewMatrix;
  }

  const glm::vec3& Camera::getPosition() const
  {
    return position;
  }

  float Camera::getRotation() const
  {
    return rotation;
  }

  void Camera::setProjection(float left, float right, float bottom, float top)
  {
    projectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
    viewProjectionMatrix = projectionMatrix * viewMatrix;
    constraints = { left, right, bottom, top };
  }

  void Camera::setPosition(const glm::vec3& position)
  {
    this->position = position;
    recalculateViewMatrix();
  }

  void Camera::setRotation(float rotation)
  {
    this->rotation = rotation;
    recalculateViewMatrix();
  }

  void Camera::setScale(float scale)
  {
    const float scaleFactor = scale / this->scale;
    setProjection(constraints.left * scaleFactor, constraints.right * scaleFactor, constraints.top * scaleFactor, constraints.bottom * scaleFactor);
    this->scale = scale;
  }

  void Camera::moveTo(const Vector& position)
  {
    this->position = { position.x, position.y, 0.0f };
    recalculateViewMatrix();
  }

  void Camera::move(const Vector& amount)
  {
    position += glm::vec3(amount.x, amount.y, 0.0f);
    recalculateViewMatrix();
  }

  const glm::mat4& Camera::getProjectionMatrix() const
  {
    return projectionMatrix;
  }

  const glm::mat4& Camera::getViewMatrix() const
  {
    return viewMatrix;
  }

  const glm::mat4& Camera::getViewProjectionMatrix() const
  {
    return viewProjectionMatrix;
  }

  Constraints Camera::getConstraints() const
  {
    return constraints + position;
  }

  float Camera::getScale() const
  {
    return scale;
  }

  void Camera::handleWASD()
  {
    float dx = 0.0f;
    float dy = 0.0f;

    if (Keyboard::isPressed(Key::W))
      dy++;
    if (Keyboard::isPressed(Key::S))
      dy--;
    if (Keyboard::isPressed(Key::A))
      dx--;
    if (Keyboard::isPressed(Key::D))
      dx++;

    if (dx != 0.0f || dy != 0.0f)
      move(normalize({ dx, dy }) * scale * 8.0f);
  }

  void Camera::handleScroll()
  {
    const float scrollY = Mouse::getScrollY();
    if (scrollY != 0.0f)
    {
      const float amount = (scrollY > 0.0f) ? 0.8f : 1.25f;
      setScale(scale * amount);
    }
  }

}
