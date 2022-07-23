#include "alumi/parser.h"

#include "alumi/parser/parser_parts.h"
#include "alumi/parser/parse_rule.h"

import alumi.parser.grammar;

using namespace alumi::syntax_tree;


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
         return SyntaxTree(AlumiGrammar::parse(root_parser), &text);
      }
   }
}