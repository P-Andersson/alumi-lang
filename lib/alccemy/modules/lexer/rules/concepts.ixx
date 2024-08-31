module;

#include <concepts>

export module alccemy.lexer.rules.concepts;

import alccemy.lexer.rules.types;
import alccemy.lexer.token;
import alccemy.lexer.text;
import alccemy.lexer.unicode;

export namespace alccemy {

   /**
   * Constraints that a lexer rule class needs to fulfill
   **/
   template<typename T, typename TokenSetT>
   concept LexerRule = requires(T& t, Tokens<TokenSetT>& tokens, UnicodeCodePoint cp, TextPos pos) { 
      t.initial_state();
      { t.handle_code_point(decltype(t.initial_state())(), tokens, cp, pos) } -> std::same_as<RulesResult>;
      { t.end_lexing(decltype(t.initial_state())(), tokens, pos) } -> std::same_as<RulesResult>;
   };


}