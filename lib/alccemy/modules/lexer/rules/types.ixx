export module alccemy.lexer.rules.types;

export namespace alccemy
{
   enum class RulesResult
   {
      Continue, // Continue parsing for this unicode character
      TerminateTokens, // Continue parsing this character, but terminate all ongoing parsings afterwards
      Consume, // Consumes current unique character and prevents 
               // subsequence rules and normal parsing from seeing this character   
   };
}

