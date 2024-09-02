#include <catch2/catch_test_macros.hpp>

#include <utf8cpp/utf8.h>

import alccemy.lexer.patterns;
import alccemy.lexer.unicode;

using namespace alccemy;

namespace {
UnicodeCodePoint utf32(char c) {
   UnicodeCodePoint out;
   utf8::utf8to32(&c, &c + 1, &out);
   return out;
}
} // namespace

TEST_CASE("Basic Test Parsing") {
   SECTION("Text") {
      SECTION("Bare - OK") {
         Text expr("Wee;");

         REQUIRE(expr.check(utf32('W'), 0).type == LexerResults::Continue);
         REQUIRE(expr.check(utf32('e'), 1).type == LexerResults::Continue);
         REQUIRE(expr.check(utf32('e'), 2).type == LexerResults::Continue);
         REQUIRE(expr.check(utf32(';'), 3).type == LexerResults::Completed);
         REQUIRE(expr.check(utf32(';'), 3).backtrack_cols == 0);
      }
      SECTION("Bare - Fail") {
         Text expr("Wee;");

         REQUIRE(expr.check(utf32('W'), 0).type == LexerResults::Continue);
         REQUIRE(expr.check(utf32('e'), 1).type == LexerResults::Continue);
         REQUIRE(expr.check(utf32(';'), 2).type == LexerResults::Failed);
         REQUIRE(expr.check(utf32(';'), 2).backtrack_cols == 0);
      }
      SECTION("Unicode - OK") {
         Text expr("W\xc3\xa5;");

         REQUIRE(expr.check(utf32('W'), 0).type == LexerResults::Continue);
         REQUIRE(expr.check(229, 1).type == LexerResults::Continue);
         REQUIRE(expr.check(utf32(';'), 2).type == LexerResults::Completed);
         REQUIRE(expr.check(utf32(';'), 2).backtrack_cols == 0);
      }

      SECTION("Unicode - Fail") {
         Text expr("W\xc3\xa5;");

         REQUIRE(expr.check(utf32('W'), 0).type == LexerResults::Continue);
         REQUIRE(expr.check(utf32('A'), 1).type == LexerResults::Failed);
         REQUIRE(expr.check(utf32('A'), 1).backtrack_cols == 0);
      }

      SECTION("Terminate") {
         Text expr("Wee;");

         REQUIRE(expr.terminate(0).type == LexerResults::Failed);
         REQUIRE(expr.terminate(1).type == LexerResults::Failed);
      }
   }

   SECTION("AnyOf") {
      SECTION("Bare") {
         AnyOf expr("ONA");

         SECTION("Ok") {
            REQUIRE(expr.check(utf32('O'), 0).type == LexerResults::Completed);
            REQUIRE(expr.check(utf32('O'), 0).backtrack_cols == 0);

            REQUIRE(expr.check(utf32('N'), 0).type == LexerResults::Completed);
            REQUIRE(expr.check(utf32('N'), 0).backtrack_cols == 0);

            REQUIRE(expr.check(utf32('A'), 0).type == LexerResults::Completed);
            REQUIRE(expr.check(utf32('A'), 0).backtrack_cols == 0);
         }
         SECTION("Not Ok") {
            REQUIRE(expr.check(utf32('w'), 0).type == LexerResults::Failed);
            REQUIRE(expr.check(utf32('w'), 0).backtrack_cols == 0);

            REQUIRE(expr.check(utf32('i'), 0).type == LexerResults::Failed);
            REQUIRE(expr.check(utf32('i'), 0).backtrack_cols == 0);

            REQUIRE(expr.check(utf32('a'), 0).type == LexerResults::Failed);
            REQUIRE(expr.check(utf32('a'), 0).backtrack_cols == 0);
         }
         SECTION("Terminate") {
            REQUIRE(expr.terminate(0).type == LexerResults::Failed);
            REQUIRE(expr.terminate(1).type == LexerResults::Failed);
         }
      }

      SECTION("Unicode") {
         AnyOf expr("ON\xc3\xa5");

         SECTION("Ok") {
            REQUIRE(expr.check(utf32('O'), 0).type == LexerResults::Completed);
            REQUIRE(expr.check(utf32('O'), 0).backtrack_cols == 0);

            REQUIRE(expr.check(utf32('N'), 0).type == LexerResults::Completed);
            REQUIRE(expr.check(utf32('N'), 0).backtrack_cols == 0);

            REQUIRE(expr.check(229, 0).type == LexerResults::Completed);
            REQUIRE(expr.check(229, 0).backtrack_cols == 0);
         }

         SECTION("Not Ok") {
            REQUIRE(expr.check(utf32('o'), 0).type == LexerResults::Failed);
            REQUIRE(expr.check(utf32('o'), 0).backtrack_cols == 0);

            REQUIRE(expr.check(utf32('n'), 0).type == LexerResults::Failed);
            REQUIRE(expr.check(utf32('n'), 0).backtrack_cols == 0);

            REQUIRE(expr.check(230, 0).type == LexerResults::Failed);
            REQUIRE(expr.check(230, 0).backtrack_cols == 0);
         }
      }
   }

   SECTION("NotAnyOf") {
      SECTION("Bare") {
         NotAnyOf expr("ONA");

         SECTION("Ok") {
            REQUIRE(expr.check(utf32('w'), 0).type == LexerResults::Completed);
            REQUIRE(expr.check(utf32('w'), 0).backtrack_cols == 0);

            REQUIRE(expr.check(utf32('i'), 0).type == LexerResults::Completed);
            REQUIRE(expr.check(utf32('i'), 0).backtrack_cols == 0);

            REQUIRE(expr.check(utf32('a'), 0).type == LexerResults::Completed);
            REQUIRE(expr.check(utf32('a'), 0).backtrack_cols == 0);
         }
         SECTION("Not Ok") {
            REQUIRE(expr.check(utf32('O'), 0).type == LexerResults::Failed);
            REQUIRE(expr.check(utf32('O'), 0).backtrack_cols == 0);

            REQUIRE(expr.check(utf32('N'), 0).type == LexerResults::Failed);
            REQUIRE(expr.check(utf32('N'), 0).backtrack_cols == 0);

            REQUIRE(expr.check(utf32('A'), 0).type == LexerResults::Failed);
            REQUIRE(expr.check(utf32('A'), 0).backtrack_cols == 0);
         }
         SECTION("Terminate") {
            REQUIRE(expr.terminate(0).type == LexerResults::Failed);
            REQUIRE(expr.terminate(1).type == LexerResults::Failed);
         }
      }

      SECTION("Unicode") {
         NotAnyOf expr("ON\xc3\xa5");

         SECTION("Ok") {
            REQUIRE(expr.check(utf32('o'), 0).type == LexerResults::Completed);
            REQUIRE(expr.check(utf32('o'), 0).backtrack_cols == 0);

            REQUIRE(expr.check(utf32('n'), 0).type == LexerResults::Completed);
            REQUIRE(expr.check(utf32('n'), 0).backtrack_cols == 0);

            REQUIRE(expr.check(230, 0).type == LexerResults::Completed);
            REQUIRE(expr.check(230, 0).backtrack_cols == 0);
         }

         SECTION("Not Ok") {
            REQUIRE(expr.check(utf32('O'), 0).type == LexerResults::Failed);
            REQUIRE(expr.check(utf32('O'), 0).backtrack_cols == 0);

            REQUIRE(expr.check(utf32('N'), 0).type == LexerResults::Failed);
            REQUIRE(expr.check(utf32('N'), 0).backtrack_cols == 0);

            REQUIRE(expr.check(229, 0).type == LexerResults::Failed);
            REQUIRE(expr.check(229, 0).backtrack_cols == 0);
         }
      }
   }
}
TEST_CASE("Repeats") {
   SECTION("0 Repeats") {
      Repeats expr(Text("L-"));

      REQUIRE(expr.check(utf32('A'), 0).type == LexerResults::Failed);
      REQUIRE(expr.check(utf32('A'), 0).backtrack_cols == 0);
   }
   SECTION("1 Repeats") {
      Repeats expr(Text("L-"));

      REQUIRE(expr.check(utf32('L'), 0).type == LexerResults::Continue);
      REQUIRE(expr.check(utf32('-'), 1).type == LexerResults::Continue);
      REQUIRE(expr.check(utf32(';'), 2).type == LexerResults::Completed);
      REQUIRE(expr.check(utf32(';'), 2).backtrack_cols == 1);
   }
   SECTION("2 Repeats") {
      Repeats expr(Text("L-"));

      REQUIRE(expr.check(utf32('L'), 0).type == LexerResults::Continue);
      REQUIRE(expr.check(utf32('-'), 1).type == LexerResults::Continue);
      REQUIRE(expr.check(utf32('L'), 2).type == LexerResults::Continue);
      REQUIRE(expr.check(utf32('-'), 3).type == LexerResults::Continue);
      REQUIRE(expr.check(utf32(';'), 4).type == LexerResults::Completed);
      REQUIRE(expr.check(utf32(';'), 4).backtrack_cols == 1);
   }
   SECTION("2 Repeats - Failure") {
      Repeats expr(Text("L-"));

      REQUIRE(expr.check(utf32('L'), 0).type == LexerResults::Continue);
      REQUIRE(expr.check(utf32('-'), 1).type == LexerResults::Continue);
      REQUIRE(expr.check(utf32('L'), 2).type == LexerResults::Continue);
      REQUIRE(expr.check(utf32('A'), 3).type == LexerResults::Failed);
      REQUIRE(expr.check(utf32('A'), 3).backtrack_cols == 0); // No backtracking for a bona-fida failure
   }
   SECTION("Terminate") {
      Repeats expr(Text("AB"));

      SECTION("0 Repeats") { REQUIRE(expr.terminate(0).type == LexerResults::Failed); }
      SECTION("0 Partial Repeats") {
         REQUIRE(expr.check(utf32('A'), 0).type == LexerResults::Continue);
         REQUIRE(expr.terminate(1).type == LexerResults::Failed);
      }
      SECTION("1 Repeats") {
         REQUIRE(expr.check(utf32('A'), 0).type == LexerResults::Continue);
         REQUIRE(expr.check(utf32('B'), 1).type == LexerResults::Continue);
         REQUIRE(expr.terminate(2).type == LexerResults::Completed);
      }
      SECTION("1 Partial Repeats") {
         REQUIRE(expr.check(utf32('A'), 0).type == LexerResults::Continue);
         REQUIRE(expr.check(utf32('B'), 1).type == LexerResults::Continue);
         REQUIRE(expr.check(utf32('A'), 2).type == LexerResults::Continue);
         REQUIRE(expr.terminate(3).type == LexerResults::Failed);
      }
      SECTION("2 Repeats") {
         REQUIRE(expr.check(utf32('A'), 0).type == LexerResults::Continue);
         REQUIRE(expr.check(utf32('B'), 1).type == LexerResults::Continue);
         REQUIRE(expr.check(utf32('A'), 2).type == LexerResults::Continue);
         REQUIRE(expr.check(utf32('B'), 3).type == LexerResults::Continue);
         REQUIRE(expr.terminate(4).type == LexerResults::Completed);
      }
      SECTION("2 Partial Repeats") {
         REQUIRE(expr.check(utf32('A'), 0).type == LexerResults::Continue);
         REQUIRE(expr.check(utf32('B'), 1).type == LexerResults::Continue);
         REQUIRE(expr.check(utf32('A'), 2).type == LexerResults::Continue);
         REQUIRE(expr.check(utf32('B'), 3).type == LexerResults::Continue);
         REQUIRE(expr.check(utf32('A'), 4).type == LexerResults::Continue);
         REQUIRE(expr.terminate(5).type == LexerResults::Failed);
      }
   }
}

