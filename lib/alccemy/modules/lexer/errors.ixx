module;

#include <exception>
#include <variant>

#include <fmt/core.h>

export module alccemy.lexer.errors;

import alccemy.lexer.concepts;
import alccemy.lexer.text;
import alccemy.lexer.token;

export namespace alccemy {

template <TokenSet TokenSetT, typename InnerErrorTypes> class LexerFailure : public std::exception {
 public:
   LexerFailure(InnerErrorTypes type, const Tokens<TokenSetT>& tokens_so_far, const TextPos& text_pos,
                size_t data_index)
       : tokens_so_far(tokens_so_far), text_pos(text_pos), data_index(data_index), error_type(type) {
      std::string reason_desc = std::visit([](const auto& err_type) { return err_type.description(); }, type);

      m_message = fmt::format("Lexer failure at line {}, Col {}; Reason: {}", text_pos.line, text_pos.col, reason_desc);
   }

   template <typename... OtherInnerErrorTs>
   LexerFailure(const LexerFailure<TokenSetT, std::variant<OtherInnerErrorTs...>>& other)
       : tokens_so_far(other.tokens_so_far), text_pos(other.text_pos), data_index(other.data_index),
         error_type(std::visit([](const auto& inner) { return InnerErrorTypes(inner); }, other.error_type)) {}

   const char* what() const noexcept override { return m_message.c_str(); }

 public:
   Tokens<TokenSetT> tokens_so_far;
   TextPos text_pos;
   size_t data_index;
   InnerErrorTypes error_type;

 private:
   std::string m_message;
};
} // namespace alccemy
