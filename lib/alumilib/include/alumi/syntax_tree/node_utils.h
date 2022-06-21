#pragma once

#include "alumi/parser/parser_parts.h"

namespace alumi
{
	namespace syntax_tree
	{
		//!
		//! Finds the failure token index from a failure result,
		//! the returned index is relative to parser start
		//! Note that any child nodes contained in the result will have thier tokens skipped so a 
		//! token grammar like this:
		//! 		 A, B, Child, C
		//! that fails on C will ALWAYS have a an index of 2 (skipping the child)
		//! This lets you identify which part of a current rule failed to parse
		//! 
		//! @precondition result mest of type Failure, Success or even RecoveredFailure is not vaild
		//! 
		size_t get_failure_token_index(const ::alumi::parser::ParseResult& result);
	}
}