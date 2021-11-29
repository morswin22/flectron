#include <flectron/flectron.hpp>
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
  Terrain();
  void update() override;
  void render() override;
};

int main(void)
{
  Terrain test;
  test.run();

  return 0;
}

Terrain::Terrain()
  : Game(500, 500, "Terrain test", "shaders/batch.vert", "shaders/batch.frag"),
    tileInfo(createRef<WFC::TileInfo>("assets/pipes.txt")),
    generator(createRef<WFC::TileModel>(tileInfo, "assets/pipes.png", 5, 25, 25, true)),
    output(generator->createOutput())
{
  timer.start();
  generator->run(output, 0u, 0u);
  timer.stop();
  std::cout << "Time: " << timer.getElapsedTime() << "\n";
}

void Terrain::update() 
{
  // generator->run(output.get(), 0u, 25u);
}

void Terrain::render()
{
  window.clear();

  const auto& tiles = generator->getTiles(output);

  const float tileSize = 20.0f;
  const float xOffset = -((float)generator->width*0.5f);
  const float yOffset = -((float)generator->height*0.5f);

  for (size_t y = 0; y < generator->height; ++y)
    for (size_t x = 0; x < generator->width; ++x)
      window.draw({(x + xOffset) * tileSize, (y + yOffset) * tileSize}, tileSize, tiles[x][y], Colors::white());
}
