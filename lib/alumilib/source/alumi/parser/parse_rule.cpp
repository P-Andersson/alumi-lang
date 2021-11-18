#include "alumi/parser/parse_rule.h"


namespace alumi
{
   namespace parser
   {
      void NeverSynchroize::do_synch(Subparser& parser)
      {
         return;
      }
   }
}
