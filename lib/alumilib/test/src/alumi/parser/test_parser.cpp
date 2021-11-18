#include "catch2/catch.hpp"

#include "alumi/parser.h"
#include "alumi/lexer/alumi_lexicon.h"

#include <utf8cpp/utf8.h>

using namespace alumi;
using namespace alumi::parser;


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

		REQUIRE(res.get_type() == ParseResult::Type::Success);
		REQUIRE(res.get_consumed() == 14);
	}
	SECTION("Some Error")
	{
		auto code_points = to_code_points(""
			"main := fn(env Environment) --> ResultCode:\n"
			"   noop"
			"");

		auto tokens = default_lexer.lex(code_points);

		AlumiParser parser;
		ParseResult res = parser.parse(tokens);

		REQUIRE(res.get_type() == ParseResult::Type::Failure);
		REQUIRE(res.get_consumed() == 14);
	}
}