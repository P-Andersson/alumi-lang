module;

#include <exception>

#include <fmt/core.h>

export module alccemy.lexer.errors;

import alccemy.lexer.concepts;
import alccemy.lexer.text;
import alccemy.lexer.token;

export namespace alccemy {

   enum class LexerFailureReason
   {
      UnexpectedCodepoint,
      MismatchedIndentionCharacters
   };

   template <TokenSet TokenSetT>
      class LexerFailure : public std::exception
   {
   public:
      LexerFailure(LexerFailureReason reason, const Tokens<TokenSetT>& tokens_so_far, const TextPos& text_pos, size_t data_index)
         : tokens_so_far(tokens_so_far)
         , text_pos(text_pos)
         , data_index(data_index) 
         , reason(reason)
      {
         std::string reason_desc;
         switch (reason)
         {
         case LexerFailureReason::UnexpectedCodepoint:
            reason_desc = "Unexpected Codepoint";
            break;
         case LexerFailureReason::MismatchedIndentionCharacters:
            reason_desc = "Spaces and Tabs used for indention in the same source, these cannot be mixed.";
            break;
         }

         m_message = fmt::format("Lexer failure at line {}, Col {}; Reason: {}", text_pos.line, text_pos.col, reason_desc);
      }

      const char* what() const noexcept override
      {
         return m_message.c_str();
      }

      Tokens<TokenSetT> tokens_so_far;
      TextPos text_pos;
      size_t data_index;
      LexerFailureReason reason;

   private:
      std::string m_message;
   };
}