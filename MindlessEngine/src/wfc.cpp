#include <MindlessEngine/wfc.hpp>
#include <MindlessEngine/random.hpp>
#include <fstream>
#include <sstream>
#include <functional>
#include <random>

namespace MindlessEngine { namespace WFC{

  enum ReadState
  {
    NONE, TILES, NEIGHBORS
  };

  Symmetry stringToSymmetry(std::string str)
  {
    if (str == "X") return sX;
    if (str == "T") return sT;
    if (str == "L") return sL;
    if (str == "I") return sI;
    if (str == "\\") return sBACKSLASH;
    return sP;
  }

  TileInfo::TileInfo(const std::string& path)
    : tiles(), weights(), neighbors()
  {
    std::ifstream file(path);
    std::string line;
    ReadState state = NONE;
    while (std::getline(file, line))
    {
      std::istringstream iss(line);
      std::string token;

      if (!std::getline(iss, token, '\t'))
        continue;

      if (token == "TILES")
      {
        state = TILES;
        continue;
      }
      else if (token == "NEIGHBORS")
      {
        state = NEIGHBORS;
        continue;
      }

      if (state == TILES)
      {
        std::string temp = token;

        if (!std::getline(iss, token, '\t'))
          continue;

        tiles.push_back(std::make_pair(temp, stringToSymmetry(token)));
      }
      else if (state == NEIGHBORS)
      {
        std::string left = token;
        std::string right;
        if (!std::getline(iss, right, '\t'))
         continue;

        neighbors.push_back(std::make_pair(left, right));
      }
    }
    file.close();
  }

  const auto kInvalidIndex = static_cast<size_t>(-1);

  Output::Output(int w, int h, int d)
    : wave(nullptr), changes(nullptr), w(w), h(h)
  {
    wave = new bool**[w];
    for (int i = 0; i < w; i++)
    {
      wave[i] = new bool*[h];
      for (int j = 0; j < h; j++)
      {
        wave[i][j] = new bool[d];
        for (int k = 0; k < d; k++)
          wave[i][j][k] = true;
      }
    }

    changes = new bool*[w];
    for (int i = 0; i < w; i++)
    {
      changes[i] = new bool[h];
      for (int j = 0; j < h; j++)
        changes[i][j] = false;
    }
  }

  Output::~Output()
  {
    for (int i = 0; i < w; i++)
    {
      for (int j = 0; j < h; j++)
        delete[] wave[i][j];
      delete[] wave[i];
    }
    delete[] wave;

    for (int i = 0; i < w; i++)
      delete[] changes[i];
    delete[] changes;
  }

  std::pair<std::string, std::string> splitBySpace(const std::string& s)
  {
    std::string left;
    std::string right;
    std::istringstream iss(s);
    std::getline(iss, left, ' ');
    std::getline(iss, right);
    return std::make_pair(left, right);
  }

