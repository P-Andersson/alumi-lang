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

      class Subparser
      {
      public:
         Subparser(std::vector<Token>& tokens);

         Subparser create_child() const;

         Token advance();

         Token peek();

         void skip_forward(size_t steps);

         size_t get_distance() const;

         void add_swallowed_token(TokenType type);

      private:
         std::vector<Token>* m_token_source;

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
         true; //element.parse(parent); // Todo disables recursive tri
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
               parent.skip_forward(res.get_consumed());
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
                  parent.skip_forward(res.get_consumed());
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
                  parent.skip_forward(res.get_consumed());
                  if (parent.peek().type() == seperator)
                  {
                     parent.skip_forward(1);
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
            parent.skip_forward(res.get_consumed());
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