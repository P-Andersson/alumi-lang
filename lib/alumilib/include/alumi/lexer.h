#pragma once

#include "alumi/lexer/lexer_detail.h"
#include "alumi/lexer/lexed_text.h"

#include <vector>
#include <optional>

namespace alumi 
{
   template<typename T>
   concept TokenPattern = requires(T & t, size_t line, size_t start, size_t end, size_t string_start_index) { t.make_token(line, start, end, string_start_index); };

   template <LexerPattern PatternT>
   class Tokenize
   {
   public:
      Tokenize(const PatternT& pattern, TokenType type)
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


      Token make_token(size_t line, size_t start, size_t end, size_t start_string_index) const
      {
         return Token(m_token_type, TextPos(line, start, start_string_index), end - start);
      }

   private:
      PatternT m_pattern;
      TokenType m_token_type;
   };

   class LexerFailure : public std::exception
   {
   public:
      enum class Reason
      {
         UnexpectedCodepoint,
         MismatchedIndentionCharacters
      };

      LexerFailure(Reason reason, const Tokens& tokens_so_far, const TextPos& text_pos, size_t data_index);

      const char* what() const noexcept override;

      Tokens tokens_so_far;
      TextPos text_pos;
      size_t data_index;
      Reason reason;

   private:
      std::string m_message;
   };

   ///
   /// Tokensizes one source
   ///
   /// 
   template <LexerPattern... PatternTs>
   class Lexer
   {
   public:
      Lexer(PatternTs... patterns)
         : m_pattern(patterns...)
      {
      }

      LexedText lex(const std::vector<UnicodeCodePoint>& text)
      {
         static const std::vector<UnicodeCodePoint> indention_chars({ 0x20, 0x09 });

         reset();

         bool is_indenting = true;
         size_t last_indent_chars = 0;
         std::optional<char> indent_char;

         Tokens tokens;
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
               throw LexerFailure(LexerFailure::Reason::UnexpectedCodepoint, tokens, TextPos(line, cur_token_start - line_start, cur_token_start), pos);
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

               tokens.push_back(Token(TokenType::Linebreak, TextPos(line, pos - line_start, pos), 1));
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
                     throw LexerFailure(LexerFailure::Reason::MismatchedIndentionCharacters, tokens, TextPos(line, pos - line_start, cur_token_start), pos);
                  }
                  tokens.push_back(Token(TokenType::Indent, TextPos(line, 0, line_start), pos - line_start));
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
         if (tokens.size() == 0 || tokens.back().type() != TokenType::Linebreak)
         {
            tokens.push_back(Token(TokenType::Linebreak, TextPos(line, cur_token_start - line_start, cur_token_start), 0));
         }
         tokens.push_back(Token(TokenType::EndOfFile, TextPos(line, pos - line_start, pos), 0));
         return LexedText(text, tokens);
      }
   private:
      class CompletePattern
      {
      public:
         CompletePattern(size_t end_pos, const std::optional<Token>& token)
            : end_pos(end_pos)
            , token(token)
         {

         }

         size_t end_pos;
         std::optional<Token> token;
      };

      void reset()
      {
         for (size_t i = 0; i < sizeof...(PatternTs); ++i)
         {
            m_done[i] = false;
         }
         m_best = std::nullopt;
      }

      template<TokenPattern T>
      std::optional<Token> make_token(const T& pattern, size_t line, size_t col_start, size_t col_end, size_t string_index)
      {
         return pattern.make_token(line, col_start, col_end, string_index);
      }

      template<typename T>
      std::optional<Token> make_token(const T& pattern, size_t line, size_t col_start, size_t col_end, size_t string_index)
      {
         return std::nullopt;
      }

      template<std::size_t I = 0, typename... Tp>
      inline typename std::enable_if<I == sizeof...(PatternTs), LexerResult>::type
         handle_codepoint(UnicodeCodePoint cp, size_t token_start_index, size_t index, Tokens& tokens, size_t line, size_t col_start, size_t col_end)
      {
         return LexerResult(LexerResults::Continue, 0);
      }

      template<std::size_t I = 0, typename... Tp>
      inline typename std::enable_if < I < sizeof...(PatternTs), LexerResult>::type
         handle_codepoint(UnicodeCodePoint cp, size_t token_start_index, size_t index, Tokens& tokens, size_t line, size_t col_start, size_t col_end)
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
         terminate_codepoint(size_t token_start_index, size_t index, Tokens& tokens, size_t line, size_t col_start, size_t col_end)
      {
         return LexerResult(LexerResults::Failed, 0);
      }

      template<std::size_t I = 0, typename... Tp>
      inline typename std::enable_if < I < sizeof...(PatternTs), LexerResult>::type
         terminate_codepoint(size_t token_start_index, size_t index, Tokens& tokens, size_t line, size_t col_start, size_t col_end)
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

                     return LexerResult(LexerResults::Completed, (col_end) - col_start - token->size());
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
}