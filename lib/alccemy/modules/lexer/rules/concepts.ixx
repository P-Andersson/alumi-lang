module;

#include <concepts>
#include <expected>

export module alccemy.lexer.rules.concepts;

import alccemy.lexer.rules.types;
import alccemy.lexer.errors;
import alccemy.lexer.token;
import alccemy.lexer.text;
import alccemy.lexer.unicode;

export namespace alccemy {

   template<typename ValueT, typename ResultT, typename TokenSetT, typename ErrorType>
   concept ExpectedValue = std::same_as<ValueT, std::expected<ResultT, ErrorType>>;

   /**
   * Constraints that a lexer rule class needs to fulfill
   **/
   template<typename T, typename TokenSetT>
   concept LexerRule = requires(T& t, Tokens<TokenSetT>& tokens, UnicodeCodePoint cp, TextPos pos) { 
      t.initial_state();
      { t.handle_code_point(decltype(t.initial_state())(), tokens, cp, pos) } -> ExpectedValue<RulesResult, TokenSetT, typename T::ErrorType>;
      { t.end_lexing(decltype(t.initial_state())(), tokens, pos) } -> ExpectedValue<void, TokenSetT, typename T::ErrorType>;
   };


}