module;

#include "alumi/parser/grammar/rule_macro.h"
#include "alumi/lexer/token.h"

#include <cassert>
#include <optional>

export module alumi.parser.grammar:value;

import alumi.parser.error_codes;
import alumi.syntax_tree.nodes;
import alumi.syntax_tree.node_builder;

using namespace alumi;

std::optional<Node> build_literal_dereference_node(const Result& res)
{
   if (res.get_type() == Result::Type::Failure)
   {
      return make_node(Error(ErrorCode::Unknown, *res.get_panic_token_index(), res.get_nodes()), res);
   }
   return make_node(Statement(res.get_nodes()), res);
};

std::optional<Node> build_value_deference_node(const Result& res)
{
   if (res.get_type() == Result::Type::Failure)
   {
      return make_node(Error(ErrorCode::Unknown, *res.get_panic_token_index(), res.get_nodes()), res);
   }
   return make_node(Statement(res.get_nodes()), res);
};


std::optional<Node> build_value_node(const Result& res)
{
   if (res.get_type() == Result::Type::Failure)
   {
      return make_node(Error(ErrorCode::Unknown, *res.get_panic_token_index(), res.get_nodes()), res);
   }
   return make_node(Statement(res.get_nodes()), res);
};
