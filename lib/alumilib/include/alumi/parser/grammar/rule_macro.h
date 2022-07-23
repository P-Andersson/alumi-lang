#pragma once

#include "alumi/parser/parser_parts.h"
#include "alumi/parser/parse_rule.h"

#define RULE(name, ...) class name; class name  { public: using RuleOp = __VA_ARGS__;  static ParseResult parse(Subparser& parent){ return RuleOp::parse(parent); }; }
