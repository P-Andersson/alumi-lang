module;

#include <concepts>

export module alccemy.lexer.concepts;

import alccemy.lexer.text;
import alccemy.lexer.unicode;

export namespace alccemy {

   /**
   * Constraints that a TokenSet needs to fulfill
   * Must be an enum with at least the Linebreak and EndOfFile values
   **/
   template <typename T>
   concept TokenSet = std::is_enum_v<T> && requires(T token_set) {
      T::Linebreak;
      T::EndOfFile;
   };

   /**
   * Constraints that a TokenSet which is indent signfiicant needs to fulfill
   * Must fullfill the same requirements as a TokenSet, plus have the Indent and Dedent tokens
   **/
   template <typename T>
   concept TokenSetIndentSignficiant = TokenSet<T> && requires(T token_set) {
      T::Indent;
      T::Dedent;
   };

   // TODO Check return type here
   /**
   * Contraints that a token producer class needs to fulfill
   **/
   template<typename T, typename TokenSetT>
   concept TokenPattern = requires(T & t, TextPos start, TextPos end) { t.make_token(start, end); };

   /**
   * Constraints that a lexer pattern class needs to fulfill
   **/
   template<typename T>
   concept LexerPattern = requires(T & t, UnicodeCodePoint cp, size_t index) { t.check(cp, index); t.terminate(index); };


}