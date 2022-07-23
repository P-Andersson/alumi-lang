module;

#include "alumi/parser/grammar/rule_macro.h"
#include "alumi/lexer/token.h"

#include <cassert>

export module alumi.parser.grammar:function_definition;

using namespace alumi;
using namespace alumi::parser;
using alumi::syntax_tree::Node;
using alumi::syntax_tree::Error;

std::optional<Node> build_func_definition(const ParseResult& res)
{
   if (res.get_type() == ParseResult::Type::Failure)
   {
      return Node(Error(ErrorCode::Unknown, *res.get_panic_token_index(), res.get_nodes()), res);
   }
   const auto& dec = res.get_nodes()[0];
   return Node(syntax_tree::FunctionDefinition(dec, res.get_nodes()[dec.recursive_child_count()]), res);

};
