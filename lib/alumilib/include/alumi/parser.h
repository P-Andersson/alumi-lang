#pragma once

#include <memory>

namespace alumi
{
   namespace parser
   {
      class AlumiParser
      {
      public:
         AlumiParser();
         ~AlumiParser();

         void parse() const;

      private:
      };
   }
}