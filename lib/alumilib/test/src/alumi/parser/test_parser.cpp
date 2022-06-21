#include "catch2/catch.hpp"

#include "alumi/parser.h"
#include "alumi/lexer/alumi_lexicon.h"

#include "alumi/syntax_tree/util_ops.h"

#include <fstream>

#include <utf8cpp/utf8.h>

using namespace alumi;
using namespace alumi::parser;
using namespace alumi::syntax_tree;


namespace {
	UnicodeCodePoint utf32(char c)
	{
		UnicodeCodePoint out;
		utf8::utf8to32(&c, &c + 1, &out);
		return out;
	}

	std::vector<UnicodeCodePoint> to_code_points(const std::string_view& view)
	{
		std::vector<UnicodeCodePoint> code_points;
		auto ite = view.begin();
		while (ite != view.end())
		{
			code_points.push_back(utf8::next(ite, view.end()));
		}
		return code_points;
	}

	void print_output(const std::string& filename, ModuleTree& tree, const std::vector<UnicodeCodePoint>& text, const std::vector<Token>& tokens)
	{
		TreeOpRepresentTree representer(&text, &tokens);
		ModuleTreeWalker walker(tree);

		walker.walk_from_root(representer);

		std::fstream f;
		f.open(filename, std::ios::out);
		f << representer.representation;
	}
}

TEST_CASE("Test Main Func")
{
	SECTION("Correct")
	{
		auto code_points = to_code_points(""
		"main := fn(env Environment) -> ResultCode:\n"
		"   noop"
		"");

		auto tokens = default_lexer.lex(code_points);

		AlumiParser parser;
		ParseResult res = parser.parse(tokens);

		ModuleTree tree(res.get_nodes());
		print_output("test_main_func_ok.txt", tree, code_points, tokens);

		REQUIRE(res.get_type() == ParseResult::Type::Success);
		REQUIRE(res.get_consumed() == 15);


	}
	SECTION("Recoverable Error")
	{
		auto code_points = to_code_points(""
			"main := fn(env Environment) --> ResultCode:\n"
			"   noop"
			"");

		auto tokens = default_lexer.lex(code_points);

		AlumiParser parser;
		ParseResult res = parser.parse(tokens);

		ModuleTree tree(res.get_nodes());
		print_output("test_main_func_not_ok.txt", tree, code_points, tokens);

		REQUIRE(res.get_type() == ParseResult::Type::RecoveredFailure);
		REQUIRE(res.get_consumed() == 15);

	}
}
TEST_CASE("Test integer Func")
{
	SECTION("Correct")
	{
		auto code_points = to_code_points(""
			"foo := fn(inp int32) -> int32:\n"
			"   noop"
			"");

		auto tokens = default_lexer.lex(code_points);

		AlumiParser parser;
		ParseResult res = parser.parse(tokens);

		ModuleTree tree(res.get_nodes());
		print_output("test_integer_func_ok.txt", tree, code_points, tokens);

		REQUIRE(res.get_type() == ParseResult::Type::Success);
		REQUIRE(res.get_consumed() == 15);
	}
	SECTION("Recoverable Error")
	{
		auto code_points = to_code_points(""
			"foo := fn(inp int32) && int32:\n"
			"   noop"
			"");

		auto tokens = default_lexer.lex(code_points);

		AlumiParser parser;
		ParseResult res = parser.parse(tokens);

		ModuleTree tree(res.get_nodes());
		print_output("test_integer_func_not_ok.txt", tree, code_points, tokens);

		REQUIRE(res.get_type() == ParseResult::Type::RecoveredFailure);
		REQUIRE(res.get_consumed() == 15);
	}
}

TEST_CASE("Test primitive decleration")
{
	SECTION("Correct - int")
	{
		auto code_points = to_code_points("foo := 5");

		auto tokens = default_lexer.lex(code_points);

		AlumiParser parser;
		ParseResult res = parser.parse(tokens);

		ModuleTree tree(res.get_nodes());
		print_output("test_prim_dec_integer_ok.txt", tree, code_points, tokens);

		REQUIRE(res.get_type() == ParseResult::Type::Success);
		REQUIRE(res.get_consumed() == 5);
	}
	SECTION("Recovered Error - int")
	{
		auto code_points = to_code_points("foo := 5<<<");

		auto tokens = default_lexer.lex(code_points);

		AlumiParser parser;
		ParseResult res = parser.parse(tokens);

		ModuleTree tree(res.get_nodes());
		print_output("test_prim_dec_integer_not_ok.txt", tree, code_points, tokens);

		REQUIRE(res.get_type() == ParseResult::Type::RecoveredFailure);
		REQUIRE(res.get_consumed() == 5);
	}
}
