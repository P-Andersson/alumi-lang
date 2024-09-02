module;

#include <cstdint>
#include <string>

#include <utf8cpp/utf8.h>

export module alccemy.lexer.unicode;

export namespace alccemy {
using UnicodeCodePoint = uint32_t;

std::string as_utf8(UnicodeCodePoint cp) {
   std::string output(4, 0);
   utf8::utf32to8(&cp, &cp + 1, output.begin());
   for (size_t i = 0; i < output.size(); ++i) {
      if (output[i] == 0) {
         output.resize(i);
         break;
      }
   }
   return output;
}
} // namespace alccemy
