#pragma once
#include "asset.hpp"

namespace flectron
{

  struct SceneAsset : Asset<char>
  {
    // Loaders
    struct Loader : Asset::Loader
    {
      virtual bool isWritable() const { return false; }

      virtual void operator()(DataPointer&, DataSize&) const
      {
        throw std::exception("Scene loader was not specified");
      }

      virtual void save(const DataPointer&, const DataSize&) const
      {
        throw std::exception("Scene saver was not specified");
      }
    };
    
    struct FileLoader : Loader
    {
      const std::string path;

      FileLoader(const std::string& path) : path(path) {}

      bool isWritable() const override { return true; }

      void operator()(DataPointer& destination, DataSize& size) const
      {
        std::ifstream file(path, std::ios::binary | std::ios::ate);
        FLECTRON_ASSERT(file.is_open(), "Failed to open file");
        size = (DataSize)file.tellg();
        destination = new char[size];
        file.seekg(0);
        file.read(destination, size);
      }

      void save(const DataPointer& destination, const DataSize& size) const
      {
        std::ofstream file(path, std::ios::binary);
        FLECTRON_ASSERT(file.is_open(), "Failed to open file");
        file.write(destination, size);
      }

      std::string info() const override
      {
        return "Scene file loader for " + path;
      }
    };

    static SceneAsset fromFile(const std::string& path)
    {
      return SceneAsset(Ref<Loader>(new FileLoader(path)));
    }

    struct EmbedLoader : Loader
    {
      const Embed embed;

      EmbedLoader(const Embed& embed) : embed(embed) {}

      void operator()(DataPointer& destination, DataSize& size) const
      {
        size = embed.size;
        destination = new char[size];
        std::memcpy(destination, embed.data, size);
      }

      void save(const DataPointer& destination, const DataSize& size) const
      {
        throw std::exception("Cannot save to embedded scene");
      }

      std::string info() const override
      {
        return "Scene embed loader";
      }
    };

    static SceneAsset fromEmbed(const Embed& embed)
    {
      return SceneAsset(Ref<Loader>(new EmbedLoader(embed)));
    }

    // Constructors
    SceneAsset() : Asset() {}

    SceneAsset(Ref<Loader> loader) 
      : Asset(loader) 
    {}

  };

};
