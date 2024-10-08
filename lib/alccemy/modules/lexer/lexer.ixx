module;

#include <cassert>
#include <expected>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include <utf8cpp/utf8.h>

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

export class UnexpectedCodepointError {
 public:
   std::string description() const noexcept { return "Unexpected Codepoint"; }
};

export template <LexerPattern PatternT, TokenSet TokenSetT> class Tokenize {
 public:
   Tokenize(const PatternT& pattern, Token<TokenSetT>::Type type) : m_pattern(pattern), m_token_type(type) {}

   LexerResult check(UnicodeCodePoint cp, size_t index) { return m_pattern.check(cp, index); }

   LexerResult terminate(size_t index) { return m_pattern.terminate(index); }

   Token<TokenSetT> make_token(TextPos token_start, TextPos token_end) const {
      return Token<TokenSetT>(m_token_type, token_start, (token_end.text_index - token_start.text_index));
   }

 private:
   PatternT m_pattern;
   Token<TokenSetT>::Type m_token_type;
};

export template <LexerPattern PatternT, TokenSet TokenSetT>
Tokenize(const PatternT& pattern, TokenSetT type) -> Tokenize<PatternT, TokenSetT>;

export template <typename... RuleTs> using RuleSet = std::tuple<RuleTs...>;

export template <LexerPattern... PatternTs> using PatternSet = std::tuple<PatternTs...>;

template <typename... ErrorTypesT> struct ErrorTypes {
   using type = JoinedVariant<ErrorTypesT...>;
};

template <typename BasicErrorType, typename... RuleTypesT> struct ErrorTypes<BasicErrorType, RuleSet<RuleTypesT...>> {
   using type = JoinedVariant<BasicErrorType, decltype(RuleTypesT::ErrorType::error_type)...>;
};

