#include <iostream>
#include <list>
#include <memory>

#include <MindlessEngine/MindlessEngine.hpp>

using namespace MindlessEngine;

class Mindless : public Game
{
private:
  std::unique_ptr<Shader> basicShader;
  std::list<Body> bodyList;

public:
  Mindless() : Game(), basicShader(nullptr), bodyList()
  {
    window.setTitle("Mindless");
    window.setScale(15.0f);

    basicShader = std::make_unique<Shader>("../MindlessEngine/shaders/basic.vert", "../MindlessEngine/shaders/basic.frag");

    int bodyCount = 10;
    float padding = 2.0f;

    float left, top, right, bottom;
    window.getCameraConstrains(&left, &top, &right, &bottom);

    for (int i = 0; i < bodyCount; i++)
    {
      BodyType type = static_cast<BodyType>(randomInt(0, 2));

      float x = randomFloat(left + padding, right - padding);
      float y = randomFloat(bottom + padding, top - padding);

      if (type == BodyType::Circle)
      {
        // TODO fix circle body creation
        // bodyList.push_back(createCircleBody(3.0f, { x, y }, 2.0f, 0.5f, false));
      }
      else if (type == BodyType::Box)
      {
        bodyList.push_back(createBoxBody(1.77f, 1.77f, { x, y }, 2.0f, 0.5f, false));
      }
      else
        throw std::invalid_argument("Invalid body type");
    }
  }

  void update(float elapsedTime) override
  {
    std::cout << "Elapsed time: " << elapsedTime * 1000.0f << "ms" << std::endl;
    for (Body& body : bodyList)
    {
      body.rotate(M_PI * elapsedTime);
    }
  }

  void render() override
  {
    basicShader->bind();
    basicShader->setUniform4f("uColor", 1.0f, 0.5f, 1.0f, 1.0f);
    basicShader->setUniformMat4f("uMVP", window.getProjectionMatrix());
    
    for (Body& body : bodyList)
    {
      draw(*body.getVertexArray(), *body.getIndexBuffer());
    }
  }
};

int main(void)
{
  Mindless mindless;
  mindless.run();

  return 0;
}