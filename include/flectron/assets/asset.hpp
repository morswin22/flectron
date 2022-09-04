#pragma once
#include <fstream>
#include <sstream>
#include <flectron/utils/embed.hpp>
#include <flectron/utils/memory.hpp>
#include <flectron/assert/assert.hpp>

namespace flectron
{

  namespace AssetStates 
  {
    enum State 
    {
      NotLoaded,
      Loading,
      Loaded,
      Error
    };
  }

  template<typename T>
  struct Asset;

  template<typename T>
  struct AssetViewBase
  {
    const T* asset;

    AssetViewBase(const T* asset) : asset(asset) {}
    AssetViewBase(const T& asset) : asset(&asset) {}

    operator const T&() const { return *asset; }
    const T* operator ->() const { return asset; }
    operator bool() const { return asset != nullptr; }
  };

  template<typename T>
  struct AssetView : AssetViewBase<Asset<T>>
  {
    AssetView(const Asset<T>* asset) : AssetViewBase(asset) {}
    AssetView(const Asset<T>& asset) : AssetViewBase(asset) {}
  };

  template<typename T>
  struct Asset
  {
    // Types
    using DataPointer = T*;
    using ConstDataPointer = const T*;
    using DataSize = size_t;

    // Loaders
    struct Loader
    {
      virtual void operator()(DataPointer&, DataSize&) const
      {
        throw std::exception("Asset loader was not specified");
      };

      virtual std::string info() const
      {
        return "Asset loader info was not specified";
      }
    };

    // Memory loader
    struct MemoryLoader : Loader
    {
      ConstDataPointer source;
      const DataSize size;

      MemoryLoader(ConstDataPointer source, const DataSize size) : source(source), size(size) {}
      
      void operator()(DataPointer& destination, DataSize& size) const override
      {
        size = this->size;
        destination = new T[size];
        for (DataSize i = 0; i < size; ++i)
          destination[i] = source[i];
      }

      std::string info() const override
      {
        return "Memory asset loader";
      }
    };

    static Asset<T> fromMemory(ConstDataPointer source, const DataSize size)
    {
      return Asset<T>(Ref<Loader>(new MemoryLoader(source, size)));
    }

    // Variables
    Ref<Loader> loader;
    AssetStates::State state;
    DataPointer data;
    DataSize size;

    // Contructors
    Asset() : Asset(Ref<Loader>(new Loader())) {}

    Asset(Ref<Loader> loader)
      : loader(loader), state(AssetStates::NotLoaded), data(nullptr), size(0u)
    {}

    Asset(const Asset& other)
      : loader(other.loader)
    {
      if (other.isLoaded())
      {
        size = other.size;
        data = new T[size];
        for (size_t i = 0u; i < size; ++i)
          data[i] = other.data[i];
        state = other.state;
      }
      else
      {
        size = 0u;
        data = nullptr;
        state = AssetStates::NotLoaded;
      }
    }

    Asset(Asset&& other)
      : loader(other.loader), state(AssetStates::NotLoaded), data(nullptr), size(0u)
    {
      std::swap(state, other.state);
      std::swap(data, other.data);
      std::swap(size, other.size);
    }

    ~Asset()
    { 
      if (!isNotLoaded())
        unload();
    }

    // Operators
    Asset& operator=(const Asset& other)
    {
      if (this != &other)
      {
        unload();
        loader = other.loader;
        if (other.isLoaded())
        {
          size = other.size;
          data = new T[size];
          for (size_t i = 0u; i < size; ++i)
            data[i] = other.data[i];
          state = other.state;
        }
        else
        {
          size = 0u;
          data = nullptr;
          state = AssetStates::NotLoaded;
        }
      }
      return *this;
    }

    Asset& operator=(Asset&& other)
    {
      if (this != &other)
      {
        if (!isNotLoaded())
          unload();
        loader = other.loader;
        std::swap(state, other.state);
        std::swap(data, other.data);
        std::swap(size, other.size);
      }
      return *this;
    }

    operator AssetView<T>() const { return AssetView<T>(this); }
    
    // Methods
    bool isNotLoaded() const { return state == AssetStates::NotLoaded; }
    bool isLoading() const { return state == AssetStates::Loading; }
    bool isLoaded() const { return state == AssetStates::Loaded; }
    bool isError() const { return state == AssetStates::Error; }

    ConstDataPointer get() const
    {
      FLECTRON_ASSERT(isLoaded(), "Asset is in an invalid state");
      return data;
    }

    DataPointer get() 
    {
      FLECTRON_ASSERT(isLoaded(), "Asset is in an invalid state");
      return data;
    }

    virtual void unload()
    {
      FLECTRON_ASSERT(!isNotLoaded(), "Asset is not loaded");
      
      delete[] data;
      size = 0u;
      data = nullptr;
      state = AssetStates::NotLoaded;
    }

    virtual void load()
    {
      if (!isNotLoaded())
        unload();
      state = AssetStates::Loading;
      try
      {
        (*loader)(data, size);
        state = AssetStates::Loaded;
      }
      catch(...)
      {
        state = AssetStates::Error;
        throw;
      }
    }

    std::string info() const
    {
      return loader->info();
    }

  };

}