  TileModel::TileModel(const Ref<TileInfo>& config, const std::string& path, int numTiles, int width, int height, bool periodic)
    : TextureAtlas(path, 1, numTiles, true), width(width), height(height), numPatterns(0), periodic(periodic), foundation(kInvalidIndex), 
      weights(), config(config), propagator(nullptr), tiles(nullptr), tileSize(32), tileMap()
  {
    tileMap[-1] = new Tile({0.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f, 1.0f});

    std::vector<std::array<int, 8>> action;
    std::unordered_map<std::string, size_t> firstOccurrence;

    int currentTile = 0;
    for (const auto& tile : config->tiles)
    {
      std::function<int(int)> a, b;
      int cardinality;

      if (tile.second == sL)
      {
        cardinality = 4;
        a = [](int i){ return (i + 1) % 4; };
        b = [](int i){ return i % 2 == 0 ? i + 1 : i - 1; };
      }
      else if (tile.second == sT)
      {
        cardinality = 4;
        a = [](int i){ return (i + 1) % 4; };
        b = [](int i){ return i % 2 == 0 ? i : 4 - i; };
      }
      else if (tile.second == sI)
      {
        cardinality = 2;
        a = [](int i){ return 1 - i; };
        b = [](int i){ return i; };
      }
      else if (tile.second == sBACKSLASH)
      {
        cardinality = 2;
        a = [](int i){ return 1 - i; };
        b = [](int i){ return 1 - i; };
      }
      else if (tile.second == sX)
      {
        cardinality = 1;
        a = [](int i){ return i; };
        b = [](int i){ return i; };
      }
      else if (tile.second == sP)
      {
        cardinality = 8; 
        a = [](int i){ return i < 4 ? (i + 1) % 4 : 4 + (i - 1) % 4; };
        b = [](int i){ return i < 4 ? i + 4 : i - 4; };
      }

      const size_t patternsSoFar = action.size();
      firstOccurrence[tile.first] = patternsSoFar;

      for (int t = 0; t < cardinality; ++t)
      {
        std::array<int, 8> map;

        map[0] = t;
        map[1] = a(t);
        map[2] = a(map[1]);
        map[3] = a(map[2]);
        map[4] = b(t);
        map[5] = b(map[1]);
        map[6] = b(map[2]);
        map[7] = b(map[3]);

        for (int s = 0; s < 8; ++s)
          map[s] += patternsSoFar;

        action.push_back(map);
      }

      // TODO create all possible rotations and reflections
      glm::vec2 pa(0.0f, 0.0f), pb(1.0f, 0.0f), pc(1.0f, 1.0f), pd(0.0f, 1.0f);
      float angle = M_PI / 2.0f;
      constexpr glm::vec2 pivot(0.5f, 0.5f);
      for (int t = 0; t < cardinality; ++t)
      {
        float s = std::sin(angle);
        float c = std::cos(angle);

        glm::mat2 rot(c, -s, s, c);

        pa = rot * (pa - pivot) + pivot;
        pb = rot * (pb - pivot) + pivot;
        pc = rot * (pc - pivot) + pivot;
        pd = rot * (pd - pivot) + pivot;

        if (tile.second == sP && t == 4)
        {
          pa = glm::vec2(1.0f, 0.0f);
          pb = glm::vec2(0.0f, 0.0f);
          pc = glm::vec2(0.0f, 1.0f);
          pd = glm::vec2(1.0f, 1.0f);
        }

        const size_t index = patternsSoFar + t;
        tileMap[index] = new Tile({(float)currentTile/(float)numTiles, 0.0f, 1.0f/(float)numTiles, 1.0f}, {pa.x, pa.y, pb.x, pb.y}, {pc.x, pc.y, pd.x, pd.y});
      }

      for (int t = 0; t < cardinality; ++t)
      {
        weights.push_back(1.0); // TODO load from config file
      }

      currentTile++;
    }

    numPatterns = action.size();
    propagator = new bool**[4];
    for (int i = 0; i < 4; ++i)
    {
      propagator[i] = new bool*[numPatterns];
      for (int j = 0; j < numPatterns; ++j)
        propagator[i][j] = new bool[numPatterns]{ false };
    }

    for (const auto& neighbor : config->neighbors)
    {
      const auto& left = splitBySpace(neighbor.first);
      const auto& right = splitBySpace(neighbor.second);

      int L = action[firstOccurrence[left.first]][left.second.size() > 0 ? std::stoi(left.second) : 0];
      int R = action[firstOccurrence[right.first]][right.second.size() > 0 ? std::stoi(right.second) : 0];
      int D = action[L][1];
      int U = action[R][1];

      propagator[0][L][R] = true;
      propagator[0][action[L][6]][action[R][6]] = true;
      propagator[0][action[R][4]][action[L][4]] = true;
      propagator[0][action[R][2]][action[L][2]] = true;

      propagator[1][D][U] = true;
      propagator[1][action[U][6]][action[D][6]] = true;
      propagator[1][action[D][4]][action[U][4]] = true;
      propagator[1][action[U][2]][action[D][2]] = true;
    }

    for (int t1 = 0; t1 < numPatterns; ++t1) {
      for (int t2 = 0; t2 < numPatterns; ++t2) {
        propagator[2][t1][t2] = propagator[0][t2][t1];
        propagator[3][t1][t2] = propagator[1][t2][t1];
      }
    }
  }

  bool TileModel::propagate(Output* output) const 
  {
    bool changed = false;

    for (int x2 = 0; x2 < width; ++x2)
      for (int y2 = 0; y2 < height; ++y2)
        for (int d = 0; d < 4; ++d) 
        {
          int x1 = x2, y1 = y2;
          if (d == 0) 
          {
            if (x2 == 0) 
            {
              if (!periodic)
                continue;
              x1 = width - 1;
            } 
            else 
            {
              x1 = x2 - 1;
            }
          } 
          else if (d == 1) 
          {
            if (y2 == height - 1) 
            {
              if (!periodic)
                continue;
              y1 = 0;
            } 
            else 
            {
              y1 = y2 + 1;
            }
          } 
          else if (d == 2) 
          {
            if (x2 == width - 1) 
            {
              if (!periodic)
                continue;
              x1 = 0;
            } 
            else 
            {
              x1 = x2 + 1;
            }
          } 
          else 
          {
            if (y2 == 0) 
            {
              if (!periodic)
                continue;
              y1 = height - 1;
            } 
            else 
            {
              y1 = y2 - 1;
            }
          }

          if (!output->changes[x1][y1]) 
            continue;

          for (int t2 = 0; t2 < numPatterns; ++t2) 
          {
            if (output->wave[x2][y2][t2]) 
            {
              bool b = false;
              for (int t1 = 0; t1 < numPatterns && !b; ++t1) 
              {
                if (output->wave[x1][y1][t1]) 
                {
                  b = propagator[d][t1][t2];
                }
              }
              if (!b) 
              {
                output->wave[x2][y2][t2] = false;
                output->changes[x2][y2] = true;
                changed = true;
              }
            }
          }
        }

    return changed;
  }

