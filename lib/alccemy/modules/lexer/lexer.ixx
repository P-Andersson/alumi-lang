module;

#include <expected>
#include <string>
#include <vector>
#include <optional>
#include <cassert>
#include <variant>

export module alccemy.lexer;

export import alccemy.lexer.concepts;
export import alccemy.lexer.errors;
export import alccemy.lexer.patterns;
export import alccemy.lexer.text;
export import alccemy.lexer.token;
export import alccemy.lexer.tokenized_text;
export import alccemy.lexer.unicode;

export import alccemy.lexer.rules;

import alccemy.util.tuple;
import alccemy.util.variant;

namespace alccemy {

   export class UnexpectedCodepointError
   {
   public:
      std::string description() const noexcept
      {
         return "Unexpected Codepoint";
      }
   };

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


      Token<TokenSetT> make_token(TextPos token_start, TextPos token_end) const
      {
         return Token<TokenSetT>(m_token_type, token_start, (token_end.text_index - token_start.text_index) + 1);
      }

   private:
      PatternT m_pattern;
      Token<TokenSetT>::Type m_token_type;
   };
   

   export template <LexerPattern PatternT, TokenSet TokenSetT>
   Tokenize(const PatternT& pattern, TokenSetT type) -> Tokenize<PatternT, TokenSetT>;


   export template<typename... RuleTs>
   using RuleSet = std::tuple<RuleTs...>;

   export template<LexerPattern... PatternTs>
   using PatternSet = std::tuple<PatternTs...>;

   template<typename... ErrorTypesT>
   struct ErrorTypes { using type = JoinedVariant<ErrorTypesT...>; };

   template<typename BasicErrorType, typename... RuleTypesT>
   struct ErrorTypes<BasicErrorType, RuleSet<RuleTypesT...>> {
      using type = JoinedVariant<BasicErrorType,
         decltype(RuleTypesT::ErrorType::error_type)... > ;
   };
   
   export template <TokenSet TokenSetT, typename RuleTs = RuleSet<>, typename PatternTs = PatternSet<>>
   class Lexer
   {
   public:
      using ErrorType = LexerFailure<TokenSetT, typename ErrorTypes<std::variant<UnexpectedCodepointError>, RuleTs>::type>;
   private:
      using ExpectedRulesResultT = std::expected<RulesResult, ErrorType>;
   public:

      Lexer(RuleTs&& rules, PatternTs&& patterns)
         : m_rules(std::move(rules))
         , m_pattern(std::move(patterns))
      {
      }

      Lexer(PatternTs&& patterns)
         : m_pattern(std::move(patterns))
      {
      }

      std::expected<TokenizedText<TokenSetT>, ErrorType> lex(const std::vector<UnicodeCodePoint>& text)
      {
         Tokens<TokenSetT> tokens;

         TokenizationState state;

         TextPos text_position(0, 0, 0);
         TextPos current_token_start(0, 0, 0);
         size_t consumed_cps_for_current_token = 0;

         auto rules_states = create_rule_states(m_rules);

         while(text_position.text_index < text.size())
         {
            auto rules_results = tuple_for(m_rules, [&, this]<size_t... rule_indicies>(std::index_sequence<rule_indicies...>)
            {
               ExpectedRulesResultT cur_result = RulesResult::Continue;
               ([&, this]<size_t index = rule_indicies>()
                  {
                     if (cur_result && cur_result.value() == RulesResult::Consume)
                     {
                        return;
                     }

                     ExpectedRulesResultT res = std::get<index>(m_rules).handle_code_point(std::get<index>(rules_states),
                        tokens, text[text_position.text_index], text_position);
                     if (!res || res.value() != RulesResult::Continue)
                     {
                        cur_result = res;
                     }
                  }
               (), ...);
               return cur_result;
            });

            if (!rules_results) {
               return std::unexpected(rules_results.error());
            }

            if (rules_results != RulesResult::Consume)
            {
               tuple_for_each(m_pattern, [&](auto& pattern, size_t patter_index)
                  {
                     if (!state.done[patter_index])
                     {
                        state.apply_lexer_result(pattern.check(text[text_position.text_index], consumed_cps_for_current_token),
                           text_position,
                           current_token_start,
                           pattern,
                           patter_index);
                     }
                  }
               );
            }

            if (text_position.text_index + 1 == text.size() || rules_results != RulesResult::Continue)
            {
               // Ignore if we have consumed the first token
               if (rules_results != RulesResult::Consume || consumed_cps_for_current_token > 0)
               {
                  // Inform currently acttive patterns that they are all done parsing, letting them fail or complete as relevant
                  tuple_for_each(m_pattern, [&](auto& pattern, size_t patter_index)
                     {
                        if (!state.done[patter_index])
                        {
                           state.apply_lexer_result(pattern.terminate(consumed_cps_for_current_token + 1),
                              text_position,
                              current_token_start,
                              pattern,
                              patter_index);
                        }
                     }
                  );
               }

            }
            
            if (rules_results != RulesResult::Consume)
            {
               consumed_cps_for_current_token += 1;
            }

            if (state.all_done()) {
               if (state.best)
               {
                  auto& token = state.best->token;
                  if (token != std::nullopt)
                  {
                     tokens.push_back(*token);
                  }
                  text_position = state.best->end_pos;
                  //current_token_start = increment_pos(text_position, text);
                  consumed_cps_for_current_token = 0;
                  
                  state.reset_for_next_token();
               }
               else
               {
                  return std::unexpected(ErrorType(UnexpectedCodepointError(), tokens, current_token_start, text_position.text_index));
               }
            }
            auto new_position = increment_pos(text_position, text);
            if (new_position.line > text_position.line) {
               state.line_length_stack.push_back(text_position.col);
            }
            text_position = new_position;

            if (consumed_cps_for_current_token == 0) {
               current_token_start = text_position;
            }
         }
         // Finalize rules
         tuple_for(m_rules, [&, this]<size_t... rule_indicies>(std::index_sequence<rule_indicies...>)
         {
            ([&, this]<size_t index = rule_indicies>()
            {
               std::get<index>(m_rules).end_lexing(std::get<index>(rules_states),
                  tokens, text_position);
            }
            (), ...);
         });
  
         // Always append an end of file token here
         tokens.push_back(Token<TokenSetT>(Token<TokenSetT>::Type::EndOfFile, text_position, 0));
                  
         return TokenizedText(text, tokens);

      }
   private:
      class CompletePattern
      {
      public:
         CompletePattern(TextPos end_pos, const std::optional<Token<TokenSetT>>& token)
            : end_pos(end_pos)
            , token(token)
         {

         }

         TextPos end_pos;
         std::optional<Token<TokenSetT>> token;
      };

      template<typename... RuleTs>
      static auto create_rule_states(const std::tuple<RuleTs...>& rules) {
         return tuple_for(rules, [&]<size_t... indicies>(std::index_sequence<indicies...>)
         {
            return std::make_tuple(std::get<indicies>(rules).initial_state()...);
         });
      }

      class TokenizationState {
      public: 
         TokenizationState() 
         {
            for (size_t i = 0; i < std::tuple_size_v<PatternTs>; ++i)
            {
               done[i] = false;
            }
         }

         bool all_done() const
         {
            for (size_t i = 0; i < std::tuple_size_v<PatternTs>; ++i)
            {
               if (!done[i])
               {
                  return false;
               }
            }
            return true;
         }

         void reset_for_next_token()
         {
            for (size_t i = 0; i < std::tuple_size_v<PatternTs>; ++i)
            {
               done[i] = false;
            }
            best = std::nullopt;
         }

         template<LexerPattern PatternT>
         void apply_lexer_result(const LexerResult& res, const TextPos& text_position, const TextPos& current_token_start, const PatternT& pattern, size_t pattern_index)
         {
            if (res.type == LexerResults::Completed)
            {
               auto backtracked = backtrack(text_position, res.backtrack_cols, line_length_stack);
               auto token = make_token(pattern, backtracked, current_token_start);
               if (best && best->token != std::nullopt)
               {
                  // We only take the largest token, or the one that occurs first if there multiple possible, 
                  // so we get a well-defined consistent behavior
                  if (token && token->size() > best->token->size())
                  {
                     best = CompletePattern(backtracked, token);
                  }
               }
               else
               {
                  best = CompletePattern(backtracked, token);
               }
               done[pattern_index] = true;
            }
            else if (res.type == LexerResults::Failed)
            {
               done[pattern_index] = true;
            }
         }

         std::vector<size_t> line_length_stack;
         bool done[std::tuple_size_v<PatternTs>];
         std::optional<CompletePattern> best;

      private:

         TextPos backtrack(const TextPos& pos, size_t count, const std::vector<size_t>& line_length_stack)
         {
            assert(count <= pos.text_index);

            TextPos new_pos = pos;
            new_pos.text_index -= count; //Note immediate skip backwards, this position will be inconsistent until function return
            while (count > new_pos.col) {
               count -= new_pos.col;
               new_pos.line -= 1;
               new_pos.col = line_length_stack.at(new_pos.line);
            }
            new_pos.col -= count;
            return new_pos;
         }

         template<TokenPattern<TokenSetT> T>
         std::optional<Token<TokenSetT>> make_token(const T& pattern, TextPos current_pos, TextPos token_start_pos)
         {
            return pattern.make_token(token_start_pos, current_pos);
         }

         template<typename T>
         std::optional<Token<TokenSetT>> make_token(const T& pattern, TextPos current_pos, TextPos token_start_pos)
         {
            return std::nullopt;
         }
      };

      TextPos increment_pos(const TextPos& pos, const std::vector<UnicodeCodePoint>& text) {
         TextPos new_pos = pos;
         new_pos.text_index += 1;
         if (text[pos.text_index] == '\n') {
            new_pos.col = 0;
            new_pos.line += 1;
         }
         else {
            new_pos.col += 1;
         }
         return new_pos;
      }

      RuleTs m_rules;
      PatternTs m_pattern;
   };

   export template <TokenSet TokenSetT, typename RuleTs = RuleSet<>, typename PatternTs = PatternSet<>>
   Lexer<TokenSetT, RuleTs, PatternTs> create_lexer(PatternTs &&patterns)
   {
      return Lexer<TokenSetT, RuleTs, PatternTs>(std::move(patterns));
   }

   export template <TokenSet TokenSetT, typename RuleTs = RuleSet<>, typename PatternTs = PatternSet<>>
      Lexer<TokenSetT, RuleTs, PatternTs> create_lexer(RuleTs&& rules, PatternTs&& patterns)
   {
      return Lexer<TokenSetT, RuleTs, PatternTs>(std::move(rules), std::move(patterns));
   }


}