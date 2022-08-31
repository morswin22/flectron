#pragma once
#include "asset.hpp"

namespace flectron
{

  struct Text;

  struct TextView : AssetViewBase<Text>
  {
    TextView(const Text* asset) : AssetViewBase(asset) {}
    TextView(const Text& asset) : AssetViewBase(asset) {}

    operator std::string_view() const;
    operator std::string() const;
  };

  struct Text : Asset<char>
  {
    // C-style string loader
    struct CStringLoader : Loader
    {
      const char* source;

      CStringLoader(const char* source) : source(source) {}

      void operator()(DataPointer& destination, DataSize& size) const override
      {
        size = std::strlen(source);
        destination = new char[size];
        std::memcpy(destination, source, size);
      }

      std::string info() const override
      {
        return "C-style string loader";
      }
    };

    static Text fromCString(const char* source)
    {
      return Text(Ref<Loader>(new CStringLoader(source)));
    }

    // String loader
    struct StringLoader : Loader
    {
      const std::string* source;

      StringLoader(const std::string* source) : source(source) {}

      void operator()(DataPointer& destination, DataSize& size) const override
      {
        size = source->size();
        destination = new char[size];
        std::memcpy(destination, source->data(), size);
      }

      std::string info() const override
      {
        return "String loader";
      }
    };

    static Text fromString(const std::string& source)
    {
      return Text(Ref<Loader>(new StringLoader(&source)));
    }

    // File loader
    struct FileLoader : Loader
    {
      const std::string path;
      
      FileLoader(const std::string& path) : path(path) {}

      void operator()(DataPointer& destination, DataSize& size) const override
      {
        std::ifstream file(path);
        FLECTRON_ASSERT(file.is_open(), "Asset couldn't open file " + path);
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string contents = buffer.str();
        size = contents.size();
        destination = new char[size];
        std::memcpy(destination, contents.data(), size);
      }

      std::string info() const override
      {
        return "Text file loader for " + path;
      }
    };

    static Text fromFile(const std::string& path)
    {
      return Text(Ref<Loader>(new FileLoader(path)));
    }

    // Embed loader
    struct EmbedLoader : Loader
    {
      const Embed source;
      
      EmbedLoader(const Embed& source) : source(source) {}
      
      void operator()(DataPointer& destination, DataSize& size) const override
      {
        size = source.size;
        destination = new char[size];
        std::memcpy(destination, source.data, size);
      }

      std::string info() const override
      {
        return "Text embed loader";
      }
    };

    static Text fromEmbed(const Embed& source)
    {
      return Text(Ref<Loader>(new EmbedLoader(source)));
    }

    // Memory loader
    static Text fromMemory(const char* source, const size_t size)
    {
      return Text(Ref<Loader>(new MemoryLoader(source, size)));
    }

    // Constructors
    Text() : Asset() {}

    Text(Ref<Loader> loader)
      : Asset(loader)
    {}

    // Operators
    operator std::string_view() const { return std::string_view(get(), size); }
    operator std::string() const { return std::string(get(), size); }
    operator TextView() const { return TextView(this); }
  };

}
