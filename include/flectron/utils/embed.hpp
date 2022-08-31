#pragma once
#include <cstdint>
#include <string>
#include <fstream>
#include <sstream>
#include <map>

#include <flectron/assert/assert.hpp>
#include <flectron/utils/memory.hpp>

#include <spdlog/fmt/fmt.h>
#include <stb_image.h>
#include <GL/glew.h>

namespace flectron
{

  struct Embed
  {
    const char* data;
    const size_t size;
  };

}

#define FLECTRON_EMBED(name) extern "C" flectron::Embed name(void)
