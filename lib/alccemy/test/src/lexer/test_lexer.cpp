#include <catch2/catch_test_macros.hpp>

#include <utf8cpp/utf8.h>

import alccemy.lexer;

using namespace alccemy;

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

	enum class TestLexicon {
		Linebreak = 100,
		EndOfFile = 101,
		Indent = 102,
		Dedent = 103,
		A = 0,
		B,
		C,
	};
}

TEST_CASE("Basics")
{
	SECTION("Simple Lexicon")
	{
		auto lexer = create_lexer<TestLexicon>(
			Text("A"),
			Text("B"),
			Text("C")
		);

		SECTION("AllOk")
		{
			REQUIRE_NOTHROW(lexer.lex(to_code_points("ABC")));
		}
	}
	SECTION("Tokenizes")
	{
		auto lexer = create_lexer<TestLexicon>(
			Tokenize(Text("A"), TestLexicon::A),
			Tokenize(Text("B"), TestLexicon::B),
			Tokenize(Text("C"), TestLexicon::C)
		);

		SECTION("Basic")
		{ 
			Tokens tokens = lexer.lex(to_code_points("ABC")).tokens();

 			REQUIRE(tokens.size() == 4);
			//REQUIRE(tokens[0] == Token(TestLexicon::Indent, TextPos(0, 0, 0), 0));
			REQUIRE(tokens[0] == Token(TestLexicon::A, TextPos(0, 0, 0), 1));
			REQUIRE(tokens[1] == Token(TestLexicon::B, TextPos(0, 1, 1), 1));
			REQUIRE(tokens[2] == Token(TestLexicon::C, TextPos(0, 2, 2), 1));
			//REQUIRE(tokens[4] == Token(TestLexicon::Linebreak, TextPos(0, 3, 3), 0));
			REQUIRE(tokens[3] == Token(TestLexicon::EndOfFile, TextPos(0, 3, 3), 0));

		}
		SECTION("Basic with Trailing Linebreak")
		{
			Tokens tokens = lexer.lex(to_code_points("ABC\n")).tokens();

			REQUIRE(tokens.size() == 6);
			REQUIRE(tokens[0] == Token(TestLexicon::Indent, TextPos(0, 0, 0), 0));
			REQUIRE(tokens[1] == Token(TestLexicon::A, TextPos(0, 0, 0), 1));
			REQUIRE(tokens[2] == Token(TestLexicon::B, TextPos(0, 1, 1), 1));
			REQUIRE(tokens[3] == Token(TestLexicon::C, TextPos(0, 2, 2), 1));
			REQUIRE(tokens[4] == Token(TestLexicon::Linebreak, TextPos(0, 3, 3), 1));
			REQUIRE(tokens[5] == Token(TestLexicon::EndOfFile, TextPos(1, 0, 4), 0));

		}
	}

	SECTION("Mixed")
	{
		auto lexer = create_lexer<TestLexicon>(
			Tokenize(Repeats(Text("A")), TestLexicon::A),
			Tokenize(Text("B"), TestLexicon::B),
			Tokenize(Text("C"), TestLexicon::C),
			Text(" ")
		);

		SECTION("Basic")
		{
			Tokens tokens = lexer.lex(to_code_points("AAA C  B")).tokens();

			REQUIRE(tokens.size() == 6);
			REQUIRE(tokens[0] == Token(TestLexicon::Indent, TextPos(0, 0, 0), 0));
			REQUIRE(tokens[1] == Token(TestLexicon::A, TextPos(0, 0, 0), 3));
			REQUIRE(tokens[2] == Token(TestLexicon::C, TextPos(0, 4, 4), 1));
			REQUIRE(tokens[3] == Token(TestLexicon::B, TextPos(0, 7, 7), 1));
		}
		SECTION("End Repeats")
		{
			Tokens tokens = lexer.lex(to_code_points("AA")).tokens();

			REQUIRE(tokens.size() == 4);
			REQUIRE(tokens[0] == Token(TestLexicon::Indent, TextPos(0, 0, 0), 0));
			REQUIRE(tokens[1] == Token(TestLexicon::A, TextPos(0, 0, 0), 2));
		}
	}
	SECTION("Indent")
	{
		auto lexer = create_lexer<TestLexicon>(
			Tokenize(Text("B"), TestLexicon::B)
		);

		SECTION("Basic")
		{
			Tokens tokens = lexer.lex(to_code_points("  B")).tokens();

			REQUIRE(tokens.size() == 4);
			REQUIRE(tokens[0] == Token(TestLexicon::Indent, TextPos(0, 0, 0), 2));
			REQUIRE(tokens[1] == Token(TestLexicon::B, TextPos(0, 2, 2), 1));
		}
		SECTION("Indented Line")
		{
			Tokens tokens = lexer.lex(to_code_points("B\n   B\n   B")).tokens();

			REQUIRE(tokens.size() == 10);
			REQUIRE(tokens[0] == Token(TestLexicon::Indent, TextPos(0, 0, 0), 0));
			REQUIRE(tokens[1] == Token(TestLexicon::B, TextPos(0, 0, 0), 1));
			REQUIRE(tokens[2] == Token(TestLexicon::Linebreak, TextPos(0, 1, 1), 1));
			REQUIRE(tokens[3] == Token(TestLexicon::Indent, TextPos(1, 0, 2), 3));
			REQUIRE(tokens[4] == Token(TestLexicon::B, TextPos(1, 3, 5), 1));
			REQUIRE(tokens[5] == Token(TestLexicon::Linebreak, TextPos(1, 4, 6), 1));
			REQUIRE(tokens[6] == Token(TestLexicon::Indent, TextPos(2, 0, 7), 3));
			REQUIRE(tokens[7] == Token(TestLexicon::B, TextPos(2, 3, 10), 1));
		}
		SECTION("Indented Dedented Line")
		{
			Tokens tokens = lexer.lex(to_code_points("B\n   B\nB")).tokens();

			REQUIRE(tokens.size() == 10);
			REQUIRE(tokens[0] == Token(TestLexicon::Indent, TextPos(0, 0, 0), 0));
			REQUIRE(tokens[1] == Token(TestLexicon::B, TextPos(0, 0, 0), 1));
			REQUIRE(tokens[2] == Token(TestLexicon::Linebreak, TextPos(0, 1, 1), 1));
			REQUIRE(tokens[3] == Token(TestLexicon::Indent, TextPos(1, 0, 2), 3));
			REQUIRE(tokens[4] == Token(TestLexicon::B, TextPos(1, 3, 5), 1));
			REQUIRE(tokens[5] == Token(TestLexicon::Linebreak, TextPos(1, 4, 6), 1));
			REQUIRE(tokens[6] == Token(TestLexicon::Indent, TextPos(2, 0, 7), 0));
			REQUIRE(tokens[7] == Token(TestLexicon::B, TextPos(2, 0, 7), 1));
		}
		SECTION("Nested Indented Line")
		{
			Tokens tokens = lexer.lex(to_code_points("B\n   B\n      B")).tokens();

			REQUIRE(tokens.size() == 10);
			REQUIRE(tokens[0] == Token(TestLexicon::Indent, TextPos(0, 0, 0), 0));
			REQUIRE(tokens[1] == Token(TestLexicon::B, TextPos(0, 0, 0), 1));
			REQUIRE(tokens[2] == Token(TestLexicon::Linebreak, TextPos(0, 1, 1), 1));
			REQUIRE(tokens[3] == Token(TestLexicon::Indent, TextPos(1, 0, 2), 3));
			REQUIRE(tokens[4] == Token(TestLexicon::B, TextPos(1, 3, 5), 1));
			REQUIRE(tokens[5] == Token(TestLexicon::Linebreak, TextPos(1, 4, 6), 1));
			REQUIRE(tokens[6] == Token(TestLexicon::Indent, TextPos(2, 0, 7), 6));
			REQUIRE(tokens[7] == Token(TestLexicon::B, TextPos(2, 6, 13), 1));
		}
		SECTION("Cliff Dedent Line")
		{
			Tokens tokens = lexer.lex(to_code_points("B\n\tB\n\t\tB\nB")).tokens();

			REQUIRE(tokens.size() == 13);
			REQUIRE(tokens[0] == Token(TestLexicon::Indent, TextPos(0, 0, 0), 0));
			REQUIRE(tokens[1] == Token(TestLexicon::B, TextPos(0, 0, 0), 1));
			REQUIRE(tokens[2] == Token(TestLexicon::Linebreak, TextPos(0, 1, 1), 1));
			REQUIRE(tokens[3] == Token(TestLexicon::Indent, TextPos(1, 0, 2), 1));
			REQUIRE(tokens[4] == Token(TestLexicon::B, TextPos(1, 1, 3), 1));
			REQUIRE(tokens[5] == Token(TestLexicon::Linebreak, TextPos(1, 2, 4), 1));
			REQUIRE(tokens[6] == Token(TestLexicon::Indent, TextPos(2, 0, 5), 2));
			REQUIRE(tokens[7] == Token(TestLexicon::B, TextPos(2, 2, 7), 1));
			REQUIRE(tokens[8] == Token(TestLexicon::Linebreak, TextPos(2, 3, 8), 1));
			REQUIRE(tokens[9] == Token(TestLexicon::Indent, TextPos(3, 0, 9), 0));
			REQUIRE(tokens[10] == Token(TestLexicon::B, TextPos(3, 0, 9), 1));
		}
		SECTION("Stepped Dedent Line")
		{
			Tokens tokens = lexer.lex(to_code_points("B\n   B\n      B\n   B\nB")).tokens();

			REQUIRE(tokens.size() == 16);
			REQUIRE(tokens[0] == Token(TestLexicon::Indent, TextPos(0, 0, 0), 0));
			REQUIRE(tokens[1] == Token(TestLexicon::B, TextPos(0, 0, 0), 1));
			REQUIRE(tokens[2] == Token(TestLexicon::Linebreak, TextPos(0, 1, 1), 1));
			REQUIRE(tokens[3] == Token(TestLexicon::Indent, TextPos(1, 0, 2), 3));
			REQUIRE(tokens[4] == Token(TestLexicon::B, TextPos(1, 3, 5), 1));
			REQUIRE(tokens[5] == Token(TestLexicon::Linebreak, TextPos(1, 4, 6), 1));
			REQUIRE(tokens[6] == Token(TestLexicon::Indent, TextPos(2, 0, 7), 6));
			REQUIRE(tokens[7] == Token(TestLexicon::B, TextPos(2, 6, 13), 1));
			REQUIRE(tokens[8] == Token(TestLexicon::Linebreak, TextPos(2, 7, 14), 1));
			REQUIRE(tokens[9] == Token(TestLexicon::Indent, TextPos(3, 0, 15), 3));
			REQUIRE(tokens[10] == Token(TestLexicon::B, TextPos(3, 3, 18), 1));
			REQUIRE(tokens[11] == Token(TestLexicon::Linebreak, TextPos(3, 4, 19), 1));
			REQUIRE(tokens[12] == Token(TestLexicon::Indent, TextPos(4, 0, 20), 0));
			REQUIRE(tokens[13] == Token(TestLexicon::B, TextPos(4, 0, 20), 1));
		}
		SECTION("Cliff Dedent Line Blank Lines")
		{
			Tokens tokens = lexer.lex(to_code_points("B\n    \n\n   B\n      B\n   \nB")).tokens();

			REQUIRE(tokens.size() == 16);
			REQUIRE(tokens[0] == Token(TestLexicon::Indent, TextPos(0, 0, 0), 0));
			REQUIRE(tokens[1] == Token(TestLexicon::B, TextPos(0, 0, 0), 1));
			REQUIRE(tokens[2] == Token(TestLexicon::Linebreak, TextPos(0, 1, 1), 1));
			REQUIRE(tokens[3] == Token(TestLexicon::Linebreak, TextPos(1, 4, 6), 1));
			REQUIRE(tokens[4] == Token(TestLexicon::Linebreak, TextPos(2, 0, 7), 1));
			REQUIRE(tokens[5] == Token(TestLexicon::Indent, TextPos(3, 0, 8), 3));
			REQUIRE(tokens[6] == Token(TestLexicon::B, TextPos(3, 3, 11), 1));
			REQUIRE(tokens[7] == Token(TestLexicon::Linebreak, TextPos(3, 4, 12), 1));
			REQUIRE(tokens[8] == Token(TestLexicon::Indent, TextPos(4, 0, 13), 6));
			REQUIRE(tokens[9] == Token(TestLexicon::B, TextPos(4, 6, 19), 1));
			REQUIRE(tokens[10] == Token(TestLexicon::Linebreak, TextPos(4, 7, 20), 1));
			REQUIRE(tokens[11] == Token(TestLexicon::Linebreak, TextPos(5, 3, 24), 1));
			REQUIRE(tokens[12] == Token(TestLexicon::Indent, TextPos(6, 0, 25), 0));
			REQUIRE(tokens[13] == Token(TestLexicon::B, TextPos(6, 0, 25), 1));
		}
	}
	SECTION("Longest/Shortest")
	{
		auto lexer = create_lexer<TestLexicon>(
			Tokenize(Repeats<Text, 1>(Text("A")), TestLexicon::A),
			Tokenize(Text("AB"), TestLexicon::B),
			Tokenize(Text("B"), TestLexicon::C),
			Text(" ")
		);

		SECTION("Longest")
		{
			Tokens tokens = lexer.lex(to_code_points("AB")).tokens();

			REQUIRE(tokens.size() == 4);
			REQUIRE(tokens[0] == Token(TestLexicon::Indent, TextPos(0, 0, 0), 0));
			REQUIRE(tokens[1] == Token(TestLexicon::B, TextPos(0, 0, 0), 2));

		}
	}

	SECTION("Sad Path")
	{
		auto lexer = create_lexer<TestLexicon>(
			Tokenize(Text("A"), TestLexicon::A),
			Tokenize(Text("B"), TestLexicon::B),
			Text(" ")
		);

		SECTION("Immediate Failure")
		{
			REQUIRE_THROWS_AS(lexer.lex(to_code_points("CCC")), LexerFailure<TestLexicon>);

			try
			{
				lexer.lex(to_code_points("CCC"));
			}
			catch (const LexerFailure<TestLexicon>& e)
			{
				REQUIRE(e.tokens_so_far.size() == 1);
				REQUIRE(e.text_pos == TextPos(0, 0, 0));
				REQUIRE(e.data_index == 0);
			}
		}
		SECTION("Mismatched Indention")
		{
			REQUIRE_THROWS_AS(lexer.lex(to_code_points("A\n   B\n\t\t\tA")), LexerFailure<TestLexicon>);

			try
			{
				lexer.lex(to_code_points("A\n   B\n\t\t\tA"));
			}
			catch (const LexerFailure<TestLexicon>& e)
			{
				REQUIRE(e.tokens_so_far.size() == 6);
				REQUIRE(e.tokens_so_far[0] == Token(TestLexicon::Indent, TextPos(0, 0, 0), 0));
				REQUIRE(e.tokens_so_far[1] == Token(TestLexicon::A, TextPos(0, 0, 0), 1));
				REQUIRE(e.tokens_so_far[2] == Token(TestLexicon::Linebreak, TextPos(0, 1, 1), 1));
				REQUIRE(e.tokens_so_far[3] == Token(TestLexicon::Indent, TextPos(1, 0, 2), 3));
				REQUIRE(e.tokens_so_far[4] == Token(TestLexicon::B, TextPos(1, 3, 5), 1));
				REQUIRE(e.text_pos == TextPos(2, 0, 6));
				REQUIRE(e.data_index == 7);
			}
		}
		SECTION("One Token Failure")
		{
			REQUIRE_THROWS_AS(lexer.lex(to_code_points("A CC")), LexerFailure<TestLexicon>);

			try
			{
				lexer.lex(to_code_points("A CC"));
			}
			catch (const LexerFailure<TestLexicon>& e)
			{
				REQUIRE(e.tokens_so_far.size() == 2);
				REQUIRE(e.tokens_so_far[0] == Token(TestLexicon::Indent, TextPos(0, 0, 0), 0));
				REQUIRE(e.tokens_so_far[1] == Token(TestLexicon::A, TextPos(0, 0, 0), 1));
				REQUIRE(e.text_pos == TextPos(0, 2, 2));
				REQUIRE(e.data_index == 2);
			}
		}
		SECTION("One Token Newline Two Token Failure")
		{
			REQUIRE_THROWS_AS(lexer.lex(to_code_points("A\n   BC")), LexerFailure<TestLexicon>);

			try
			{
				lexer.lex(to_code_points("A\n   BC"));
			}
			catch (const LexerFailure<TestLexicon>& e)
			{
				REQUIRE(e.tokens_so_far.size() == 5);
				REQUIRE(e.tokens_so_far[0] == Token(TestLexicon::Indent, TextPos(0, 0, 0), 0));
				REQUIRE(e.tokens_so_far[1] == Token(TestLexicon::A, TextPos(0, 0, 0), 1));
				REQUIRE(e.tokens_so_far[2] == Token(TestLexicon::Linebreak, TextPos(0, 1, 1), 1));
				REQUIRE(e.tokens_so_far[3] == Token(TestLexicon::Indent, TextPos(1, 0, 2), 3));
				REQUIRE(e.tokens_so_far[4] == Token(TestLexicon::B, TextPos(1, 3, 5), 1));

				REQUIRE(e.text_pos == TextPos(1, 4, 6));
				REQUIRE(e.data_index == 6);
			}
		}
	}
}