#pragma once

#include "alumi/lexer/token.h"
#include "alumi/syntax_tree/nodes.h"

#include <variant>
#include <vector>
#include <unordered_set>
#include <optional>
#include <memory>
#include <type_traits>
#include <concepts>

namespace alumi
{
   namespace parser
   {

      //!
      //! Represents a parsing view of a single token stream
      //! 
      //! 
      class Subparser
      {
      public:
         Subparser(std::vector<Token>& tokens);

         Subparser create_child() const;

         Token advance();

         Token peek();

         //!
         //! Copies the state of a child-parser that should not be scope-limited to a sub-expression,
         //! such current token and the indention data
         //! 
         void use_state(const Subparser& parser);

         size_t get_distance() const;

         std::optional<size_t> get_indent() const;

         //!
         //! Marks this parser, along with further children, 
         //! to swallow and ignore further tokens of this type
         //! 
         void add_swallowed_token(TokenType type);

      private:
         std::vector<Token>* m_token_source;
         std::vector<size_t> m_indent_stack;

         size_t m_start;
         size_t m_current;
         std::vector<TokenType> m_swallowed;

      };

 

      //!
      //! Describes the results of a specific parsing attemp t
      //! 
      class ParseResult
      {
      public:
         enum class Type
         {
            Failure,
            Success
         };

         ParseResult(Type type,
            const Subparser& parser,
            const syntax_tree::Nodes& parsed_nodes);

         Type get_type() const;

         const Subparser& get_subparser() const;

         size_t get_consumed() const;

         const syntax_tree::Nodes& get_nodes() const;

      private:
         Type m_type;

         Subparser m_parser_used;

         syntax_tree::Nodes m_nodes;

      };

      template<typename T>
      concept ParserElement = requires (T& element, Subparser& parent) {
         true; //element.parse(parent); // If enabled, forward declared types cannot be used and that makes recurisve rules impossible
      };
      
      template<ParserElement T>
      class Peek
      {
      public:
         static ParseResult parse(Subparser& parent)
         {
            Subparser child = parent.create_child();
            auto res = T::parse(child);
            if (res.get_type() == ParseResult::Type::Success)
            {
               return ParseResult(ParseResult::Type::Success, parent, {});
            }
            return ParseResult(ParseResult::Type::Failure, parent, {});
         }
      private:
      };
      
      template<TokenType token_type>
      class Is
      {
      public:
         static ParseResult parse(Subparser& parent)
         {
            if (parent.advance().type() == token_type)
            {
               return ParseResult(ParseResult::Type::Success, parent, {});
            }
            return ParseResult(ParseResult::Type::Failure, parent, {});
         }
      private:
      };

      template<ParserElement T, TokenType swallowed>
      class Swallow
      {
      public:
         static ParseResult parse(Subparser& parent)
         {
            Subparser parser = parent.create_child();
            parser.add_swallowed_token(swallowed);
            return T::parse(parser);

         }
      private:
      };


      template<ParserElement T>
      class Optional 
      {
      public:
         static ParseResult parse(Subparser& parent)
         {
            Subparser parser = parent.create_child();
            auto res = T::parse(parser);
            if (res.get_type() == ParseResult::Type::Failure)
            {
               return ParseResult(ParseResult::Type::Success, parent, {});
            }
            else
            {
               parent.use_state(res.get_subparser());
               return ParseResult(ParseResult::Type::Success, parent, res.get_nodes());
            }
         }

      private:
      };

      template<ParserElement T>
      class Repeats
      {
      public:
         static ParseResult parse(Subparser& parent)
         { 

            syntax_tree::Nodes child_nodes;
            while (true)
            {
               Subparser parser = parent.create_child();
               ParseResult res = T::parse(parser);
               if (res.get_type() == ParseResult::Type::Failure)
               {
                  return ParseResult(ParseResult::Type::Success, parent, child_nodes);
               }
               else
               {
                  child_nodes.insert(child_nodes.end(), res.get_nodes().begin(), res.get_nodes().end());
                  parent.use_state(res.get_subparser());
               }
            }
         }

