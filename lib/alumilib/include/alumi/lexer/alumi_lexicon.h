#pragma once

#include "alumi/lexer.h"

namespace alumi
{
#define ALUMI_NON_SYMBOL_CHARS "!$%&'()*+,-./:;<=>?@[\\]^_{|}~"
#define ALUMI_OPERATOR_CHARS "!&|+-*/<>_~"
#define ALUMI_ASSIGNMENT ":="
#define ALUMI_NUMERICS "0123456789"
#define ALUMI_WHITESPACE " \t"

   static auto default_lexer = Lexer(
      AnyOf(ALUMI_WHITESPACE),
      Tokenize(Pattern(NotAnyOf(ALUMI_NUMERICS ALUMI_NON_SYMBOL_CHARS ALUMI_WHITESPACE), Repeats(NotAnyOf(ALUMI_NON_SYMBOL_CHARS ALUMI_WHITESPACE))), TokenType::Symbol),
      Tokenize(Repeats<AnyOf, 1>(AnyOf(ALUMI_OPERATOR_CHARS)), TokenType::Operator),
      Tokenize(Repeats<AnyOf, 1>(AnyOf(ALUMI_NUMERICS)), TokenType::Literal),
      Tokenize(Text(":="), TokenType::Assignment),
      Tokenize(Text(","), TokenType::Seperator),
      Tokenize(Text("("), TokenType::SubscopeBegin),
      Tokenize(Text(")"), TokenType::SubScopeEnd),
      Tokenize(Text(":"), TokenType::ScopeBegin),
      Tokenize(Text("fn"), TokenType::FuncDeclare),
      Tokenize(Text("if"), TokenType::If),
      Tokenize(Text("else"), TokenType::Else),
      Tokenize(Text("for"), TokenType::For),
      Tokenize(Text("while"), TokenType::While)
      );
}