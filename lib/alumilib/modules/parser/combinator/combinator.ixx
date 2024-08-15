
//!
//! This is the root module for the alumi parser combinators.
//! 
//! These are a set of templates that are combined at compile time to form the Alumi-parser
//! 
//! Ie this is a compile-time parser combinator
//! 
export module alumi.parser.combinator;

export import :any_of;
export import :is;
export import :optional;
export import :peek;
export import :repeats;
export import :repeats_with_seperator;
export import :rule;
export import :sequence;
export import :swallow;

export import :indented;
export import :dedented;
export import :no_indent;