  const std::string resultToString(const Result& result)
  {
    return result == Result::kSuccess ? "success" : result == Result::kFail ? "fail" : "unfinished";
  }

  double calculateSum(const std::vector<double>& a)
  {
    return std::accumulate(a.begin(), a.end(), 0.0);
  }

  size_t spinTheBottle(const std::vector<double>& a, double between_zero_and_one)
  {
    double sum = calculateSum(a);

    if (sum == 0.0) {
      return std::floor(between_zero_and_one * a.size());
    }

    double between_zero_and_sum = between_zero_and_one * sum;

    double accumulated = 0;

    for (int i = 0; i < a.size(); ++i) {
      accumulated += a[i];
      if (between_zero_and_sum <= accumulated) {
        return i;
      }
    }

    return 0;
  }

  Result TileModel::run(Output* output, size_t seed, size_t limit) const
  {
    std::mt19937 gen(seed);
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    RandomDouble randomDouble = [&]() { return dist(gen); };

    for (size_t l = 0; l < limit || limit == 0; ++l) {
      Result result = observe(output, randomDouble);

      if (result != Result::kUnfinished) {
        // std::cout << resultToString(result) << " after " << l << " iterations\n";
        return result;
      }
      while (propagate(output));
    }

    // std::cout << "Unfinished after " << limit << " iterations\n";
    return Result::kUnfinished;
  }

  Result TileModel::observe(Output* output, RandomDouble& randomDouble) const
  {
    int argminx, argminy;
    const Result& result = findLowestEntropy(output, randomDouble, argminx, argminy);
    if (result != Result::kUnfinished) 
      return result;

    std::vector<double> distribution(numPatterns);
    for (int t = 0; t < numPatterns; ++t) {
      distribution[t] = output->wave[argminx][argminy][t] ? weights[t] : 0.0;
    }
    size_t r = spinTheBottle(std::move(distribution), randomDouble());
    for (int t = 0; t < numPatterns; ++t) {
      output->wave[argminx][argminy][t] = t == r;
    }
    output->changes[argminx][argminy] = true;

    return Result::kUnfinished;
  }

  Result TileModel::findLowestEntropy(const Output* output, RandomDouble& randomDouble, int& argminx, int& argminy) const
  {
    double min = std::numeric_limits<double>::infinity();

    for (int x = 0; x < width; ++x)
      for (int y = 0; y < height; ++y) 
      {

        size_t numSuperimposed = 0;
        double entropy = 0;

        for (int t = 0; t < numPatterns; ++t) {
          if (output->wave[x][y][t]) {
            numSuperimposed += 1;
            entropy += weights[t];
          }
        }

        if (entropy == 0 || numSuperimposed == 0) {
          return Result::kFail;
        }

        if (numSuperimposed == 1) {
          continue;
        }

        const double noise = 0.5 * randomDouble();
        entropy += noise;

        if (entropy < min) {
          min = entropy;
          argminx = x;
          argminy = y;
        }
      }

    if (min == std::numeric_limits<double>::infinity()) {
      return Result::kSuccess;
    } else {
      return Result::kUnfinished;
    }
  }

  std::vector<std::vector<Tile*>> TileModel::getTiles(Output* output)
  {
    std::vector<std::vector<Tile*>> tiles;
    for (int i = 0; i < width; ++i)
    {
      tiles.push_back(std::vector<Tile*>());
      for (int j = 0; j < height; ++j)
      {
        int selected = -1;
        for (int k = 0; k < numPatterns; ++k)
        {
          if (output->wave[i][j][k])
          {
            if (selected == -1)
            {
              selected = k;
            }
            else
            {
              selected = -1;
              break;
            }
          }
        }
        // std::cout << selected << " ";
        tiles[i].push_back(tileMap[selected]);
      }
      // std::cout << std::endl;
    }
    // std::cout << std::endl << std::endl;
    return tiles;
  }

}; };