TEST_CASE("Pattern") {
   SECTION("Result") {
      Pattern expr(Text("A"), AnyOf("123N"), Text("B"));

      SECTION("OK") {
         REQUIRE(expr.check(utf32('A'), 0).type == LexerResults::Continue);
         REQUIRE(expr.check(utf32('1'), 1).type == LexerResults::Continue);
         REQUIRE(expr.check(utf32('B'), 2).type == LexerResults::Completed);
         REQUIRE(expr.check(utf32('B'), 2).backtrack_cols == 0);

         REQUIRE(expr.check(utf32('A'), 0).type == LexerResults::Continue);
         REQUIRE(expr.check(utf32('3'), 1).type == LexerResults::Continue);
         REQUIRE(expr.check(utf32('B'), 2).type == LexerResults::Completed);
         REQUIRE(expr.check(utf32('B'), 2).backtrack_cols == 0);

         REQUIRE(expr.check(utf32('A'), 0).type == LexerResults::Continue);
         REQUIRE(expr.check(utf32('N'), 1).type == LexerResults::Continue);
         REQUIRE(expr.check(utf32('B'), 2).type == LexerResults::Completed);
         REQUIRE(expr.check(utf32('B'), 2).backtrack_cols == 0);
      }

      SECTION("NOT OK") {
         REQUIRE(expr.check(utf32('A'), 0).type == LexerResults::Continue);
         REQUIRE(expr.check(utf32('N'), 1).type == LexerResults::Continue);
         REQUIRE(expr.check(utf32('1'), 2).type == LexerResults::Failed);
         REQUIRE(expr.check(utf32('1'), 2).backtrack_cols == 0);

         REQUIRE(expr.check(utf32('A'), 0).type == LexerResults::Continue);
         REQUIRE(expr.check(utf32('4'), 1).type == LexerResults::Failed);
         REQUIRE(expr.check(utf32('4'), 1).backtrack_cols == 0);
      }

      SECTION("Terminate") {
         SECTION("Partial") {
            REQUIRE(expr.check(utf32('A'), 0).type == LexerResults::Continue);
            REQUIRE(expr.terminate(1).type == LexerResults::Failed);
         }
         SECTION("Completed") {
            REQUIRE(expr.check(utf32('A'), 0).type == LexerResults::Continue);
            REQUIRE(expr.check(utf32('1'), 1).type == LexerResults::Continue);
            REQUIRE(expr.check(utf32('B'), 2).type == LexerResults::Completed);
            REQUIRE(expr.terminate(3).type == LexerResults::Failed);
         }
      }
   }
}

