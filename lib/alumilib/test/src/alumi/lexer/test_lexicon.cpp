#include "catch2/catch.hpp"

#include "alumi/lexer/alumi_lexicon.h"

#include <utf8cpp/utf8.h>

using namespace alumi;

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

	std::vector<TokenType> get_types(const std::vector<Token>& tokens)
	{
		std::vector<TokenType> types;
		for (const Token& token : tokens)
		{
			types.push_back(token.type());
		}
		return types;
	}
}

TEST_CASE("Lex Snippets")
{
	SECTION("function declaration")
	{
		auto lexed_text = default_lexer.lex(to_code_points("foo := fn(type1 p1, type2 p2) -> type3\n   "));

		REQUIRE_THAT(get_types(lexed_text.tokens()), Catch::Matchers::Equals(std::vector<TokenType>{
				TokenType::Indent,
				TokenType::Symbol,
				TokenType::Assignment,
				TokenType::FuncDeclare,
				TokenType::SubscopeBegin,
				TokenType::Symbol,
				TokenType::Symbol,
				TokenType::Seperator,
				TokenType::Symbol,
				TokenType::Symbol,
				TokenType::SubScopeEnd,
				TokenType::ReturnOp,
				TokenType::Symbol,
				TokenType::Linebreak,
				TokenType::EndOfFile,
		}));
	}
}