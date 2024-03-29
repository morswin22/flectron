#pragma once
#include <flectron/renderer/renderer.hpp>
#include <functional>

namespace flectron { namespace WFC {

  using RandomDouble = std::function<double()>;

  enum Symmetry
  {
    sX, sT, sI, sL, sBACKSLASH, sP
  }; // X T I L \ P

  class TileInfo 
  {
  public:
    TextView source;

  private:
    std::vector<std::pair<std::string, Symmetry>> tiles;
    std::vector<double> weights;
    std::vector<std::pair<std::string, std::string>> neighbors;

  public:
    TileInfo(const Text& source);
    TileInfo(const TextView& source);

    friend class TileModel;
  };

  struct Tile {
    GLuint texture;
    glm::vec4 textureCoords;
    glm::vec4 ab;
    glm::vec4 cd;

    Tile(GLuint texture, const glm::vec4& textureCoords, const glm::vec4& ab, const glm::vec4& cd);

    void render(const Vector& position, float size, const Color& color = Colors::white()) const;
  };

  struct Output {
    bool*** wave;
    bool** changes;
    int w, h;

    Output(int w, int h, int d);
    ~Output();
  };

  enum Result
  {
    kSuccess, kFail, kUnfinished,
  };

  class TileModel : public TextureAtlas
  {
  public:
    TileModel(const Ref<TileInfo>& config, const Image& image, int numTiles, int width, int height, bool periodic);
    TileModel(const Ref<TileInfo>& config, const ImageView& image, int numTiles, int width, int height, bool periodic);

    bool propagate(Output* output) const;

    Ref<Output> createOutput() const { return std::make_shared<Output>(width, height, numPatterns); }

    Result run(Ref<Output>& output, size_t seed, size_t limit) const;
    Result observe(Output* output, RandomDouble& randomDouble) const;
    Result findLowestEntropy(const Output* output, RandomDouble& random_double, int& argminx, int& argminy) const;

    std::vector<std::vector<Tile*>> getTiles(Ref<Output>& output);
    GLuint getTexture() const { return image->getGPU(); }

  public:
    size_t width;
    size_t height;
    size_t numPatterns;
    bool periodic;
    size_t foundation; // TODO remove this
    std::vector<double> weights;

  private:
    Ref<TileInfo> config;
    bool*** propagator;
    Tile*** tiles;
    size_t tileSize;

    std::unordered_map<int, Tile*> tileMap;
  };

} }
