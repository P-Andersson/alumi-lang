import alccemy;

using namespace alccemy;

namespace {

enum class Tokens {
   Number,
   Plus,
   Minus,
   Multiply,
   Divide,
   OpenParen,
   CloseParen,
   EndOfFile,
};

auto lexer = create_lexer<Tokens>(PatternSet{
    Tokenize(Pattern(
       Repeats(AnyOf("0123456789")), 
       Repeats<Text, 0, 1>(Text(".")), 
       Repeats<AnyOf, 0>(AnyOf("0123456789"))), 
      Tokens::Number),
    Tokenize(Text("+"), Tokens::Plus),
    Tokenize(Text("-"), Tokens::Minus),
    Tokenize(Text("*"), Tokens::Multiply),
    Tokenize(Text("/"), Tokens::Divide),
    Tokenize(Text("("), Tokens::OpenParen),
    Tokenize(Text(")"), Tokens::CloseParen),
    AnyOf(" \t\n"),
});

} // namespace

int main(int argc, char** argv) {
   auto res = lexer.lexUtf8("4 + 2(3 -1)/4");
   return res.has_value() ? 0 : 1;
}
