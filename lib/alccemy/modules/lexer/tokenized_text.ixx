module;

#include <vector>

export module alccemy.lexer.tokenized_text;

import alccemy.lexer.unicode;
import alccemy.lexer.token;

export namespace alccemy {

//!
//! Represents a set of text after being lexed, containing the tokens and the
//! code points
//!
template <typename TokenSet> class TokenizedText {
 public:
   TokenizedText(const Tokens<TokenSet>& tokens) : m_tokens(tokens) {}

   const Tokens<TokenSet>& tokens() const { return m_tokens; }

 private:
   Tokens<TokenSet> m_tokens;
};
} // namespace alccemy
