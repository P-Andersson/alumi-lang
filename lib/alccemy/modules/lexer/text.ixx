module;

#include <compare>
#include <iostream>
#include <string>

export module alccemy.lexer.text;

export namespace alccemy{
   class TextPos
   {
   public:
      TextPos(size_t line, size_t col, size_t text_index)
         : line(line)
         , col(col)
         , text_index(text_index)
      {}

      auto operator<=>(const TextPos& other) const = default;

      std::string to_string() const 
      {
         return "line: " + std::to_string(line) + 
                ", col: " + std::to_string(col) + 
                ", index: " + std::to_string(text_index);
      }

      size_t line;
      size_t col;
      size_t text_index;
   };
}

std::ostream& operator<<(std::ostream& os, const alccemy::TextPos& pos)
{
   os << pos.to_string();
   return os;
}