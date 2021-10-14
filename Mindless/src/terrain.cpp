#include <MindlessEngine/MindlessEngine.hpp>
#include <iostream>

using namespace MindlessEngine;

class Terrain : public Game
{
private:
  Ref<WFC::TileInfo> tileInfo;
  Ref<WFC::TileModel> generator;
  Scope<WFC::Output> output;
  GLuint texture;
  Stopwatch timer;
public:
  Terrain()
  : Game(500, 500, "Terrain test", "shaders/batch.vert", "shaders/batch.frag"),
    tileInfo(createRef<WFC::TileInfo>("assets/pipes.txt")),
    generator(createRef<WFC::TileModel>(tileInfo, "assets/pipes.png", 5, 25, 25, true)),
    output(generator->createOutput()),
    texture(generator->getTexture())
  {
    timer.start();
    generator->run(output.get(), 0u, 0u);
    timer.stop();
    std::cout << "Time: " << timer.getElapsedTime() << "\n";
  }

  void update() override
  {
    // generator->run(output.get(), 0u, 25u);
  }

  void render() override
  {
    window.clear();

    const auto& tiles = generator->getTiles(output.get());

    const float w = 500.0f;
    const float h = 500.0f;
    const float hw = w / generator->width;
    const float hh = h / generator->height;
    for (int y = 0; y < generator->height; ++y)
    {
      for (int x = 0; x < generator->width; ++x)
      {
        const float u = (float)(x - generator->width*0.5f) / generator->width;
        const float v = (float)(y - generator->height*0.5f) / generator->height;
        const float x1 = u * w;
        const float y1 = v * h;

        const glm::vec4& ab = tiles[x][y]->ab;
        const glm::vec4& cd = tiles[x][y]->cd; 

        Renderer::draw(
          {x1 + ab.x * hw, y1 + ab.y * hh},
          {x1 + ab.z * hw, y1 + ab.w * hh},
          {x1 + cd.x * hw, y1 + cd.y * hh},
          {x1 + cd.z * hw, y1 + cd.w * hh},
          texture,
          tiles[x][y]->textureCoords,
          Colors::white()
        );
      }
    }
  }
};

int main(void)
{
  Terrain test;
  test.run();

  return 0;
}