#pragma once

#include "alumi/lexer/token.h"
#include "alumi/parser/data.h"

namespace alumi
{
   //!
   //! Represents a set of text after being lexed, containing the tokens and the 
   //! code points
   //! 
   class LexedText
   {
   public:
      LexedText(const std::vector<UnicodeCodePoint>& text, const Tokens& tokens);

      const std::vector<UnicodeCodePoint>& text() const;

      const Tokens& tokens() const;
   private:
      std::vector<UnicodeCodePoint> m_text;
      Tokens m_tokens;
   };
}
