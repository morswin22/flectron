#include <MindlessEngine/MindlessEngine.hpp>

using namespace MindlessEngine;

class Terrain : public Game
{
private:
  Ref<ComputeShader> terrainShader;
  GLuint terrainBuffer;
  GLuint biomesMap;
  Vector offset;
  float scale;
  float seed;
  bool regenerate;
  float* mapData;
public:
  Terrain()
  : Game(800, 600, "Terrain test", "shaders/batch.vert", "shaders/batch.frag"),
    terrainShader(createRef<ComputeShader>("shaders/terrain.comp")),
    offset(0.0f, 0.0f),
    scale(32.0f),
    seed(0.0f),
    regenerate(true),
    mapData(nullptr)
  {
    glGenTextures(1, &terrainBuffer);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, terrainBuffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, window.width, window.height, 0, GL_RGBA, GL_FLOAT, nullptr);

    biomesMap = loadTexture("assets/biomes.png", true, false);

    terrainShader->bind();
    // terrainShader->setUniform1i("uBiomesMap", 1);

    // height, moisture, biome
    mapData = new float[window.width * window.height * 3];
  }

  ~Terrain()
  {
    glDeleteTextures(1, &terrainBuffer);
    glDeleteTextures(1, &biomesMap);
    delete[] mapData;
  }

  void generate()
  {
    if (scale <= 0.0f)
      scale = 0.0001f;

    const int octaves = 6; // Update also in the shader

    srand(seed);
    Vector octaveOffsets[octaves];
    for (int i = 0; i < octaves; i++)
    {
      octaveOffsets[i].x = randomFloat(-100000.0f, 100000.0f) + offset.x;
      octaveOffsets[i].y = randomFloat(-100000.0f, 100000.0f) + offset.y;
    }

    glBindImageTexture(0, terrainBuffer, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    // glBindTextureUnit(1, biomesMap);
    terrainShader->bind();
    terrainShader->setUniform1f("uScale", scale); // > 0.0f
    terrainShader->setUniform1i("uOctaves", octaves); // > 0
    terrainShader->setUniform1f("uPersistence", 0.5f); // [0.0f;1.0f]
    terrainShader->setUniform1f("uLacunarity", 2.0f); // > 1.0f
    terrainShader->setUniform2f("uHalfSize", window.width * 0.5f, window.height * 0.5f);
    terrainShader->setUniform2fv("uOctaveOffsets", (float*)octaveOffsets, octaves);
    terrainShader->dispatch(window.width, window.height, 1);
    terrainShader->barrier();

    glReadPixels(0, 0, window.width, window.height, GL_RGB, GL_FLOAT, mapData);

    // mapData now contains all the height, moisture and biome data
    // and is ready for use
  }

  void update() override
  {
    if (Keyboard::isPressed(Keys::C))
    {
      seed = randomFloat(-10000.0f, 10000.0f);
      regenerate = true;
    }

    if (Keyboard::isPressed(Keys::Z))
    {
      scale *= 1.0f / 0.93f;
      regenerate = true;
    }

    if (Keyboard::isPressed(Keys::X))
    {
      scale *= 0.93f;
      regenerate = true;
    }

    float dx = 0.0f;
    float dy = 0.0f;
    const float speed = 4.0f;

    if (Keyboard::isPressed(Keys::W))
      dy -= 0.1f;
    if (Keyboard::isPressed(Keys::S))
      dy += 0.1f;
    if (Keyboard::isPressed(Keys::A))
      dx -= 0.1f;
    if (Keyboard::isPressed(Keys::D))
      dx += 0.1f;

    if (dx != 0.0f || dy != 0.0f)
    {
      offset = offset + normalize({ dx, dy }) * speed / scale;
      regenerate = true;
    }

    if (regenerate)
      generate();
    regenerate = false;
  }

  void render() override
  {
    window.clear();

    const Constraints& cc = window.camera.getConstraints();

    Renderer::draw(
      {cc.left, cc.bottom},
      {cc.right, cc.bottom},
      {cc.right, cc.top},
      {cc.left, cc.top},
      terrainBuffer,
      {0.0, 0.0, 1.0, 1.0},
      Colors::white()
    );
  }
};

int main(void)
{
  Terrain test;
  test.run();

  return 0;
}