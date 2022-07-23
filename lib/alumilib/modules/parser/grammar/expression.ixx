module;

#include "alumi/parser/grammar/rule_macro.h"
#include "alumi/lexer/token.h"

#include <cassert>

export module alumi.parser.grammar:expression;

using namespace alumi;
using namespace alumi::parser;
using namespace alumi::syntax_tree;

std::optional<Node> build_expression_node(const ParseResult& res)
{
   if (res.get_type() == ParseResult::Type::Failure)
   {
      return Node(Error(ErrorCode::Unknown, *res.get_panic_token_index(), res.get_nodes()), res);
   }
   return Node(syntax_tree::Expression(), res);
};
