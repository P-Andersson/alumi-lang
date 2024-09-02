module;

#include <expected>
#include <vector>
#include <variant>
#include <string>
#include <optional>

export module alccemy.lexer.rules.indention;

import alccemy.lexer.concepts;
import alccemy.lexer.errors;
import alccemy.lexer.token;
import alccemy.lexer.text;
import alccemy.lexer.unicode;
import alccemy.lexer.rules.types;

export namespace alccemy 
{
   class MixedIndentionCharactersError {
   public:
      std::string description() const noexcept {
         return "Different indention character used for indention in the same source, these cannot be mixed.";
      }
   };

   class MismatchedIndentionError {
   public:
      std::string description() const noexcept {
         return "Dedention to an unknown indention layer";
      }
   };

   template <TokenSet TokenSetT, TokenSetT indention_token, TokenSetT dedention_token>
   class IndentionRule 
   {
   public:
      using ErrorType = LexerFailure<TokenSetT, std::variant<MixedIndentionCharactersError, MismatchedIndentionError>>;

      struct IndentionRuleState
      {
         std::optional<UnicodeCodePoint> current_indention_char;
         
         std::vector<size_t> indention_stack;
         std::optional<TextPos> indention_start;
         std::optional<size_t> current_indention;
      };

      IndentionRule(std::vector<UnicodeCodePoint>&& indention_chars = { 0x20, 0x09 } /*Space and Tab*/)
         : m_indention_chars(std::move(indention_chars))
      {

      }

      IndentionRuleState initial_state() const
      {
         auto state = IndentionRuleState();
         state.current_indention = 0; // Always start indenting
         state.indention_stack.push_back(0); // This should never be removed as we only pop from the stack if indention is less than the stacks value, which is impossible
         return state;
      }

      std::expected<RulesResult, ErrorType> handle_code_point(IndentionRuleState& state, Tokens<TokenSetT>& tokens, const UnicodeCodePoint& cp, const TextPos& position) const
      {
         // Newline means new indention
         if (cp == '\n')
         {
            state.current_indention = 0;
            state.indention_start = std::nullopt;
            return RulesResult::TerminateTokens; // We do not allow any token to break past indentions,
            // because we are not handling backtracking in this rule (yet)
         }

         if (state.current_indention && !state.indention_start)
         {
            state.indention_start = position;
         }

         if (state.current_indention && std::find(m_indention_chars.begin(), m_indention_chars.end(), cp) != m_indention_chars.end())
         {
            if (state.current_indention_char && cp != state.current_indention_char)
            {
               return std::unexpected(ErrorType(MixedIndentionCharactersError{}, tokens, position, 1));
            }
            else
            {
               state.current_indention_char = cp;
            }
            (*state.current_indention) += 1;
            return RulesResult::Consume;
         }
         if (state.current_indention)
         {
            if (state.indention_stack.back() != state.current_indention)
            {
               if (state.current_indention > state.indention_stack.back())
               {
                  tokens.push_back(Token<TokenSetT>(indention_token, state.indention_start.value(), state.current_indention.value()));
                  state.indention_stack.push_back(*state.current_indention);
               }
               else
               {
                  while (state.current_indention < state.indention_stack.back())
                  {
                     tokens.push_back(Token<TokenSetT>(dedention_token, state.indention_start.value(), state.current_indention.value()));
                     state.indention_stack.pop_back();
                  }
                  // Ensure matching ident
                  if (state.current_indention != state.indention_stack.back())
                  {
                     return std::unexpected(ErrorType(MismatchedIndentionError{}, tokens, position, 1));
                  }
               }
            }
            state.current_indention_char.reset();
            state.current_indention.reset();
         }
         return RulesResult::Continue;
      }

      std::expected<void, ErrorType> end_lexing(IndentionRuleState& state, Tokens<TokenSetT>& tokens, const TextPos& pos) const
      {
         for (size_t i = 0; i < state.indention_stack.size() - 1; ++i)
         {
            tokens.push_back(Token<TokenSetT>(dedention_token, pos, 0));
         }

         return std::expected<void, ErrorType>();
      }
   private:
      std::vector<UnicodeCodePoint> m_indention_chars;

   };
}

