#include <catch2/catch_test_macros.hpp>

import alccemy.lexer;

using namespace alccemy;

namespace {
	enum class TestLexicon {
		Linebreak = 100,
		EndOfFile = 101,
		Indent = 102,
		Dedent = 103,
		A = 0,
		B,
		C,
		Ace,
	};
}

TEST_CASE("Basics")
{
	SECTION("Simple Lexicon")
	{
		auto lexer = create_lexer<TestLexicon>(
			PatternSet{
				Text("A"),
				Text("B"),
				Text("C")
			}
		);

		SECTION("AllOk")
		{
			REQUIRE_NOTHROW(lexer.lexUtf8("ABC"));
		}
	}
	SECTION("Tokenizes")
	{
		auto lexer = create_lexer<TestLexicon>(
			PatternSet{
				Tokenize(Text("A"), TestLexicon::A),
				Tokenize(Text("B"), TestLexicon::B),
				Tokenize(Text("C"), TestLexicon::C),
				Tokenize(Text(as_utf8(0x0001F0A1)), TestLexicon::Ace),
				Tokenize(Text("\n"), TestLexicon::Linebreak)
			}
		);

		SECTION("Basic")
		{ 
			auto tokens = lexer.lexUtf8("ABC").value().tokens();

 			REQUIRE(tokens.size() == 4);
			REQUIRE(tokens[0] == Token(TestLexicon::A, TextPos(0, 0, 0), 1));
			REQUIRE(tokens[1] == Token(TestLexicon::B, TextPos(0, 1, 1), 1));
			REQUIRE(tokens[2] == Token(TestLexicon::C, TextPos(0, 2, 2), 1));
			REQUIRE(tokens[3] == Token(TestLexicon::EndOfFile, TextPos(0, 3, 3), 0));

		}

		SECTION("With Multibyte Chars")
		{
			auto tokens = lexer.lexUtf8("A" + as_utf8(0x0001F0A1) + "C").value().tokens();

			REQUIRE(tokens.size() == 4);
			REQUIRE(tokens[0] == Token(TestLexicon::A, TextPos(0, 0, 0), 1));
			REQUIRE(tokens[1] == Token(TestLexicon::Ace, TextPos(0, 1, 1), 4));
			REQUIRE(tokens[2] == Token(TestLexicon::C, TextPos(0, 2, 5), 1));
			REQUIRE(tokens[3] == Token(TestLexicon::EndOfFile, TextPos(0, 3, 6), 0));

		}

		SECTION("Basic with Trailing Linebreak")
		{
			auto tokens = lexer.lexUtf8("ABC\n").value().tokens();

			REQUIRE(tokens.size() == 5);
			REQUIRE(tokens[0] == Token(TestLexicon::A, TextPos(0, 0, 0), 1));
			REQUIRE(tokens[1] == Token(TestLexicon::B, TextPos(0, 1, 1), 1));
			REQUIRE(tokens[2] == Token(TestLexicon::C, TextPos(0, 2, 2), 1));
			REQUIRE(tokens[3] == Token(TestLexicon::Linebreak, TextPos(0, 3, 3), 1));
			REQUIRE(tokens[4] == Token(TestLexicon::EndOfFile, TextPos(1, 0, 4), 0));

		}
	}

	SECTION("Mixed")
	{
		auto lexer = create_lexer<TestLexicon>(
			PatternSet{
				Tokenize(Repeats(Text("A")), TestLexicon::A),
				Tokenize(Text("B"), TestLexicon::B),
				Tokenize(Text("C"), TestLexicon::C),
				Text(" ")

			}
		);

		SECTION("Basic")
		{
			auto tokens = lexer.lexUtf8("AAA C  B").value().tokens();

			REQUIRE(tokens.size() == 4);
			REQUIRE(tokens[0] == Token(TestLexicon::A, TextPos(0, 0, 0), 3));
			REQUIRE(tokens[1] == Token(TestLexicon::C, TextPos(0, 4, 4), 1));
			REQUIRE(tokens[2] == Token(TestLexicon::B, TextPos(0, 7, 7), 1));
			REQUIRE(tokens[3] == Token(TestLexicon::EndOfFile, TextPos(0, 8, 8), 0));
		}
		SECTION("End Repeats")
		{
 			auto tokens = lexer.lexUtf8("AA").value().tokens();

 			REQUIRE(tokens.size() == 2);
			REQUIRE(tokens[0] == Token(TestLexicon::A, TextPos(0, 0, 0), 2));
		}
	}
	SECTION("Indent")
	{
		auto lexer = create_lexer<TestLexicon>(
			RuleSet{
				IndentionRule<TestLexicon, TestLexicon::Indent, TestLexicon::Dedent>()
			},
			PatternSet{
    			Tokenize(Text("A"), TestLexicon::A),
				Tokenize(Text("B"), TestLexicon::B),
				Tokenize(Text("\n"), TestLexicon::Linebreak),
				Text(" "),
			}
		);

		SECTION("Basic")
		{
			auto tokens = lexer.lexUtf8("  B").value().tokens();

			REQUIRE(tokens.size() == 4);
			REQUIRE(tokens[0] == Token(TestLexicon::Indent, TextPos(0, 0, 0), 2));
			REQUIRE(tokens[1] == Token(TestLexicon::B, TextPos(0, 2, 2), 1));
			REQUIRE(tokens[2] == Token(TestLexicon::Dedent, TextPos(0, 3, 3), 0));
		}
		SECTION("Indented Line")
		{
			auto tokens = lexer.lexUtf8("B\n   B\n   B").value().tokens();

			REQUIRE(tokens.size() == 8);
			REQUIRE(tokens[0] == Token(TestLexicon::B, TextPos(0, 0, 0), 1));
			REQUIRE(tokens[1] == Token(TestLexicon::Linebreak, TextPos(0, 1, 1), 1));
			REQUIRE(tokens[2] == Token(TestLexicon::Indent, TextPos(1, 0, 2), 3));
			REQUIRE(tokens[3] == Token(TestLexicon::B, TextPos(1, 3, 5), 1));
			REQUIRE(tokens[4] == Token(TestLexicon::Linebreak, TextPos(1, 4, 6), 1));
			REQUIRE(tokens[5] == Token(TestLexicon::B, TextPos(2, 3, 10), 1));
			REQUIRE(tokens[6] == Token(TestLexicon::Dedent, TextPos(2, 4, 11), 0));
			REQUIRE(tokens[7] == Token(TestLexicon::EndOfFile, TextPos(2, 4, 11), 0));
		}
		SECTION("Indented Dedented Line")
		{
			auto tokens = lexer.lexUtf8("B\n   B\nB").value().tokens();

			REQUIRE(tokens.size() == 8);
			REQUIRE(tokens[0] == Token(TestLexicon::B, TextPos(0, 0, 0), 1));
			REQUIRE(tokens[1] == Token(TestLexicon::Linebreak, TextPos(0, 1, 1), 1));
			REQUIRE(tokens[2] == Token(TestLexicon::Indent, TextPos(1, 0, 2), 3));
			REQUIRE(tokens[3] == Token(TestLexicon::B, TextPos(1, 3, 5), 1));
			REQUIRE(tokens[4] == Token(TestLexicon::Linebreak, TextPos(1, 4, 6), 1));
			REQUIRE(tokens[5] == Token(TestLexicon::Dedent, TextPos(2, 0, 7), 0));
			REQUIRE(tokens[6] == Token(TestLexicon::B, TextPos(2, 0, 7), 1));
			REQUIRE(tokens[7] == Token(TestLexicon::EndOfFile, TextPos(2, 1, 8), 0));
		}
		SECTION("Only Newlines Should Produce Indents")
		{
			auto tokens = lexer.lexUtf8("B  A").value().tokens();

			REQUIRE(tokens.size() == 3);
			REQUIRE(tokens[0] == Token(TestLexicon::B, TextPos(0, 0, 0), 1));
			REQUIRE(tokens[1] == Token(TestLexicon::A, TextPos(0, 3, 3), 1));
			REQUIRE(tokens[2] == Token(TestLexicon::EndOfFile, TextPos(0, 4, 4), 0));
		}
		SECTION("Nested Indented Line")
		{
			auto tokens = lexer.lexUtf8("B\n   B\n      B").value().tokens();

			REQUIRE(tokens.size() == 10);
			REQUIRE(tokens[0] == Token(TestLexicon::B, TextPos(0, 0, 0), 1));
			REQUIRE(tokens[1] == Token(TestLexicon::Linebreak, TextPos(0, 1, 1), 1));
			REQUIRE(tokens[2] == Token(TestLexicon::Indent, TextPos(1, 0, 2), 3));
			REQUIRE(tokens[3] == Token(TestLexicon::B, TextPos(1, 3, 5), 1));
			REQUIRE(tokens[4] == Token(TestLexicon::Linebreak, TextPos(1, 4, 6), 1));
			REQUIRE(tokens[5] == Token(TestLexicon::Indent, TextPos(2, 0, 7), 6));
			REQUIRE(tokens[6] == Token(TestLexicon::B, TextPos(2, 6, 13), 1));
			REQUIRE(tokens[7] == Token(TestLexicon::Dedent, TextPos(2, 7, 14), 0));
			REQUIRE(tokens[8] == Token(TestLexicon::Dedent, TextPos(2, 7, 14), 0));
			REQUIRE(tokens[9] == Token(TestLexicon::EndOfFile, TextPos(2, 7, 14), 0));
		}
		SECTION("Cliff Dedent Line")
		{
			auto tokens = lexer.lexUtf8("B\n\tB\n\t\tB\nB").value().tokens();

			REQUIRE(tokens.size() == 12);
			REQUIRE(tokens[0] == Token(TestLexicon::B, TextPos(0, 0, 0), 1));
			REQUIRE(tokens[1] == Token(TestLexicon::Linebreak, TextPos(0, 1, 1), 1));
			REQUIRE(tokens[2] == Token(TestLexicon::Indent, TextPos(1, 0, 2), 1));
			REQUIRE(tokens[3] == Token(TestLexicon::B, TextPos(1, 1, 3), 1));
			REQUIRE(tokens[4] == Token(TestLexicon::Linebreak, TextPos(1, 2, 4), 1));
			REQUIRE(tokens[5] == Token(TestLexicon::Indent, TextPos(2, 0, 5), 2));
			REQUIRE(tokens[6] == Token(TestLexicon::B, TextPos(2, 2, 7), 1));
			REQUIRE(tokens[7] == Token(TestLexicon::Linebreak, TextPos(2, 3, 8), 1));
			REQUIRE(tokens[8] == Token(TestLexicon::Dedent, TextPos(3, 0, 9), 0));
			REQUIRE(tokens[9] == Token(TestLexicon::Dedent, TextPos(3, 0, 9), 0));
			REQUIRE(tokens[10] == Token(TestLexicon::B, TextPos(3, 0, 9), 1));
			REQUIRE(tokens[11] == Token(TestLexicon::EndOfFile, TextPos(3, 1, 10), 0));
		}
		SECTION("Stepped Dedent Line")
		{
			auto tokens = lexer.lexUtf8("B\n   B\n      B\n   B\nB").value().tokens();

			REQUIRE(tokens.size() == 14);
			REQUIRE(tokens[0] == Token(TestLexicon::B, TextPos(0, 0, 0), 1));
			REQUIRE(tokens[1] == Token(TestLexicon::Linebreak, TextPos(0, 1, 1), 1));
			REQUIRE(tokens[2] == Token(TestLexicon::Indent, TextPos(1, 0, 2), 3));
			REQUIRE(tokens[3] == Token(TestLexicon::B, TextPos(1, 3, 5), 1));
			REQUIRE(tokens[4] == Token(TestLexicon::Linebreak, TextPos(1, 4, 6), 1));
			REQUIRE(tokens[5] == Token(TestLexicon::Indent, TextPos(2, 0, 7), 6));
			REQUIRE(tokens[6] == Token(TestLexicon::B, TextPos(2, 6, 13), 1));
			REQUIRE(tokens[7] == Token(TestLexicon::Linebreak, TextPos(2, 7, 14), 1));
			REQUIRE(tokens[8] == Token(TestLexicon::Dedent, TextPos(3, 0, 15), 3));
			REQUIRE(tokens[9] == Token(TestLexicon::B, TextPos(3, 3, 18), 1));
			REQUIRE(tokens[10] == Token(TestLexicon::Linebreak, TextPos(3, 4, 19), 1));
			REQUIRE(tokens[11] == Token(TestLexicon::Dedent, TextPos(4, 0, 20), 0));
			REQUIRE(tokens[12] == Token(TestLexicon::B, TextPos(4, 0, 20), 1));
			REQUIRE(tokens[13] == Token(TestLexicon::EndOfFile, TextPos(4, 1, 21), 0));
		}
		SECTION("Cliff Dedent Line Blank Lines")
		{
			auto tokens = lexer.lexUtf8("B\n    \n\n   B\n      B\n   \nB").value().tokens();

			REQUIRE(tokens.size() == 15);
			REQUIRE(tokens[0] == Token(TestLexicon::B, TextPos(0, 0, 0), 1));
			REQUIRE(tokens[1] == Token(TestLexicon::Linebreak, TextPos(0, 1, 1), 1));
			REQUIRE(tokens[2] == Token(TestLexicon::Linebreak, TextPos(1, 4, 6), 1));
			REQUIRE(tokens[3] == Token(TestLexicon::Linebreak, TextPos(2, 0, 7), 1));
			REQUIRE(tokens[4] == Token(TestLexicon::Indent, TextPos(3, 0, 8), 3));
			REQUIRE(tokens[5] == Token(TestLexicon::B, TextPos(3, 3, 11), 1));
			REQUIRE(tokens[6] == Token(TestLexicon::Linebreak, TextPos(3, 4, 12), 1));
			REQUIRE(tokens[7] == Token(TestLexicon::Indent, TextPos(4, 0, 13), 6));
			REQUIRE(tokens[8] == Token(TestLexicon::B, TextPos(4, 6, 19), 1));
			REQUIRE(tokens[9] == Token(TestLexicon::Linebreak, TextPos(4, 7, 20), 1));
			REQUIRE(tokens[10] == Token(TestLexicon::Linebreak, TextPos(5, 3, 24), 1));
			REQUIRE(tokens[11] == Token(TestLexicon::Dedent, TextPos(6, 0, 25), 0));
			REQUIRE(tokens[12] == Token(TestLexicon::Dedent, TextPos(6, 0, 25), 0));
			REQUIRE(tokens[13] == Token(TestLexicon::B, TextPos(6, 0, 25), 1));
			REQUIRE(tokens[14] == Token(TestLexicon::EndOfFile, TextPos(6, 1, 26), 0));
		}
	}
	SECTION("Longest/Shortest")
	{
		auto lexer = create_lexer<TestLexicon>(
			PatternSet{
				Tokenize(Repeats<Text, 1>(Text("A")), TestLexicon::A),
				Tokenize(Text("AB"), TestLexicon::B),
				Tokenize(Text("B"), TestLexicon::C),
				Text(" ")
			}
		);

		SECTION("Longest")
		{
			auto tokens = lexer.lexUtf8("AB").value().tokens();

			REQUIRE(tokens.size() == 2);
			REQUIRE(tokens[0] == Token(TestLexicon::B, TextPos(0, 0, 0), 2));

		}
	}

	SECTION("Sad Path")
	{
		auto lexer = create_lexer<TestLexicon>(
			RuleSet{
				IndentionRule<TestLexicon, TestLexicon::Indent, TestLexicon::Dedent>()
			},
			PatternSet{
				Tokenize(Text("A"), TestLexicon::A),
				Tokenize(Text("B"), TestLexicon::B),
				Tokenize(Text("\n"), TestLexicon::Linebreak),
				Text(" ")
			}
		);

		SECTION("Immediate Failure")
		{
			auto res = lexer.lexUtf8("CCC");

			REQUIRE(!res.has_value());

			auto e = res.error();

			REQUIRE(e.tokens_so_far.size() == 0);
			REQUIRE(e.text_pos == TextPos(0, 0, 0));
			REQUIRE(e.data_index == 0);
		}
		SECTION("Mismatched Indention")
		{
			auto res = lexer.lexUtf8("A\n    B\n  A");
			REQUIRE(!res.has_value());

			auto e = res.error();
 			REQUIRE(e.tokens_so_far.size() == 6);
			REQUIRE(e.tokens_so_far[0] == Token(TestLexicon::A, TextPos(0, 0, 0), 1));
			REQUIRE(e.tokens_so_far[1] == Token(TestLexicon::Linebreak, TextPos(0, 1, 1), 1));
			REQUIRE(e.tokens_so_far[2] == Token(TestLexicon::Indent, TextPos(1, 0, 2), 4));
			REQUIRE(e.tokens_so_far[3] == Token(TestLexicon::B, TextPos(1, 4, 6), 1));
			REQUIRE(e.tokens_so_far[4] == Token(TestLexicon::Linebreak, TextPos(1, 5, 7), 1));
			REQUIRE(e.tokens_so_far[5] == Token(TestLexicon::Dedent, TextPos(2, 0, 8), 2));
			REQUIRE(e.text_pos == TextPos(2, 2, 10));
			REQUIRE(e.data_index == 1);

		}
		SECTION("One Token Failure")
		{
			auto res = lexer.lexUtf8("A CC");
			REQUIRE(!res.has_value());

			auto e = res.error();
			REQUIRE(e.tokens_so_far.size() == 1);
			REQUIRE(e.tokens_so_far[0] == Token(TestLexicon::A, TextPos(0, 0, 0), 1));
			REQUIRE(e.text_pos == TextPos(0, 2, 2));
			REQUIRE(e.data_index == 2);
			
		}
		SECTION("One Token Newline Two Token Failure")
		{
			auto res = lexer.lexUtf8("A\n   BC");
			REQUIRE(!res.has_value());

			auto e = res.error();
			REQUIRE(e.tokens_so_far.size() == 4);
			REQUIRE(e.tokens_so_far[0] == Token(TestLexicon::A, TextPos(0, 0, 0), 1));
			REQUIRE(e.tokens_so_far[1] == Token(TestLexicon::Linebreak, TextPos(0, 1, 1), 1));
			REQUIRE(e.tokens_so_far[2] == Token(TestLexicon::Indent, TextPos(1, 0, 2), 3));
			REQUIRE(e.tokens_so_far[3] == Token(TestLexicon::B, TextPos(1, 3, 5), 1));

			REQUIRE(e.text_pos == TextPos(1, 4, 6));
			REQUIRE(e.data_index == 6);
			
		}
	}
}