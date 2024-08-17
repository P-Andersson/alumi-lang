module;

#include <compare>

export module alccemy.lexer.text;

export namespace alccemy{
   class TextPos
   {
   public:
      TextPos(size_t line, size_t col, size_t char_index)
         : m_line(line)
         , m_col(col)
         , m_char_index(char_index) 
      {}

      size_t line() const
      {
         return m_line;
      }

      size_t col() const
      {
         return m_col;
      }

      size_t char_index() const
      {
         return m_char_index;
      }

      auto operator<=>(const TextPos& other) const = default;

   private:
      size_t m_line;
      size_t m_col;
      size_t m_char_index;
   };
}