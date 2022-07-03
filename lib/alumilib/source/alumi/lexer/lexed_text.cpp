#include "alumi/lexer/lexed_text.h"

namespace alumi
{
   LexedText::LexedText(const std::vector<UnicodeCodePoint>& text, const Tokens& tokens)
      : m_text(text)
      , m_tokens(tokens)   {

   }

   const std::vector<UnicodeCodePoint>& LexedText::text() const
   {
      return m_text;
   }

   const Tokens& LexedText::tokens() const
   {
      return m_tokens;
   }
}
