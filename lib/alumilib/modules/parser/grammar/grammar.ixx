module;

#include "alumi/parser/grammar/rule_macro.h"

#include "alumi/lexer/token.h"

export module alumi.parser.grammar;

import alumi.parser.combinator;

using namespace alumi;

import :assignment;
import :code_block;
import :expression;
import :function_call;
import :function_declaration;
import :function_definition;
import :function_parameter;
import :root;
import :statement;
import :tuple;
import :value;

namespace grammar 
{
class CodeBlock;

RULE(NewBlock, Indented);
RULE(EndBlock, AnyOf<Is<TokenType::EndOfFile>, Sequence<Is<TokenType::Linebreak>, Dedented>>);
RULE(EndOfLine, AnyOf<Is<TokenType::Linebreak>, Is<TokenType::EndOfFile>>);

RULE(LiteralDereference, ParseRule<Is<TokenType::Literal>, NeverSynchroize, build_literal_dereference_node>);

RULE(SymbolDereference, ParseRule<Is<TokenType::Symbol>, NeverSynchroize, build_value_deference_node>);

RULE(Value, ParseRule<AnyOf<
   Is<TokenType::Literal>,
   Is<TokenType::Symbol>
   >, NeverSynchroize, build_value_node>);

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

RULE(TupleValues, ParseRule <
   RepeatsWithSeperator<Value, TokenType::Seperator>,
   NeverSynchroize, build_tuple_values_node>);

RULE(FunctionArgumentsScope, ParseRule <
   Sequence<Is<TokenType::SubscopeBegin>,
   TupleValues,
   Is<TokenType::SubScopeEnd>>,
   SynchronzieOnMatchedPair<TokenType::SubscopeBegin, TokenType::SubScopeEnd>, build_function_call_paremeters_scope_node>);

RULE(FunctionCall, ParseRule<
   Sequence<Value, FunctionArgumentsScope>, NeverSynchroize, build_function_call_node>);

RULE(Expression, ParseRule<AnyOf<
   Value,
   FunctionCall>
   , NeverSynchroize, build_expression_node>);

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
}