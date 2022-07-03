#pragma once

#include "alumi/parser/parser_parts.h"

#include <concepts>
#include <type_traits>

namespace alumi
{
   namespace parser
   {

      class NeverSynchroize 
      {
      public:
         static void do_synch(Subparser& parser);
      };


      template <TokenType opener, TokenType closer>
      class SynchronzieOnMatchedPair 
      {
      public:
         static void do_synch(Subparser& parser)
         {
            parser.restart_parsing();
            size_t imbalance = 1;
            while(true)
            {
               Token token = parser.advance();
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
            parser.restart_parsing();
            while (true)
            {
               Token token = parser.advance();
               if (token.type() == TokenType::EndOfFile)
               {
                  return;
               }
               if (token.type() == closer)
               {
                  parser.clear_panic();
                  return;
               }
            }
         }
      };



      template <typename T>
      concept SynchronizeT = requires (Subparser& parser){ T::do_synch(parser);  };

      template<ParserElement T, SynchronizeT SynchT, std::optional<syntax_tree::Node> (*FuncT)(const ParseResult&)>
      class ParseRule
      {
      public:
         static ParseResult parse(Subparser& parser)
         {
            Subparser child = parser.create_child();
            ParseResult res = T::parse(child);
            
            syntax_tree::Nodes nodes;
            if (!res.get_subparser().is_panicing())
            {
               auto node_opt = FuncT(res);
               if (node_opt != std::nullopt)
               {
                  nodes.push_back(*node_opt);
               }
            }

            nodes.insert(nodes.end(), res.get_nodes().begin(), res.get_nodes().end());
            parser.use_state(res.get_subparser());
            if (res.get_type() == ParseResult::Type::Failure)
            {
               parser.do_panic();
               SynchT::do_synch(parser);
               if (!parser.is_panicing())
               {
                  return ParseResult(ParseResult::Type::RecoveredFailure, parser, nodes);
               }
            }            
            return ParseResult(res.get_type(), parser, nodes);
         }
      };
   }

}