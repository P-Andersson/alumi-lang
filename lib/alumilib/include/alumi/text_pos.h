#pragma once

#include <cstdint>
#include <compare>

namespace alumi
{
   class TextPos
   {
   public:
      TextPos(size_t line, size_t col, size_t char_index);

      size_t line() const;
      
      size_t col() const;

      size_t char_index() const;

      auto operator<=>(const TextPos& other) const = default;

   private:
      size_t m_line;
      size_t m_col;
      size_t m_char_index;
   };

}

