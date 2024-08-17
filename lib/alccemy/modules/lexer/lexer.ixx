module;

#include <exception>
#include <string>
#include <vector>
#include <optional>

#include <fmt/core.h>

export module alccemy.lexer;

export import alccemy.lexer.concepts;
export import alccemy.lexer.patterns;
export import alccemy.lexer.text;
export import alccemy.lexer.token;
export import alccemy.lexer.tokenized_text;
export import alccemy.lexer.unicode;

namespace alccemy {

   export template <LexerPattern PatternT, TokenSet TokenSetT>
   class Tokenize
   {
   public:
      Tokenize(const PatternT& pattern, Token<TokenSetT>::Type type)
         : m_pattern(pattern)
         , m_token_type(type)
      {

      }

      LexerResult check(UnicodeCodePoint cp, size_t index)
      {
         return m_pattern.check(cp, index);
      }

      LexerResult terminate(size_t index)
      {
         return m_pattern.terminate(index);
      }


      Token<TokenSetT> make_token(size_t line, size_t start, size_t end, size_t start_string_index) const
      {
         return Token<TokenSetT>(m_token_type, TextPos(line, start, start_string_index), end - start);
      }

   private:
      PatternT m_pattern;
      Token<TokenSetT>::Type m_token_type;
   };
   

   export template <LexerPattern PatternT, TokenSet TokenSetT>
   Tokenize(const PatternT& pattern, TokenSetT type) -> Tokenize<PatternT, TokenSetT>;

   export enum class LexerFailureReason
   {
      UnexpectedCodepoint,
      MismatchedIndentionCharacters
   };

   export template <TokenSet TokenSetT>
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

         m_message = fmt::format("Lexer failure at line {}, Col {}; Reason: {}", text_pos.line(), text_pos.col(), reason_desc);
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


   export template <TokenSet TokenSetT, LexerPattern... PatternTs>
   class Lexer
   {
   public:
      Lexer(PatternTs... patterns)
         : m_pattern(patterns...)
      {
      }

      TokenizedText<TokenSetT> lex(const std::vector<UnicodeCodePoint>& text)
      {
         // TODO: Figure out how to represent linebreaks, indents and eofs? Mandatory members of the token set?

         static const std::vector<UnicodeCodePoint> indention_chars({ 0x20, 0x09 });

         reset();

         bool is_indenting = true;
         size_t last_indent_chars = 0;
         std::optional<char> indent_char;

         Tokens<TokenSetT> tokens;
         size_t pos = 0;
         size_t cur_token_start = 0;
         size_t line = 0;
         size_t line_start = 0;

         auto handle_lexer_result = [&](const LexerResult& res)
            {
               if (res.type == LexerResults::Completed)
               {
                  pos += 1 - res.backtrack_cols;
                  cur_token_start = pos;
                  reset();
               }
               else if (res.type == LexerResults::Failed)
               {
                  reset();
                  throw LexerFailure<TokenSetT>(LexerFailureReason::UnexpectedCodepoint, tokens, TextPos(line, cur_token_start - line_start, cur_token_start), pos);
               }
               else
               {
                  pos += 1 - res.backtrack_cols;
               }
            };

         while (pos < text.size())
         {
            auto character = text[pos];
            if (character == 0x0a) // Newline
            {
               if (cur_token_start < pos && !is_indenting)
               {
                  LexerResult res = terminate_codepoint(cur_token_start, pos - cur_token_start, tokens, line, cur_token_start - line_start, pos - line_start);
                  handle_lexer_result(res);
               }

               tokens.push_back(Token<TokenSetT>(Token<TokenSetT>::Type::Linebreak, TextPos(line, pos - line_start, pos), 1));
               line_start = pos + 1;
               line += 1;
               pos += 1;
               is_indenting = true;
               continue;
            }
            if (is_indenting)
            {
               if ((indent_char == std::nullopt && std::find(indention_chars.begin(), indention_chars.end(), character) != indention_chars.end()))
               {
                  indent_char = character;
               }
               if (indent_char != character)
               {
                  if (std::find(indention_chars.begin(), indention_chars.end(), character) != indention_chars.end())
                  {
                     throw LexerFailure<TokenSetT>(LexerFailureReason::MismatchedIndentionCharacters, tokens, TextPos(line, pos - line_start, cur_token_start), pos);
                  }
                  tokens.push_back(Token<TokenSetT>(Token<TokenSetT>::Type::Indent, TextPos(line, 0, line_start), pos - line_start));
                  is_indenting = false;
                  cur_token_start = pos;
               }
               else
               {
                  pos += 1;
                  continue;
               }
            }


            LexerResult res = handle_codepoint(character, cur_token_start, pos - cur_token_start, tokens, line, cur_token_start - line_start, pos - line_start);
            handle_lexer_result(res);

         }
         if (cur_token_start < pos && !is_indenting)
         {
            LexerResult res = terminate_codepoint(cur_token_start, pos - cur_token_start, tokens, line, cur_token_start - line_start, pos - line_start);
            handle_lexer_result(res);
         }
         // Ensure that there is a final end of line token in order for the presence of terminating newline to not affect compiler behaviour
         if (tokens.size() == 0 || tokens.back().type() != Token<TokenSetT>::Type::Linebreak)
         {
            tokens.push_back(Token<TokenSetT>(Token<TokenSetT>::Type::Linebreak, TextPos(line, cur_token_start - line_start, cur_token_start), 0));
         }
         tokens.push_back(Token<TokenSetT>(Token<TokenSetT>::Type::EndOfFile, TextPos(line, pos - line_start, pos), 0));
         return TokenizedText<TokenSetT>(text, tokens);
      }
   private:
      class CompletePattern
      {
      public:
         CompletePattern(size_t end_pos, const std::optional<Token<TokenSetT>>& token)
            : end_pos(end_pos)
            , token(token)
         {

         }

