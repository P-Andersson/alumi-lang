module;

#include "alumi/lexer/token.h"

export module alumi.parser.combinator:swallow;

import :concepts;

import alumi.parser.result;
import alumi.parser.subparser;

using namespace alumi;

export {

   template<ParserElement T, TokenType swallowed>
   class Swallow
   {
   public:
      static Result parse(Subparser& parent)
      {
         Subparser parser = parent.create_child();
         parser.add_swallowed_token(swallowed);
         return T::parse(parser);

      }
   private:
   };

}