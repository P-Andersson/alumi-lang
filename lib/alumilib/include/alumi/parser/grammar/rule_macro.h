#pragma once

import alumi.parser.result;
import alumi.parser.combinator;


#define RULE(name, ...) class name; class name  { public: using RuleOp = __VA_ARGS__;  static Result parse(Subparser& parent){ return RuleOp::parse(parent); }; }
