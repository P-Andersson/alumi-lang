#include "alumi/parser.h"


import alumi.parser.grammar;


namespace alumi
{
   namespace parser
   {
      AlumiParser::AlumiParser()
      {

      }
      AlumiParser::~AlumiParser()
      {

      }

      SyntaxTree AlumiParser::parse(const LexedText& text) const
      {
         Subparser root_parser(text.tokens());
         return SyntaxTree(grammar::AlumiGrammar::parse(root_parser), &text);
      }
   }
}