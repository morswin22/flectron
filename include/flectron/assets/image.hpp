#pragma once
#include "asset.hpp"
#include <stb_image.h>

namespace flectron
{

  struct Image;

  struct ImageView : AssetViewBase<Image>
  {
    ImageView(const Image* asset) : AssetViewBase(asset) {}
    ImageView(const Image& asset) : AssetViewBase(asset) {}
  };

  struct Image : Asset<unsigned char>
  {
    // Types
    using Dim = int;

    // Base loader
    struct Loader : Asset::Loader
    {
      virtual void operator()(DataPointer&, DataSize&, Dim&, Dim&, Dim&) const
      {
        throw std::exception("Image loader was not specified");
      }
    };

    // File loader
    struct FileLoader : Loader
    {
      const std::string path;

      FileLoader(const std::string& path) : path(path) {}

      void operator()(DataPointer& destination, DataSize& size, Dim& width, Dim& height, Dim& channels) const override
      {
        destination = stbi_load(path.c_str(), &width, &height, &channels, STBI_default);
        FLECTRON_ASSERT(destination, "Failed to load texture");
        size = width * height * channels;
      }

      std::string info() const override
      {
        return "Image file loader for " + path;
      }
    };

    static Image fromFile(const std::string& path)
    {
      return Image(Ref<Loader>(new FileLoader(path)));
    }

    // Embed loader
    struct EmbedLoader : Loader
    {
      const Embed source;

      EmbedLoader(const Embed& source) : source(source) {}

      void operator()(DataPointer& destination, DataSize& size, Dim& width, Dim& height, Dim& channels) const override
      {
        destination = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(source.data), source.size, &width, &height, &channels, STBI_default);
        FLECTRON_ASSERT(destination, "Failed to load texture");
        size = width * height * channels;
      }

      std::string info() const override
      {
        return "Image embed loader";
      }
    };

    static Image fromEmbed(const Embed& source)
    {
      return Image(Ref<Loader>(new EmbedLoader(source)));
    }

    // Contructors
    Image() : Image(Ref<Loader>(new Loader())) {}

    Image(Ref<Loader> loader) 
      : Asset(loader), textureID(0u), width(0), height(0), channels(0)
    {}

    ~Image()
    {
      if (isGPU())
        unloadGPU();
      if (!isNotLoaded())
        unload();
    }

    // If the image is uploaded to the GPU, then this will only copy the loader
    Image(const Image& other)
      : Asset(other), textureID(0u), width(0), height(0), channels(0) // TODO should this deep copy on the GPU as well?
    {}

    Image(Image&& other)
      : Asset(std::move(other)), textureID(other.textureID), width(other.width), height(other.height), channels(other.channels)
    {
      other.textureID = 0u;
      other.width = 0;
      other.height = 0;
      other.channels = 0;
    }

    // Operators
    // If the image is uploaded to the GPU, then this will only copy the loader
    Image& operator=(const Image& other)
    {
      if (this != &other)
      {
        if (isGPU())
          unloadGPU();
        if (!isNotLoaded())
          unload();
        Asset::operator=(other);
      }
      return *this;
    }

    Image& operator=(Image&& other)
    {
      if (this != &other)
      {
        if (isGPU())
          unloadGPU();
        if (!isNotLoaded())
          unload();
        Asset::operator=(std::move(other));
        std::swap(textureID, other.textureID);
        std::swap(width, other.width);
        std::swap(height, other.height);
        std::swap(channels, other.channels);
      }
      return *this;
    }

    operator ImageView() const { return ImageView(this); }

    // Types
    using Parameters = std::map<GLenum, GLenum>;

    // Variables
    static inline Parameters defaultParameters = {
      { GL_TEXTURE_MIN_FILTER, GL_NEAREST },
      { GL_TEXTURE_MAG_FILTER, GL_NEAREST },
      { GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE },
      { GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE }
    };

    GLuint textureID;
    Dim width;
    Dim height;
    Dim channels;

    // Methods
    bool isGPU() const { return textureID != 0u; }

    GLuint getGPU() const
    {
      FLECTRON_ASSERT(isGPU(), "Image is not uploaded to the GPU");
      return textureID;
    }

    void loadGPU(const Parameters& parameters = defaultParameters)
    {
      FLECTRON_ASSERT(isLoaded(), "Image is not loaded");

      if (isGPU())
        unloadGPU();

      glCreateTextures(GL_TEXTURE_2D, 1, &textureID);
      glBindTexture(GL_TEXTURE_2D, textureID);
      for (auto& [key, value] : parameters)
        glTexParameteri(GL_TEXTURE_2D, key, value);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    }

    void unloadGPU()
    {
      FLECTRON_ASSERT(isGPU(), "Image is not uploaded to the GPU");

      glDeleteTextures(1, &textureID);
      textureID = 0u;
    }

    void unload() override
    {
      FLECTRON_ASSERT(!isNotLoaded(), "Image is not loaded");
      
      delete[] data;
      size = 0u;
      data = nullptr;
      state = AssetStates::NotLoaded;
      width = 0;
      height = 0;
      channels = 0;
    }

    void load() override
    {
      if (!isNotLoaded())
        unload();
      state = AssetStates::Loading;
      try
      {
        (*static_cast<Loader*>(loader.get()))(data, size, width, height, channels);
        state = AssetStates::Loaded;
      }
      catch(...)
      {
        state = AssetStates::Error;
        throw;
      }
    }
  };

}
