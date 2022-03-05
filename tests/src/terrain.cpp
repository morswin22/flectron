#include <flectron.hpp>
#include <iostream>

using namespace flectron;

class TerrainLayer : public Layer
{
private:
  Ref<WFC::TileInfo> tileInfo;
  Ref<WFC::TileModel> generator;
  Ref<WFC::Output> output;
  Stopwatch timer;

public:
  TerrainLayer(Application& application)
  : Layer(application),
    tileInfo(createRef<WFC::TileInfo>("assets/pipes.txt")),
    generator(createRef<WFC::TileModel>(tileInfo, "assets/pipes.png", 5, 25, 25, true)),
    output(generator->createOutput())
  {
    timer.start();
    generator->run(output, 0u, 0u);
    timer.stop();
    std::cout << "Time: " << timer.getElapsedTime() << "\n";
  }

  void update() override
  {
    const auto& tiles = generator->getTiles(output);

    const float tileSize = 20.0f;
    const float xOffset = -((float)generator->width*0.5f);
    const float yOffset = -((float)generator->height*0.5f);

    for (size_t y = 0; y < generator->height; ++y)
      for (size_t x = 0; x < generator->width; ++x)
        application.window.draw({(x + xOffset) * tileSize, (y + yOffset) * tileSize}, tileSize, tiles[x][y], Colors::white());
  }
};

class TerrainApplication : public Application
{
public:
  TerrainApplication() : Application({}, { "Terrain test", 500, 500, VSYNC })
  {
    pushLayer<TerrainLayer>();
  }
};

Scope<Application> flectron::createApplication(const ApplicationArguments&)
{
  return createScope<TerrainApplication>();
}
