#include "catch2/catch.hpp"

#include "alumi/parser/parser_parts.h"

using namespace alumi;
using namespace alumi::parser;


TEST_CASE("Test Parser Parts")
{
	SECTION("Is")
	{
		Is<TokenType::Symbol> element;

		SECTION("Ok")
		{
			std::vector<Token> tokens{
				Token(TokenType::Symbol, TextPos(0, 0, 0), 2)
			};
			Subparser parser(tokens);

			auto res = element.parse(parser);
			REQUIRE(res.get_type() == ParseResult::Type::Success);
			REQUIRE(res.get_consumed() == 1);
			//REQUIRE(res.get_nodes().size() == 0);
		}

		SECTION("Failure Wrong Token")
		{
			std::vector<Token> tokens{
				Token(TokenType::Literal, TextPos(0, 0, 0), 2)
			};
			Subparser parser(tokens);
			
			auto res = element.parse(parser);
			REQUIRE(res.get_type() == ParseResult::Type::Failure);
			REQUIRE(res.get_consumed() == 1);
			//REQUIRE(res.get_nodes().size() == 0);
		}
	}

	SECTION("Swallow")
	{
		Swallow<Is<TokenType::Symbol>, TokenType::Indent> element;

		SECTION("Ok")
		{
			std::vector<Token> tokens{
				Token(TokenType::Indent, TextPos(0, 0, 0), 2),
				Token(TokenType::Indent, TextPos(0, 2, 2), 2),
				Token(TokenType::Symbol, TextPos(0, 4, 4), 2)
			};
			Subparser parser(tokens);

			auto res = element.parse(parser);
			REQUIRE(res.get_type() == ParseResult::Type::Success);
			REQUIRE(res.get_consumed() == 3);
			//REQUIRE(res.get_nodes().size() == 0);
		}

		SECTION("Failure Wrong Token")
		{
			std::vector<Token> tokens{
				Token(TokenType::Indent, TextPos(0, 2, 2), 2),
				Token(TokenType::Literal, TextPos(0, 0, 0), 2)
			};
			Subparser parser(tokens);

			auto res = element.parse(parser);
			REQUIRE(res.get_type() == ParseResult::Type::Failure);
			REQUIRE(res.get_consumed() == 2);
			//REQUIRE(res.get_nodes().size() == 0);
		}
	}


	SECTION("Optional")
	{
		Optional<Is<TokenType::Operator>> element;

		SECTION("Ok Right")
		{
			std::vector<Token> tokens{
				Token(TokenType::Operator, TextPos(0, 0, 0), 2)
			};
			Subparser parser(tokens);

			auto res = element.parse(parser);
			REQUIRE(res.get_type() == ParseResult::Type::Success);
			REQUIRE(res.get_consumed() == 1);
			//REQUIRE(res.get_nodes().size() == 0);
		}

		SECTION("Ok Wrong")
		{
			std::vector<Token> tokens{
				Token(TokenType::Symbol, TextPos(0, 0, 0), 2)
			};
			Subparser parser(tokens);

			auto res = element.parse(parser);
			REQUIRE(res.get_type() == ParseResult::Type::Success);
			REQUIRE(res.get_consumed() == 0);
			//REQUIRE(res.get_nodes().size() == 0);
		}
	}

	SECTION("Sequence")
	{
		Sequence<Is<TokenType::Operator>, Is<TokenType::Symbol>> element;

		SECTION("Ok")
		{
			std::vector<Token> tokens{
				Token(TokenType::Operator, TextPos(0, 0, 0), 1),
				Token(TokenType::Symbol, TextPos(0, 1, 1), 1),
			};

			Subparser parser(tokens);

			auto res = element.parse(parser);
			REQUIRE(res.get_type() == ParseResult::Type::Success);
			REQUIRE(res.get_consumed() == 2);
			//REQUIRE(res.get_nodes().size() == 2);
		}

		SECTION("Mismatch Last")
		{
			std::vector<Token> tokens{
				Token(TokenType::Operator, TextPos(0, 0, 0), 1),
				Token(TokenType::Operator, TextPos(0, 1, 1), 1),
			};

			Subparser parser(tokens);

			auto res = element.parse(parser);
			REQUIRE(res.get_type() == ParseResult::Type::Failure);
			REQUIRE(res.get_consumed() == 2);
			//REQUIRE(res.get_nodes().size() == 2);

		}

		SECTION("Mismatch First")
		{
			std::vector<Token> tokens{
				Token(TokenType::Symbol, TextPos(0, 0, 0), 1),
				Token(TokenType::Operator, TextPos(0, 1, 1), 1),
			};

			Subparser parser(tokens);

			auto res = element.parse(parser);
			REQUIRE(res.get_type() == ParseResult::Type::Failure);
			REQUIRE(res.get_consumed() == 1);
			//REQUIRE(res.get_nodes().size() == 1);
		}
	}

	SECTION("Skip")
	{
		SECTION("Ok - Ignore")
		{
			Sequence<Peek<Is<TokenType::Symbol>>, Is<TokenType::Symbol>> element;

			std::vector<Token> tokens{
				Token(TokenType::Symbol, TextPos(0, 0, 0), 2)
			};
			Subparser parser(tokens);

			auto res = element.parse(parser);
			REQUIRE(res.get_type() == ParseResult::Type::Success);
			REQUIRE(res.get_consumed() == 1);
		}

		SECTION("Failure Wrong Peek Token")
		{
			Sequence<Peek<Is<TokenType::Literal>>, Is<TokenType::Symbol>> element;

			std::vector<Token> tokens{
				Token(TokenType::Symbol, TextPos(0, 0, 0), 2)
			};
			Subparser parser(tokens);

			auto res = element.parse(parser);
			REQUIRE(res.get_type() == ParseResult::Type::Failure);
			REQUIRE(res.get_consumed() == 0);
		}
	}

	SECTION("Repeats")
	{
		Repeats< Sequence<Is<TokenType::Operator>, Is<TokenType::Symbol>> > element;
		SECTION("Ok Zero")
		{
			std::vector<Token> tokens{
				Token(TokenType::Symbol, TextPos(0, 1, 1), 1),
			};

			Subparser parser(tokens);

			auto res = element.parse(parser);
			REQUIRE(res.get_type() == ParseResult::Type::Success);
			REQUIRE(res.get_consumed() == 0);
			//REQUIRE(res.get_nodes().size() == 0);
		}
		SECTION("Ok Once")
		{
			std::vector<Token> tokens{
				Token(TokenType::Operator, TextPos(0, 0, 0), 1),
				Token(TokenType::Symbol, TextPos(0, 1, 1), 1),
				Token(TokenType::Seperator, TextPos(0, 2, 2), 1),
			};

			Subparser parser(tokens);

			auto res = element.parse(parser);
			REQUIRE(res.get_type() == ParseResult::Type::Success);
			REQUIRE(res.get_consumed() == 2);
			//REQUIRE(res.get_nodes().size() == 1);
		}
		SECTION("Ok Twice")
		{
			std::vector<Token> tokens{
				Token(TokenType::Operator, TextPos(0, 0, 0), 1),
				Token(TokenType::Symbol, TextPos(0, 1, 1), 1),
				Token(TokenType::Operator, TextPos(0, 2, 2), 1),
				Token(TokenType::Symbol, TextPos(0, 3, 3), 1),
				Token(TokenType::Seperator, TextPos(0, 4, 4), 1),
			};

			Subparser parser(tokens);

			auto res = element.parse(parser);
			REQUIRE(res.get_type() == ParseResult::Type::Success);
			REQUIRE(res.get_consumed() == 4);
			//REQUIRE(res.get_nodes().size() == 2);
		}
		SECTION("Fail")
		{
			std::vector<Token> tokens{
				Token(TokenType::Operator, TextPos(0, 0, 0), 1),
				Token(TokenType::Symbol, TextPos(0, 1, 1), 1),
				Token(TokenType::Operator, TextPos(0, 2, 2), 1),
				Token(TokenType::Seperator, TextPos(0, 3, 3), 1),
			};

			Subparser parser(tokens);

			auto res = element.parse(parser);
			REQUIRE(res.get_type() == ParseResult::Type::Success);
			REQUIRE(res.get_consumed() == 2);
			//REQUIRE(res.get_nodes().size() == 1);
		}
	}

	SECTION("Repeats with Seperator")
	{
		RepeatsWithSeperator< Sequence<Is<TokenType::Operator>, Is<TokenType::Symbol>>, TokenType::Seperator> element;

		SECTION("Ok Zero")
		{
			std::vector<Token> tokens{
				Token(TokenType::Symbol, TextPos(0, 1, 1), 1),
			};

			Subparser parser(tokens);

			auto res = element.parse(parser);
			REQUIRE(res.get_type() == ParseResult::Type::Success);
			REQUIRE(res.get_consumed() == 0);
			//REQUIRE(res.get_nodes().size() == 0);

		}
		SECTION("Ok Once")
		{
			std::vector<Token> tokens{
				Token(TokenType::Operator, TextPos(0, 0, 0), 1),
				Token(TokenType::Symbol, TextPos(0, 1, 1), 1),
				Token(TokenType::Symbol, TextPos(0, 2, 2), 1),
			};

			Subparser parser(tokens);

			auto res = element.parse(parser);
			REQUIRE(res.get_type() == ParseResult::Type::Success);
			REQUIRE(res.get_consumed() == 2);
			//REQUIRE(res.get_nodes().size() == 1);

		}
		SECTION("Ok Once With Seperator")
		{
			std::vector<Token> tokens{
				Token(TokenType::Operator, TextPos(0, 0, 0), 1),
				Token(TokenType::Symbol, TextPos(0, 1, 1), 1),
				Token(TokenType::Seperator, TextPos(0, 2, 2), 1),
				Token(TokenType::Symbol, TextPos(0, 3, 3), 1)
			};

			Subparser parser(tokens);

			auto res = element.parse(parser);
			REQUIRE(res.get_type() == ParseResult::Type::Success);
			REQUIRE(res.get_consumed() == 3);
			//REQUIRE(res.get_nodes().size() == 1);

		}
		SECTION("Ok Twice")
		{
			std::vector<Token> tokens{
				Token(TokenType::Operator, TextPos(0, 0, 0), 1),
				Token(TokenType::Symbol, TextPos(0, 1, 1), 1),
				Token(TokenType::Seperator, TextPos(0, 2, 2), 1),
				Token(TokenType::Operator, TextPos(0, 3, 3), 1),
				Token(TokenType::Symbol, TextPos(0, 4, 4), 1),
				Token(TokenType::Symbol, TextPos(0, 5, 5), 1)
			};

			Subparser parser(tokens);

			auto res = element.parse(parser);
			REQUIRE(res.get_type() == ParseResult::Type::Success);
			REQUIRE(res.get_consumed() == 5);
			//REQUIRE(res.get_nodes().size() == 2);
		}
		SECTION("Ok Twice With Seperator")
		{
			std::vector<Token> tokens{
				Token(TokenType::Operator, TextPos(0, 0, 0), 1),
				Token(TokenType::Symbol, TextPos(0, 1, 1), 1),
				Token(TokenType::Seperator, TextPos(0, 2, 2), 1),
				Token(TokenType::Operator, TextPos(0, 3, 3), 1),
				Token(TokenType::Symbol, TextPos(0, 4, 4), 1),
				Token(TokenType::Seperator, TextPos(0, 5, 5), 1),
				Token(TokenType::Symbol, TextPos(0, 6, 6), 1)
			};

			Subparser parser(tokens);

			auto res = element.parse(parser);
			REQUIRE(res.get_type() == ParseResult::Type::Success);
			REQUIRE(res.get_consumed() == 6);
			//REQUIRE(res.get_nodes().size() == 2);
		}
		SECTION("Ok Once due to Missing Seperator")
		{
			std::vector<Token> tokens{
				Token(TokenType::Operator, TextPos(0, 0, 0), 1),
				Token(TokenType::Symbol, TextPos(0, 1, 1), 1),
				Token(TokenType::Operator, TextPos(0, 2, 2), 1),
				Token(TokenType::Symbol, TextPos(0, 3, 3), 1)
			};

			Subparser parser(tokens);

			auto res = element.parse(parser);
			REQUIRE(res.get_type() == ParseResult::Type::Success);
			REQUIRE(res.get_consumed() == 2);
			//REQUIRE(res.get_nodes().size() == 1 );
		}
	}

	SECTION("Indent")
	{
		SECTION("Ok")
		{
			Sequence<Indented, Is<TokenType::Symbol>> element;

			std::vector<Token> tokens{
				Token(TokenType::Indent, TextPos(0, 0, 0), 1),
				Token(TokenType::Symbol, TextPos(0, 1, 1), 1),
			};

			Subparser parser(tokens);

			auto res = element.parse(parser);
			REQUIRE(res.get_type() == ParseResult::Type::Success);
			REQUIRE(res.get_consumed() == 2);
		}
		SECTION("Ok - Stepped")
		{
			Repeats< Sequence<Indented, Is<TokenType::Symbol>>> element;

			std::vector<Token> tokens{
				Token(TokenType::Indent, TextPos(0, 0, 0), 1),
				Token(TokenType::Symbol, TextPos(0, 1, 1), 1),
   			Token(TokenType::Indent, TextPos(0, 2, 2), 2),
				Token(TokenType::Symbol, TextPos(0, 3, 3), 1),
				Token(TokenType::EndOfFile, TextPos(0, 4, 4), 0),
			};

			Subparser parser(tokens);

			auto res = element.parse(parser);
			REQUIRE(res.get_type() == ParseResult::Type::Success);
			REQUIRE(res.get_consumed() == 4);
		}
		SECTION("Not Ok Flat")
		{
			Sequence<Indented, Is<TokenType::Symbol>, Indented, Is<TokenType::Symbol>> element;

			std::vector<Token> tokens{
				Token(TokenType::Indent, TextPos(0, 0, 0), 1),
				Token(TokenType::Symbol, TextPos(0, 1, 1), 1),
				Token(TokenType::Indent, TextPos(0, 2, 2), 1),
				Token(TokenType::Symbol, TextPos(0, 3, 3), 1),
       		Token(TokenType::EndOfFile, TextPos(0, 4, 4), 0),
			};

			Subparser parser(tokens);

			auto res = element.parse(parser);
			REQUIRE(res.get_type() == ParseResult::Type::Failure);
			REQUIRE(res.get_consumed() == 3);
		}
	}

	SECTION("Dedented")
	{
		SECTION("Ok")
		{
			Sequence<Indented, Is<TokenType::Symbol>, Dedented, Is<TokenType::Symbol>> element;

			std::vector<Token> tokens{
				Token(TokenType::Indent, TextPos(0, 0, 0), 1),
				Token(TokenType::Symbol, TextPos(0, 1, 1), 1),
				Token(TokenType::Indent, TextPos(0, 0, 0), 0),
				Token(TokenType::Symbol, TextPos(0, 1, 1), 1),
			};

			Subparser parser(tokens);

			auto res = element.parse(parser);
			REQUIRE(res.get_type() == ParseResult::Type::Success);
			REQUIRE(res.get_consumed() == 4);
		}
		SECTION("Ok - Stepped")
		{
			Sequence<Repeats<Indented>, Repeats<Sequence<Dedented, Is<TokenType::Symbol>>>> element;

			std::vector<Token> tokens{
				Token(TokenType::Indent, TextPos(0, 0, 0), 1),
				Token(TokenType::Indent, TextPos(0, 0, 0), 2),
				Token(TokenType::Indent, TextPos(0, 0, 0), 1),
				Token(TokenType::Symbol, TextPos(0, 1, 1), 1),
				Token(TokenType::Indent, TextPos(0, 2, 2), 0),
				Token(TokenType::Symbol, TextPos(0, 3, 3), 1),
				Token(TokenType::EndOfFile, TextPos(0, 4, 4), 0),
			};

			Subparser parser(tokens);

			auto res = element.parse(parser);
			REQUIRE(res.get_type() == ParseResult::Type::Success);
			REQUIRE(res.get_consumed() == 6);
		}
		SECTION("Not Ok - Flat")
		{
			Sequence<Indented, Is<TokenType::Symbol>, Dedented, Is<TokenType::Symbol>> element;

			std::vector<Token> tokens{
				Token(TokenType::Indent, TextPos(0, 0, 0), 1),
				Token(TokenType::Symbol, TextPos(0, 1, 1), 1),
				Token(TokenType::Indent, TextPos(0, 0, 0), 1),
				Token(TokenType::Symbol, TextPos(0, 1, 1), 1),
			};

			Subparser parser(tokens);

			auto res = element.parse(parser);
			REQUIRE(res.get_type() == ParseResult::Type::Failure);
			REQUIRE(res.get_consumed() == 3);
		}
		SECTION("Not Ok - Indented")
		{
			Sequence<Indented, Is<TokenType::Symbol>, Dedented, Is<TokenType::Symbol>> element;

			std::vector<Token> tokens{
				Token(TokenType::Indent, TextPos(0, 0, 0), 1),
				Token(TokenType::Symbol, TextPos(0, 1, 1), 1),
				Token(TokenType::Indent, TextPos(0, 0, 0), 2),
				Token(TokenType::Symbol, TextPos(0, 1, 1), 1),
			};

			Subparser parser(tokens);

			auto res = element.parse(parser);
			REQUIRE(res.get_type() == ParseResult::Type::Failure);
			REQUIRE(res.get_consumed() == 3);
		}

	}


	SECTION("NoIdent")
	{
		SECTION("Ok")
		{
			Sequence<Indented, Is<TokenType::Symbol>, NoIndent, Is<TokenType::Symbol>> element;

			std::vector<Token> tokens{
				Token(TokenType::Indent, TextPos(0, 0, 0), 1),
				Token(TokenType::Symbol, TextPos(0, 1, 1), 1),
				Token(TokenType::Indent, TextPos(0, 0, 0), 1),
				Token(TokenType::Symbol, TextPos(0, 1, 1), 1),
			};

			Subparser parser(tokens);

			auto res = element.parse(parser);
			REQUIRE(res.get_type() == ParseResult::Type::Success);
			REQUIRE(res.get_consumed() == 4);
		}
		SECTION("Not Ok - Indent")
		{
			Sequence<Indented, Is<TokenType::Symbol>, NoIndent, Is<TokenType::Symbol>> element;

			std::vector<Token> tokens{
				Token(TokenType::Indent, TextPos(0, 0, 0), 1),
				Token(TokenType::Symbol, TextPos(0, 1, 1), 1),
				Token(TokenType::Indent, TextPos(0, 0, 0), 2),
				Token(TokenType::Symbol, TextPos(0, 1, 1), 1),
			};

			Subparser parser(tokens);

			auto res = element.parse(parser);
			REQUIRE(res.get_type() == ParseResult::Type::Failure);
			REQUIRE(res.get_consumed() == 3);
		}
		SECTION("Not Ok - Dedent")
		{
			Sequence<Indented, Is<TokenType::Symbol>, NoIndent, Is<TokenType::Symbol>> element;

			std::vector<Token> tokens{
				Token(TokenType::Indent, TextPos(0, 0, 0), 1),
				Token(TokenType::Symbol, TextPos(0, 1, 1), 1),
				Token(TokenType::Indent, TextPos(0, 0, 0), 0),
				Token(TokenType::Symbol, TextPos(0, 1, 1), 1),
			};

			Subparser parser(tokens);

			auto res = element.parse(parser);
			REQUIRE(res.get_type() == ParseResult::Type::Failure);
			REQUIRE(res.get_consumed() == 3);
		}
		SECTION("Not Ok - After dedent")
		{
			Sequence<Indented, Indented, Is<TokenType::Symbol>, Peek<Dedented>, NoIndent, Is<TokenType::Symbol>> element;

			std::vector<Token> tokens{
				Token(TokenType::Indent, TextPos(0, 0, 0), 0),
				Token(TokenType::Indent, TextPos(0, 0, 0), 2),
				Token(TokenType::Symbol, TextPos(0, 1, 1), 1),
				Token(TokenType::Indent, TextPos(0, 0, 0), 1),
				Token(TokenType::Symbol, TextPos(0, 1, 1), 1),
			};

			Subparser parser(tokens);

			auto res = element.parse(parser);
			REQUIRE(res.get_type() == ParseResult::Type::Failure);
			REQUIRE(res.get_consumed() == 4);
		}
	}

	SECTION("AnyOf")
	{
		AnyOf< 
			Is<TokenType::Operator>, 
			Swallow<Sequence<Is<TokenType::Indent>, Is<TokenType::Symbol>>, TokenType::For >,
			Is<TokenType::If>> element;

		SECTION("Ok First")
		{
			std::vector<Token> tokens{
				Token(TokenType::Operator, TextPos(0, 1, 1), 1),
			};

			Subparser parser(tokens);

			auto res = element.parse(parser);
			REQUIRE(res.get_type() == ParseResult::Type::Success);
			REQUIRE(res.get_consumed() == 1);
			//REQUIRE(res.get_nodes().size() == 1);
		}
		SECTION("Ok Third")
		{
			std::vector<Token> tokens{
				Token(TokenType::If, TextPos(0, 1, 1), 1),
			};

			Subparser parser(tokens);

			auto res = element.parse(parser);
			REQUIRE(res.get_type() == ParseResult::Type::Success);
			REQUIRE(res.get_consumed() == 1);
			//REQUIRE(res.get_nodes().size() == 1);
		}
		SECTION("Ok Second")
		{
			std::vector<Token> tokens{
				Token(TokenType::Indent, TextPos(0, 1, 1), 1),
				Token(TokenType::For, TextPos(0, 2, 2), 1),
				Token(TokenType::For, TextPos(0, 3, 3), 1),
				Token(TokenType::Symbol, TextPos(0, 4, 4), 1)
			};

			Subparser parser(tokens);

			auto res = element.parse(parser);
			REQUIRE(res.get_type() == ParseResult::Type::Success);
			REQUIRE(res.get_consumed() == 4);
			//REQUIRE(res.get_nodes().size() == 1);
		}

		SECTION("Not Ok First")
		{
			std::vector<Token> tokens{
				Token(TokenType::SubscopeBegin, TextPos(0, 1, 1), 1)
			};

			Subparser parser(tokens);

			auto res = element.parse(parser);
			REQUIRE(res.get_type() == ParseResult::Type::Failure);
			REQUIRE(res.get_consumed() == 1);
			//REQUIRE(res.get_nodes().size() == 1);
		}

		SECTION("Not Ok Second")
		{
			std::vector<Token> tokens{
				Token(TokenType::Indent, TextPos(0, 1, 1), 1),
				Token(TokenType::For, TextPos(0, 2, 2), 1),
				Token(TokenType::For, TextPos(0, 3, 3), 1),
				Token(TokenType::Operator, TextPos(0, 4, 4), 1)
			};

			Subparser parser(tokens);

			auto res = element.parse(parser);
			REQUIRE(res.get_type() == ParseResult::Type::Failure);
			REQUIRE(res.get_consumed() == 4);
			//REQUIRE(res.get_nodes().size() == 1);
		}
	}
}