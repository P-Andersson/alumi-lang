#pragma once

#include "alumi/parser/parser_parts.h"

#include <concepts>
#include <type_traits>

namespace alumi
{
   namespace parser
   {

      template<ParserElement T, syntax_tree::Node (*FuncT)(const ParseResult&)>
      class ParseRule
      {
      public:
         static ParseResult parse(Subparser& parser)
         {
            ParseResult res = T::parse(parser);
            
            syntax_tree::Nodes nodes = { FuncT(res) };
            nodes.insert(nodes.end(), res.get_nodes().begin(), res.get_nodes().end());
            return ParseResult(res.get_type(), res.get_subparser(), nodes);
         }
      };
   }

}