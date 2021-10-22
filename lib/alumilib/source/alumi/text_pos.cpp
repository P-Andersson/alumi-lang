#include "alumi/text_pos.h"

namespace alumi
{
   TextPos::TextPos(size_t line, size_t col)
      : m_line(line)
      , m_col(col)
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
}