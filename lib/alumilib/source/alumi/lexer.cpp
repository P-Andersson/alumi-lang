#include "alumi/lexer.h"

#include <fmt/core.h>

namespace alumi
{
   LexerFailure::LexerFailure(Reason reason, const Tokens& tokens_so_far, const TextPos& text_pos, size_t data_index)
      : tokens_so_far(tokens_so_far)
      , text_pos(text_pos)
      , data_index(data_index)
      , reason(reason)
   {
      std::string reason_desc;
      switch (reason)
      {
      case Reason::UnexpectedCodepoint:
         reason_desc = "Unexpected Codepoint";
         break;
      case Reason::MismatchedIndentionCharacters:
         reason_desc = "Spaces and Tabs used for indention in the same source, these cannot be mixed.";
         break;
      }

      m_message = fmt::format("Lexer failure at line {}, Col {}; Reason: {}", text_pos.line(), text_pos.col(), reason_desc);
   }

   const char* LexerFailure::what() const noexcept
   {
      return m_message.c_str();
   }
}