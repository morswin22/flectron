#include <flectron/assets/text.hpp>

namespace flectron
{

  // TODO move the implementation to this file

  TextView::operator std::string_view() const { return std::string_view(asset->get(), asset->size); }
  TextView::operator std::string() const { return std::string(asset->get(), asset->size); }

}
