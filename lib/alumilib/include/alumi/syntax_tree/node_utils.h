#pragma once

#include "alumi/parser/data.h"
#include "alumi/syntax_tree.h"

import alumi.parser;

namespace alumi
{
	namespace syntax_tree
	{
		//!
		//! Finds the failure token index from a failure result,
		//! the returned index is relative to parser start
		//! Note that any child nodes contained in the result will have their tokens skipped so a 
		//! token grammar like this:
		//! 		 A, B, Child, C
		//! that fails on C will ALWAYS have a an index of 2 (skipping the child)
		//! This lets you identify which part of a current rule failed to parse
		//! 
		//! @precondition result mest of type Failure, Success or even RecoveredFailure is not vaild
		//! 
		size_t get_failure_token_index(const ::alumi::parser::Result& result);

	  	//!
		//! Converts a single node to the source text, replaces "in-the-middle" child nodes with " ... ". 
		//! May be empty if it did not consume any tokens by itself
		//! 
		std::vector<UnicodeCodePoint> as_string(const Node& node, const SyntaxTree& tree);
		//! Converts a single node to the source text it consumed
		std::vector<UnicodeCodePoint> as_string_deep(const Node& node, const SyntaxTree& tree);
	}
}