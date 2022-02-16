#include <flectron.hpp>
#include <iostream>

using namespace flectron;

class Terrain : public Game
{
private:
  Ref<WFC::TileInfo> tileInfo;
  Ref<WFC::TileModel> generator;
  Ref<WFC::Output> output;
  Stopwatch timer;

public:
  Terrain()
  : Game(500, 500, "Terrain test", "shaders/batch.vert", "shaders/batch.frag"),
    tileInfo(createRef<WFC::TileInfo>("assets/pipes.txt")),
    generator(createRef<WFC::TileModel>(tileInfo, "assets/pipes.png", 5, 25, 25, true)),
    output(generator->createOutput())
  {
    timer.start();
    generator->run(output, 0u, 0u);
    timer.stop();
    std::cout << "Time: " << timer.getElapsedTime() << "\n";

    // scene.createScript("Update", [&]() {
    //   generator->run(output, 0u, 25u);
    // });

    scene.createScript("Renderer", [&]() {
      const auto& tiles = generator->getTiles(output);

      const float tileSize = 20.0f;
      const float xOffset = -((float)generator->width*0.5f);
      const float yOffset = -((float)generator->height*0.5f);

      for (size_t y = 0; y < generator->height; ++y)
        for (size_t x = 0; x < generator->width; ++x)
          window.draw({(x + xOffset) * tileSize, (y + yOffset) * tileSize}, tileSize, tiles[x][y], Colors::white());
    }, FLECTRON_RENDER);
  }
};

Game* flectron::createGame()
{
  return new Terrain();
}
