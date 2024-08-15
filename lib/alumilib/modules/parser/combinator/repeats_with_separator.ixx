module;

#include "alumi/lexer/token.h"

export module alumi.parser.combinator:repeats_with_seperator;

import :concepts;

import alumi.parser.result;
import alumi.parser.subparser;

using namespace alumi;

export {

   template<ParserElement T, TokenType seperator>
   class RepeatsWithSeperator
   {
   public:
      static Result parse(Subparser& parent)
      {
         size_t repeats = 0;

         Nodes child_nodes;
         while (true)
         {
            Subparser parser = parent.create_child();
            Result res = T::parse(parser);
            if (res.get_type() == Result::Type::Failure)
            {
               return Result(Result::Type::Success, parent, child_nodes);
            }
            else
            {
               child_nodes.insert(child_nodes.end(), res.get_nodes().begin(), res.get_nodes().end());
               parent.take_over_from(res.get_subparser());
               if (parent.peek().type() == seperator)
               {
                  parent.advance();
               }
               else
               {
                  return Result(Result::Type::Success, parent, child_nodes);
               }
            }
         }
      }
   private:
   };

}