         size_t end_pos;
         std::optional<Token<TokenSetT>> token;
      };

      void reset()
      {
         for (size_t i = 0; i < sizeof...(PatternTs); ++i)
         {
            m_done[i] = false;
         }
         m_best = std::nullopt;
      }

      template<TokenPattern<TokenSetT> T>
      std::optional<Token<TokenSetT>> make_token(const T& pattern, size_t line, size_t col_start, size_t col_end, size_t string_index)
      {
         return pattern.make_token(line, col_start, col_end, string_index);
      }

      template<typename T>
      std::optional<Token<TokenSetT>> make_token(const T& pattern, size_t line, size_t col_start, size_t col_end, size_t string_index)
      {
         return std::nullopt;
      }

      template<std::size_t I = 0, typename... Tp>
      inline typename std::enable_if<I == sizeof...(PatternTs), LexerResult>::type
         handle_codepoint(UnicodeCodePoint cp, size_t token_start_index, size_t index, Tokens<TokenSetT>& tokens, size_t line, size_t col_start, size_t col_end)
      {
         return LexerResult(LexerResults::Continue, 0);
      }

      template<std::size_t I = 0, typename... Tp>
      inline typename std::enable_if < I < sizeof...(PatternTs), LexerResult>::type
         handle_codepoint(UnicodeCodePoint cp, size_t token_start_index, size_t index, Tokens<TokenSetT>& tokens, size_t line, size_t col_start, size_t col_end)
      {
         if (!m_done[I])
         {
            auto res = std::get<I>(m_pattern).check(cp, index);
            if (res.type == LexerResults::Completed)
            {
               auto token = make_token(std::get<I>(m_pattern), line, col_start, (col_end + 1 - res.backtrack_cols), token_start_index);
               if (m_best != std::nullopt && m_best->token != std::nullopt)
               {
                  if (token != std::nullopt && token->size() > m_best->token->size())
                  {
                     m_best = CompletePattern(index - res.backtrack_cols, token);
                  }
               }
               else
               {
                  m_best = CompletePattern(index - res.backtrack_cols, token);
               }
               m_done[I] = true;
            }
            else if (res.type == LexerResults::Failed)
            {
               m_done[I] = true;
            }
            if ((res.type == LexerResults::Completed || res.type == LexerResults::Failed) && all_done())
            {
               if (m_best != std::nullopt)
               {
                  auto& token = m_best->token;
                  if (token != std::nullopt)
                  {
                     tokens.push_back(*token);

                     return LexerResult(LexerResults::Completed, (1 + col_end) - col_start - token->size());
                  }
                  else
                  {
                     return LexerResult(LexerResults::Completed, index - m_best->end_pos);
                  }
               }
               return LexerResult(LexerResults::Failed, 0);

            }
         }
         return handle_codepoint<I + 1>(cp, token_start_index, index, tokens, line, col_start, col_end);
      }

      template<std::size_t I = 0, typename... Tp>
      inline typename std::enable_if<I == sizeof...(PatternTs), LexerResult>::type
         terminate_codepoint(size_t token_start_index, size_t index, Tokens<TokenSetT>& tokens, size_t line, size_t col_start, size_t col_end)
      {
         return LexerResult(LexerResults::Failed, 0);
      }

      template<std::size_t I = 0, typename... Tp>
      inline typename std::enable_if < I < sizeof...(PatternTs), LexerResult>::type
         terminate_codepoint(size_t token_start_index, size_t index, Tokens<TokenSetT>& tokens, size_t line, size_t col_start, size_t col_end)
      {
         if (!m_done[I])
         {
            auto res = std::get<I>(m_pattern).terminate(index);
            if (res.type == LexerResults::Completed)
            {
               auto token = make_token(std::get<I>(m_pattern), line, col_start, (col_end - res.backtrack_cols), token_start_index);
               if (m_best != std::nullopt && m_best->token != std::nullopt)
               {
                  if (token != std::nullopt && token->size() > m_best->token->size())
                  {
                     m_best = CompletePattern(index - res.backtrack_cols, token);
                  }
               }
               else
               {
                  m_best = CompletePattern(index - res.backtrack_cols, token);
               }
               m_done[I] = true;
            }
            else if (res.type == LexerResults::Failed)
            {
               m_done[I] = true;
            }
            if ((res.type == LexerResults::Completed || res.type == LexerResults::Failed) && all_done())
            {
               if (m_best != std::nullopt)
               {
                  auto& token = m_best->token;
                  if (token != std::nullopt)
                  {
                     tokens.push_back(*token);

                     return LexerResult(LexerResults::Completed, (col_end)-col_start - token->size());
                  }
                  else
                  {
                     return LexerResult(LexerResults::Completed, index - m_best->end_pos);
                  }
               }
               return LexerResult(LexerResults::Failed, 0);

            }
         }
         return terminate_codepoint<I + 1>(token_start_index, index, tokens, line, col_start, col_end);
      }

      bool all_done() const
      {
         for (size_t i = 0; i < (sizeof...(PatternTs)); ++i)
         {
            if (!m_done[i])
            {
               return false;
            }
         }
         return true;
      }

      std::tuple<PatternTs...> m_pattern;
      bool m_done[sizeof...(PatternTs)];
      std::optional<CompletePattern> m_best;
   };

   export template <TokenSet TokenSetT, LexerPattern... PatternTs>
   Lexer<TokenSetT, PatternTs...> create_lexer(PatternTs... patterns) 
   {
      return Lexer<TokenSetT, PatternTs...>(std::move(patterns)...);
   }


}