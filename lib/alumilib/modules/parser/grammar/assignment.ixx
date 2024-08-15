module;

#include "alumi/parser/grammar/rule_macro.h"
#include "alumi/lexer/token.h"

#include <cassert>
#include <optional>

export module alumi.parser.grammar:assignment;

import alumi.parser.error_codes;
import alumi.syntax_tree.nodes;
import alumi.syntax_tree.node_builder;


using namespace alumi;

std::optional<Node> build_assignment_node(const Result& res)
{
   if (res.get_type() == Result::Type::Failure)
   {
      Error error(ErrorCode::Unknown, *res.get_panic_token_index(), res.get_nodes());
      return make_node(Node::Type(error), res);
   }
   return make_node(Assignment(res.get_nodes()[0]), res);
};