export template <TokenSet TokenSetT, typename RuleTs = RuleSet<>, typename PatternTs = PatternSet<>> class Lexer {
 public:
   using ErrorType = LexerFailure<TokenSetT, typename ErrorTypes<std::variant<UnexpectedCodepointError>, RuleTs>::type>;

 private:
   using ExpectedRulesResultT = std::expected<RulesResult, ErrorType>;

 public:
   Lexer(RuleTs&& rules, PatternTs&& patterns) : m_rules(std::move(rules)), m_base_patterns(std::move(patterns)) {}

   Lexer(PatternTs&& patterns) : m_base_patterns(std::move(patterns)) {}

   std::expected<TokenizedText<TokenSetT>, ErrorType> lexUtf8(const std::string& text) const {
      return EncodingAwareLexer<step_utf8, step_back_utf8>().lex(m_base_patterns, m_rules, text);
   }

 private:
   RuleTs m_rules;
   PatternTs m_base_patterns;

   static UnicodeCodePoint step_utf8(const std::string& src_text, size_t& index) {
      auto ite = src_text.begin() + index;
      auto codepoint = utf8::next(ite, src_text.end());
      index = std::distance(src_text.begin(), ite);
      return codepoint;
   }

   static void step_back_utf8(const std::string& src_text, size_t& index) {
      auto ite = src_text.begin() + index;
      utf8::prior(ite, src_text.begin());
      index = std::distance(src_text.begin(), ite);
   }

 private:
   struct CodepointInText {
      CodepointInText(UnicodeCodePoint codepoint, const TextPos& pos) : codepoint(codepoint), pos(pos) {}

      UnicodeCodePoint codepoint;
      TextPos pos;
   };

   template <UnicodeCodePoint (*step_f)(const std::string&, size_t&), void (*step_back_f)(const std::string&, size_t&)>
   class EncodingAwareLexer {
    public:
      std::expected<TokenizedText<TokenSetT>, ErrorType> lex(const PatternTs& base_patterns, const RuleTs rules,
                                                             const std::string& src_text) const {
         PatternTs patterns = base_patterns;

         TokenizationState state;

         auto rules_states = create_rule_states(rules);

         std::vector<CodepointInText> current_token_components;

         auto pull_next = [&]() -> bool {
            if (state.text_position.text_index >= src_text.size()) {
               return false;
            }

            auto [next_pos, codepoint] = next(state.text_position, src_text);

            // Apply rules for each new character
            auto rules_results =
                tuple_for(rules, [&, this]<size_t... rule_indicies>(std::index_sequence<rule_indicies...>) {
                   ExpectedRulesResultT cur_result = RulesResult::Continue;
                   (
                       [&, this ]<size_t index = rule_indicies>() {
                          if (cur_result && cur_result.value() == RulesResult::Consume) {
                             return;
                          }

                          ExpectedRulesResultT res = std::get<index>(rules).handle_code_point(
                              std::get<index>(rules_states), state.tokens, codepoint, state.text_position);
                          if (!res || res.value() != RulesResult::Continue) {
                             cur_result = res;
                          }
                       }(),
                       ...);
                   return cur_result;
                });

            if (rules_results != RulesResult::Consume) {
               current_token_components.push_back(CodepointInText(codepoint, state.text_position));
            }
            state.text_position = next_pos;

            return true;
         };

         auto apply_lexer_result = [&](auto& pattern, LexerResult res, size_t& current_codepoint_index) -> bool {
            if (res.type == LexerResults::Completed) {
               // Note, we backtrack from next position because we want the
               // position right after the backtrack
               auto end_index = current_codepoint_index + 1 - (res.backtrack_cols);
               auto end_pos = state.text_position;
               if (end_index < current_token_components.size()) {
                  end_pos = current_token_components[end_index].pos;
               }

               auto token = state.make_token(pattern, end_pos, state.current_token_start);
               if (state.best && state.best->token != std::nullopt) {
                  // We only take the largest token, or the one that occurs first
                  // if there multiple possible, so we get a well-defined
                  // consistent behavior
                  if (token && token->size() > state.best->token->size()) {
                     state.best = CompletePattern(end_pos, token);
                  }
               } else {
                  state.best = CompletePattern(end_pos, token);
               }
               return true;
            } else if (res.type == LexerResults::Failed) {
               return true;
            } else {
               current_codepoint_index += 1 - res.backtrack_cols;
               return false;
            }
         };

         while (state.text_position.text_index < src_text.size()) {
            tuple_for_each(patterns, [&, this](auto& pattern, size_t patter_index) {
               bool done = false;

               size_t current_codepoint = 0;
               while (!done) {
                  while (!done && current_codepoint < current_token_components.size()) {
                     auto codepoint = current_token_components[current_codepoint].codepoint;
                     auto res = pattern.check(codepoint, current_codepoint);
                     done = apply_lexer_result(pattern, res, current_codepoint);
                  }
                  if (!done && !pull_next()) {
                     auto res = pattern.terminate(current_codepoint);
                     apply_lexer_result(pattern, res, current_codepoint);
                     return;
                  }
               }
            });
            if (state.best) {
               auto& token = state.best->token;
               if (token != std::nullopt) {
                  state.tokens.push_back(*token);
               }
               current_token_components.erase(
                   std::remove_if(current_token_components.begin(), current_token_components.end(),
                                  [&](const auto& comp) { return comp.pos < state.best->end_pos; }),
                   current_token_components.end());
               if (current_token_components.size() > 0) {
                  state.current_token_start = current_token_components.front().pos;
               } else {
                  state.current_token_start = state.text_position;
               }

               state.reset_for_next_token();
            } else {
               return std::unexpected(ErrorType(UnexpectedCodepointError(), state.tokens, state.current_token_start,
                                                state.text_position.text_index));
            }
         }

         // TODO Terminate rules

         /*
         while (state.text_position.text_index < src_text.size()) {
            auto [next_pos, codepoint] = next(state.text_position, src_text);

            auto rules_results =
                tuple_for(rules, [&, this]<size_t... rule_indicies>(std::index_sequence<rule_indicies...>) {
                   ExpectedRulesResultT cur_result = RulesResult::Continue;
                   (
                       [&, this ]<size_t index = rule_indicies>() {
                          if (cur_result && cur_result.value() == RulesResult::Consume) {
                             return;
                          }

                          ExpectedRulesResultT res = std::get<index>(rules).handle_code_point(
                              std::get<index>(rules_states), state.tokens, codepoint, state.text_position);
                          if (!res || res.value() != RulesResult::Continue) {
                             cur_result = res;
                          }
                       }(),
                       ...);
                   return cur_result;
                });

            if (!rules_results) {
               return std::unexpected(rules_results.error());
            }

            if (rules_results != RulesResult::Consume) {
               tuple_for_each(patterns, [&](auto& pattern, size_t patter_index) {
                  if (!state.done[patter_index]) {
                     state.apply_lexer_result(pattern.check(codepoint, state.consumed_cps_for_current_token), next_pos,
                                              state.current_token_start, src_text, pattern, patter_index);
                  }
               });
            }

            if (next_pos.text_index == src_text.size() || rules_results != RulesResult::Continue) {
               // Ignore if we have consumed the first token
               if (rules_results != RulesResult::Consume || state.consumed_cps_for_current_token > 0) {
                  // Inform currently acttive patterns that they are all done
                  // parsing, letting them fail or complete as relevant
                  tuple_for_each(patterns, [&](auto& pattern, size_t patter_index) {
                     if (!state.done[patter_index]) {
                        state.apply_lexer_result(pattern.terminate(state.consumed_cps_for_current_token + 1), next_pos,
                                                 state.current_token_start, src_text, pattern, patter_index);
                     }
                  });
               }
            }

            if (rules_results != RulesResult::Consume) {
               state.consumed_cps_for_current_token += 1;
            }

            if (state.all_done()) {
               if (state.best) {
                  auto& token = state.best->token;
                  if (token != std::nullopt) {
                     state.tokens.push_back(*token);
                  }
                  next_pos = state.best->end_pos;
                  state.consumed_cps_for_current_token = 0;

                  state.reset_for_next_token();
               } else {
                  return std::unexpected(ErrorType(UnexpectedCodepointError(), state.tokens, state.current_token_start,
                                                   state.text_position.text_index));
               }
            }

            if (next_pos.line > state.text_position.line) {
               state.line_length_stack.push_back(state.text_position.col);
            }
            state.text_position = next_pos;

            if (state.consumed_cps_for_current_token == 0) {
               state.current_token_start = state.text_position;
            }
         }
         // Finalize rules
         tuple_for(rules, [&, this]<size_t... rule_indicies>(std::index_sequence<rule_indicies...>) {
            (
                [&, this ]<size_t index = rule_indicies>() {
                   std::get<index>(rules).end_lexing(std::get<index>(rules_states), state.tokens, state.text_position);
                }(),
                ...);
         });
         */
         // Always append an end of file token here
         state.tokens.push_back(Token<TokenSetT>(Token<TokenSetT>::Type::EndOfFile, state.text_position, 0));

         return TokenizedText(state.tokens);
      }

      class CompletePattern {
       public:
         CompletePattern(TextPos end_pos, const std::optional<Token<TokenSetT>>& token)
             : end_pos(end_pos), token(token) {}

         TextPos end_pos;
         std::optional<Token<TokenSetT>> token;
      };

      template <typename... RuleTs> static auto create_rule_states(const std::tuple<RuleTs...>& rules) {
         return tuple_for(rules, [&]<size_t... indicies>(std::index_sequence<indicies...>) {
            return std::make_tuple(std::get<indicies>(rules).initial_state()...);
         });
      }

      class TokenizationState {
       public:
         TokenizationState() {
            for (size_t i = 0; i < std::tuple_size_v<PatternTs>; ++i) {
               done[i] = false;
            }
         }

         bool all_done() const {
            for (size_t i = 0; i < std::tuple_size_v<PatternTs>; ++i) {
               if (!done[i]) {
                  return false;
               }
            }
            return true;
         }

         void reset_for_next_token() {
            for (size_t i = 0; i < std::tuple_size_v<PatternTs>; ++i) {
               done[i] = false;
            }
            best = std::nullopt;
         }

         template <LexerPattern PatternT>
         void apply_lexer_result(const LexerResult& res, const TextPos& next_position,
                                 const TextPos& current_token_start, const std::string& source_text,
                                 const PatternT& pattern, size_t pattern_index) {
            if (res.type == LexerResults::Completed) {
               // Note, we backtrack from next position because we want the
               // position right after the backtrack
               auto backtracked = backtrack(next_position, res.backtrack_cols, line_length_stack, source_text);
               auto token = make_token(pattern, backtracked, current_token_start);
               if (best && best->token != std::nullopt) {
                  // We only take the largest token, or the one that occurs first
                  // if there multiple possible, so we get a well-defined
                  // consistent behavior
                  if (token && token->size() > best->token->size()) {
                     best = CompletePattern(backtracked, token);
                  }
               } else {
                  best = CompletePattern(backtracked, token);
               }
               done[pattern_index] = true;
            } else if (res.type == LexerResults::Failed) {
               done[pattern_index] = true;
            } else if (res.type == LexerResults::Continue && res.backtrack_cols > 0) {
            }
         }

       public:
         std::vector<size_t> line_length_stack;
         bool done[std::tuple_size_v<PatternTs>];
         std::optional<CompletePattern> best;

         Tokens<TokenSetT> tokens;
         TextPos text_position = TextPos(0, 0, 0);
         TextPos current_token_start = TextPos(0, 0, 0);
         size_t consumed_cps_for_current_token = 0;

       public:
         TextPos backtrack(const TextPos& pos, size_t count, const std::vector<size_t>& line_length_stack,
                           const std::string& source_text) {
            assert(count <= pos.text_index);

            TextPos new_pos = pos;
            // Note immediate skip backwards, this position will be inconsistent
            // until function return
            for (size_t i = 0; i < count; ++i) {
               step_back_f(source_text, new_pos.text_index);
            }

            while (count > new_pos.col) {
               count -= new_pos.col;
               new_pos.line -= 1;
               new_pos.col = line_length_stack.at(new_pos.line);
            }
            new_pos.col -= count;
            return new_pos;
         }

         template <TokenPattern<TokenSetT> T>
         std::optional<Token<TokenSetT>> make_token(const T& pattern, TextPos end_pos, TextPos token_start_pos) {
            return pattern.make_token(token_start_pos, end_pos);
         }

         template <typename T>
         std::optional<Token<TokenSetT>> make_token(const T& pattern, TextPos end_pos, TextPos token_start_pos) {
            return std::nullopt;
         }
      };

      static std::tuple<TextPos, UnicodeCodePoint> next(const TextPos& pos, const std::string& text) {
         TextPos new_pos = pos;
         auto cp = step_f(text, new_pos.text_index);
         if (text[pos.text_index] == '\n') {
            new_pos.col = 0;
            new_pos.line += 1;
         } else {
            new_pos.col += 1;
         }
         return std::make_tuple(new_pos, cp);
      }
   };
};

export template <TokenSet TokenSetT, typename RuleTs = RuleSet<>, typename PatternTs = PatternSet<>>
Lexer<TokenSetT, RuleTs, PatternTs> create_lexer(PatternTs&& patterns) {
   return Lexer<TokenSetT, RuleTs, PatternTs>(std::move(patterns));
}

export template <TokenSet TokenSetT, typename RuleTs = RuleSet<>, typename PatternTs = PatternSet<>>
Lexer<TokenSetT, RuleTs, PatternTs> create_lexer(RuleTs&& rules, PatternTs&& patterns) {
   return Lexer<TokenSetT, RuleTs, PatternTs>(std::move(rules), std::move(patterns));
}

} // namespace alccemy
