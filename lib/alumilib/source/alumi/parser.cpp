#include "alumi/parser.h"

#include "alumi/parser/parser_parts.h"

namespace alumi
{
   namespace parser
   {
      /*class ParserState
      {
      public:
         AluElement line_terminator = RulesSet({
                  TokenType::Indent
         });

         ParseRule<void> value = ParseRule<void>(
            Sequence({
               Optional(TokenType::Operator),
               RulesSet({
                  TokenType::Literal,
                  TokenType::Symbol
                   })
               }));

         ParseRule<void> expression = ParseRule<void>(
            RulesSet({
               Sequence({&value, TokenType::Operator, &value})
               }));

         ParseRule<void> assignment = ParseRule<void>(
            Sequence({
               TokenType::Symbol,
               TokenType::Assignment,
               & expression
            })
         );


         ParseRule<void> line = ParseRule<void>(
            Sequence({
               RulesSet({
                  expression
               }),
               line_terminator
            })
         );

         ParseRule<void> code_body = ParseRule<void>(
            Repeats(line)
         );
      };


      AlumiParser::AlumiParser()
      {
      }
      AlumiParser::~AlumiParser()
      {

      }

      void AlumiParser::parse() const
      {



      }*/

   }
}