module;

#include <vector>

export module alccemy.lexer.tokenized_text;

import alccemy.lexer.unicode;
import alccemy.lexer.token;

export namespace alccemy{

   //!
   //! Represents a set of text after being lexed, containing the tokens and the 
   //! code points
   //! 
   template <typename TokenSet>
   class TokenizedText
   {
   public:
      TokenizedText(const std::vector<UnicodeCodePoint>& text, const Tokens<TokenSet>& tokens)
      : m_text(text)
      , m_tokens(tokens)
      {

      }


      const std::vector<UnicodeCodePoint>& text() const
      {
         return m_text;
      }

      const Tokens<TokenSet>& tokens() const
      {
         return m_tokens;
      }

   private:
      std::vector<UnicodeCodePoint> m_text;
      Tokens<TokenSet> m_tokens;
   };
}