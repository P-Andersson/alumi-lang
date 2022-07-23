module;

#include "alumi/parser/grammar/rule_macro.h"

#include "alumi/lexer/token.h"

export module alumi.parser.grammar;

using namespace alumi;
using namespace alumi::parser;

import :assignment;
import :code_block;
import :expression;
import :function_declaration;
import :function_definition;
import :function_parameter;
import :root;
import :statement;

class CodeBlock;

RULE(NewBlock, Indented);
RULE(EndBlock, AnyOf<Is<TokenType::EndOfFile>, Sequence<Is<TokenType::Linebreak>, Dedented>>);
RULE(EndOfLine, AnyOf<Is<TokenType::Linebreak>, Is<TokenType::EndOfFile>>);

RULE(FunctionParameters, ParseRule<
   RepeatsWithSeperator<Sequence<Is<TokenType::Symbol>,
   Is<TokenType::Symbol>>, TokenType::Seperator>
   , NeverSynchroize, build_function_parameters_node>);

RULE(FunctionParametersScope, ParseRule <
   Sequence<Is<TokenType::SubscopeBegin>,
   FunctionParameters,
   Is<TokenType::SubScopeEnd>>,
   SynchronzieOnMatchedPair<TokenType::SubscopeBegin, TokenType::SubScopeEnd>, build_function_parameters_scope>);

RULE(FunctionDeclaration, ParseRule<
   Sequence<Is<TokenType::FuncDeclare>,
   FunctionParametersScope,
   Optional<Sequence<Is<TokenType::ReturnOp>, Is<TokenType::Symbol>>>,
   Is<TokenType::ScopeBegin>, Is<TokenType::Linebreak>
   >, SynchronizeOnToken<TokenType::Linebreak>, build_function_decleration>);

RULE(FunctionDefinition, ParseRule<
   Sequence<FunctionDeclaration, CodeBlock>, SynchronizeOnToken<TokenType::Indent>, build_func_definition>);

RULE(Expression, ParseRule<
   AnyOf<
   Is<TokenType::Literal>,
   Is<TokenType::Symbol>
   >, NeverSynchroize, build_expression_node>);

RULE(Assignment,
   ParseRule<Sequence<
   Is<TokenType::Symbol>,
   Is<TokenType::Assignment>,
   AnyOf<
   FunctionDefinition,
   Expression
   >,
   EndOfLine
   >, SynchronizeOnToken<TokenType::Linebreak>, build_assignment_node>);

RULE(Statement, ParseRule<
   AnyOf<
   Is<TokenType::Noop>,
   Assignment
   >, NeverSynchroize, build_statement_node>);

RULE(CodeBlock, ParseRule<
   Sequence<
   NewBlock,
   Statement,
   EndOfLine,
   Repeats<Sequence<NoIndent, Statement, EndOfLine>>,
   EndBlock
   >, SynchronzieOnMatchedPair<TokenType::Indent, TokenType::Indent>, build_block_node>);

export RULE(AlumiGrammar, ParseRule<AnyOf<EndOfLine, CodeBlock>, SynchronizeOnToken<TokenType::Linebreak>, build_root_node>);