TEST_CASE("Patterns") {

   SECTION("OK or FAIL") {
      Patterns expr(Text("ABA"), Pattern(AnyOf("BA"), Text("C")), Repeats<Text, 1>(Text("D")));

      SECTION("OK") {
         REQUIRE(expr.check(utf32('A'), 0).type == LexerResults::Continue);
         REQUIRE(expr.check(utf32('B'), 1).type == LexerResults::Continue);
         REQUIRE(expr.check(utf32('A'), 2).type == LexerResults::Completed);

         REQUIRE(expr.check(utf32('B'), 0).type == LexerResults::Continue);
         REQUIRE(expr.check(utf32('C'), 1).type == LexerResults::Completed);

         REQUIRE(expr.check(utf32('A'), 0).type == LexerResults::Continue);
         REQUIRE(expr.check(utf32('C'), 1).type == LexerResults::Completed);

         REQUIRE(expr.check(utf32('D'), 0).type == LexerResults::Continue);
         REQUIRE(expr.check(utf32('D'), 1).type == LexerResults::Continue);
         REQUIRE(expr.check(utf32('D'), 2).type == LexerResults::Continue);
         REQUIRE(expr.check(utf32(' '), 3).type == LexerResults::Completed);
      }

      SECTION("NOT OK") {
         REQUIRE(expr.check(utf32('A'), 0).type == LexerResults::Continue);
         REQUIRE(expr.check(utf32('B'), 1).type == LexerResults::Continue);
         REQUIRE(expr.check(utf32('D'), 2).type == LexerResults::Failed);

         REQUIRE(expr.check(utf32('k'), 0).type == LexerResults::Failed);

         REQUIRE(expr.check(utf32('A'), 0).type == LexerResults::Continue);
         REQUIRE(expr.check(utf32('B'), 1).type == LexerResults::Continue);
         REQUIRE(expr.check(utf32('C'), 2).type == LexerResults::Failed);
      }
   }
   SECTION("Terminate") {
      Patterns expr(Text("ABA"), Pattern(AnyOf("BA"), Text("C")), Repeats<Text, 1>(Text("D")));
      SECTION("None Fail") { REQUIRE(expr.terminate(0).type == LexerResults::Failed); }
      SECTION("One Fail") {
         REQUIRE(expr.check(utf32('A'), 0).type == LexerResults::Continue);
         REQUIRE(expr.terminate(1).type == LexerResults::Failed);
      }
      SECTION("One Ok") {
         REQUIRE(expr.check(utf32('D'), 0).type == LexerResults::Continue);
         REQUIRE(expr.terminate(1).type == LexerResults::Completed);
      }
   }
}
