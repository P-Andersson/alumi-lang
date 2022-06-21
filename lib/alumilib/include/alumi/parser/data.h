#pragma once

#include <cstdint>
#include <string>

namespace alumi
{
   using UnicodeCodePoint = uint32_t;

   std::string as_utf8(UnicodeCodePoint cp);
}