      private:

      };

      template<ParserElement T, TokenType seperator>
      class RepeatsWithSeperator
      {
      public:
         static ParseResult parse(Subparser& parent)
         {
            size_t repeats = 0;

            syntax_tree::Nodes child_nodes;
            while (true)
            {
               Subparser parser = parent.create_child();
               ParseResult res = T::parse(parser);
               if (res.get_type() == ParseResult::Type::Failure)
               {
                  return ParseResult(ParseResult::Type::Success, parent, child_nodes);
               }
               else
               {
                  child_nodes.insert(child_nodes.end(), res.get_nodes().begin(), res.get_nodes().end());
                  parent.use_state(res.get_subparser());
                  if (parent.peek().type() == seperator)
                  {
                     parent.advance();
                  }
                  else
                  {
                     return ParseResult(ParseResult::Type::Success, parent, child_nodes);
                  }
               }
            }
         }
      private:
      };

      template<ParserElement... Ts>
      class Sequence
      {
      public:
         static ParseResult parse(Subparser& parent)
         {
            syntax_tree::Nodes child_nodes;
            ParseResult res = do_parse<Ts...>(parent, child_nodes);
            return ParseResult(res.get_type(), parent, child_nodes);
         }
      private:

         template<ParserElement ElemT>
         static ParseResult do_parse(Subparser& parent, syntax_tree::Nodes& child_nodes)
         {
            ParseResult res = ElemT::parse(parent);
            child_nodes.insert(child_nodes.end(), res.get_nodes().begin(), res.get_nodes().end());

            return res;
         }

         template<ParserElement ElemT, ParserElement NextT, ParserElement... OthersT>
         static ParseResult do_parse(Subparser& parent, syntax_tree::Nodes& child_nodes)
         {
            ParseResult res = do_parse<ElemT>(parent, child_nodes);
            if (res.get_type() != ParseResult::Type::Failure)
            {
               return do_parse<NextT, OthersT...>(parent, child_nodes);
            }
            else
            {
               return ParseResult(ParseResult::Type::Failure, parent, child_nodes);
            }
         }

      };

      //!
      //! Expect an Indent token, that indents to a greater than the previous line
      //! 
      class Indented
      {
      public:
         static ParseResult parse(Subparser& parent);
      private:
      };

      //!
      //! Expect an Indent token, that indents to a lesser than the previous line
      //! 
      class Dedented
      {
      public:
         static ParseResult parse(Subparser& parent);
      private:
      };

      //!
      //! Expect an Indent token, that indents to the same level as previous line
      //! 
      class NoIndent
      {
      public:
         static ParseResult parse(Subparser& parent);
      private:
      };


      template<ParserElement... Ts>
      class AnyOf
      {
      public:
         static ParseResult parse(Subparser& parent)
         {
            std::optional<ParseResult> best_failure;
            ParseResult res = do_parse<Ts...>(parent, best_failure);
            if (res.get_type() == ParseResult::Type::Failure)
            {
               res = *best_failure;
            }
            parent.use_state(res.get_subparser());
            return ParseResult(res.get_type(), parent, res.get_nodes());

         }
      private:

         template<ParserElement ElemT>
         static ParseResult do_parse(Subparser& parent, std::optional<ParseResult>& best_failure)
         {
            Subparser parser = parent.create_child();
            ParseResult res = ElemT::parse(parser);
            if (res.get_type() == ParseResult::Type::Failure)
            {
               if (best_failure == std::nullopt || best_failure->get_consumed() < res.get_consumed())
               {
                  best_failure = res;
               }
            }
            return res;
         }

         template<ParserElement ElemT, ParserElement NextT, ParserElement... OthersT>
         static ParseResult do_parse(Subparser& parent, std::optional<ParseResult>& best_failure)
         {
            ParseResult res = do_parse<ElemT>(parent, best_failure);
            if (res.get_type() == ParseResult::Type::Failure)
            {
               return do_parse<NextT, OthersT...>(parent, best_failure);
            }
            else
            {
               return res;
            }
         }

      };
   }
}