#include "alumi/text_pos.h"

namespace alumi
{
   TextPos::TextPos(size_t line, size_t col, size_t char_index)
      : m_line(line)
      , m_col(col)
      , m_char_index(char_index)
   {

   }

   size_t TextPos::line() const
   {
      return m_line;
   }

   size_t TextPos::col() const
   {
      return m_col;
   }

   size_t TextPos::char_index() const
   {
      return m_char_index;
   }
}