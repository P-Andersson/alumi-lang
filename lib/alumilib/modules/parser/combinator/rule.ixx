module;

#include "alumi/lexer/token.h"

#include <optional>

export module alumi.parser.combinator:rule;

import :concepts;

import alumi.parser.result;
import alumi.parser.subparser;
import alumi.syntax_tree.nodes;

using namespace alumi;

export {

   class NeverSynchroize
   {
   public:
      static void do_synch(Subparser& parser)
      {
         return;
      }
   };


   template <TokenType opener, TokenType closer>
   class SynchronzieOnMatchedPair
   {
   public:
      static void do_synch(Subparser& parser)
      {
         Subparser synch_parser = parser;
         synch_parser.restart_parsing();
         size_t imbalance = 1;
         while (true)
         {
            Token token = synch_parser.advance();
            if (token.type() == TokenType::EndOfFile)
            {
               return;
            }
            if (token.type() == opener)
            {
               imbalance += 1;
            }
            if (token.type() == closer)
            {
               imbalance -= 1;
               if (imbalance <= 1)
               {
                  parser.take_over_from(synch_parser);
                  parser.clear_panic();
                  return;
               }
            }

         }
      }
   };

   template <TokenType closer>
   class SynchronizeOnToken
   {
   public:
      static void do_synch(Subparser& parser)
      {
         Subparser synch_parser = parser;
         synch_parser.restart_parsing();
         while (true)
         {
            Token token = synch_parser.advance();
            if (token.type() == TokenType::EndOfFile)
            {
               return;
            }
            if (token.type() == closer)
            {
               parser.take_over_from(synch_parser);
               parser.clear_panic();
               return;
            }
         }
      }
   };



   template <typename T>
   concept SynchronizeT = requires (Subparser & parser) { T::do_synch(parser); };

   template<ParserElement T, SynchronizeT SynchT, std::optional<Node>(*FuncT)(const Result&)>
   class ParseRule
   {
   public:
      static Result parse(Subparser& parser)
      {
         Subparser child = parser.create_child();
         Result res = T::parse(child);

         Nodes nodes;
         auto node_opt = FuncT(res);
         if (node_opt.has_value())
         {
            nodes.push_back(*node_opt);
         }


         nodes.insert(nodes.end(), res.get_nodes().begin(), res.get_nodes().end());
         parser.take_over_from(res.get_subparser());
         if (res.get_type() == Result::Type::Failure)
         {
            parser.do_panic(res.get_subparser().start_token_index());
            SynchT::do_synch(parser);
            if (!parser.is_panicing())
            {
               return Result(Result::Type::RecoveredFailure, parser, nodes);
            }
         }
         return Result(res.get_type(), parser, nodes);
      }
   };

}