module;

#include "alumi/lexer/token.h"

#include <optional>

export module alumi.parser.combinator:indented;

import :concepts;

import alumi.parser.result;
import alumi.parser.subparser;

using namespace alumi;

export {

   class Indented
   {
   public:
      static Result parse(Subparser& parent)
      {
         auto indent = parent.get_indent();
         auto token = parent.advance();

         if (token.type() == TokenType::Indent)
         {
            if (indent == std::nullopt || parent.get_indent() > indent)
            {
               return Result(Result::Type::Success, parent, {});
            }
         }
         parent.do_panic(parent.current_token_index() - 1);
         return Result(Result::Type::Failure, parent, {});
      }
   private:
   };

}
