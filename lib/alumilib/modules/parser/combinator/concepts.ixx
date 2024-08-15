export module alumi.parser.combinator:concepts;

import alumi.parser.subparser;

export
{
   template<typename T>
   concept ParserElement = requires (T & element, Subparser& parent) {
      true; //element.parse(parent); // If enabled, forward declared types cannot be used and that makes recurisve rules difficult/impossible. This is thus left to true.
   };
}