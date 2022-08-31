#include <flectron.hpp>
#include <flectron/application/entry.hpp>
#include <iostream>

FLECTRON_EMBED(PIPES_TXT);
FLECTRON_EMBED(PIPES_PNG);

using namespace flectron;

class TerrainLayer : public Layer
{
private:
  Image pipesImage;
  Text pipesText;
  Ref<WFC::TileInfo> tileInfo;
  Ref<WFC::TileModel> generator;
  Ref<WFC::Output> output;
  Stopwatch timer;

public:
  TerrainLayer(Application& application)
  : Layer(application),
    pipesImage(Image::fromEmbed(PIPES_PNG())),
    pipesText(Text::fromEmbed(PIPES_TXT())),
    tileInfo(nullptr),
    generator(nullptr),
    output(nullptr)
  {
  }

  void setup() override
  {
    pipesImage.load();
    pipesImage.loadGPU();
    pipesImage.unload();

    pipesText.load();

    tileInfo = createRef<WFC::TileInfo>(pipesText);
    generator = createRef<WFC::TileModel>(tileInfo, pipesImage, 5, 25, 25, true);
    output = generator->createOutput();

    timer.start();
    generator->run(output, 0u, 0u);
    timer.stop();
    FLECTRON_LOG_INFO("Elapsed time: {}", timer.getElapsedTime());
  }

  void update() override
  {
    const auto& tiles = generator->getTiles(output);

    const float tileSize = 20.0f;
    const float xOffset = -((float)generator->width*0.5f);
    const float yOffset = -((float)generator->height*0.5f);

    for (size_t y = 0; y < generator->height; ++y)
      for (size_t x = 0; x < generator->width; ++x)
        tiles[x][y]->render({(x + xOffset) * tileSize, (y + yOffset) * tileSize}, tileSize);
  }

  void cleanup() override
  {
    pipesImage.unloadGPU();
    pipesText.unload();
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
