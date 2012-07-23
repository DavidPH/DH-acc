//-----------------------------------------------------------------------------
//
// Copyright(C) 2011-2012 David Hill
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------
//
// Large number handling.
//
//-----------------------------------------------------------------------------

#include "bignum.hpp"

#include "SourceException.hpp"

#include <sstream>


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

static inline int char_to_int(char c)
{
   switch(c)
   {
   case '0': case '1': case '2': case '3': case '4':
   case '5': case '6': case '7': case '8': case '9':
      return c - '0';

   case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
      return c - 'A' + 10;

   case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
      return c - 'a' + 10;

   default:
      return -1;
   }
}


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// get_bigreal
//
bigreal get_bigreal(std::string const &value, SourcePosition const &)
{
   bigreal f;
   std::istringstream iss(value);
   iss >> f;
   return f;
}

//
// get_bigsint
//
bigsint get_bigsint(std::string const &value, SourcePosition const &pos)
{
   int         base = 10;
   char const *str  = value.c_str();
   biguint     val  = 0;

   if(*str == '0')
   {
      ++str;

      if(*str == 'X' || *str == 'x')
      {
         base = 16;
         ++str;
      }
      else if(*str == 'B' || *str == 'b')
      {
         base = 2;
         ++str;
      }
      else
         base = 8;
   }

   for(char c; (c = *str) != '\0'; ++str)
   {
      int i = char_to_int(c);

      if(i > base)
         ERROR_P("character out of range");

      if(i < 0)
      {
         if(c == 'U' || c == 'u' || // unsigned
            c == 'L' || c == 'l' || // long
            c == 'H' || c == 'h')   // short
            break;

         ERROR_P("character out of range");
      }

      val *= base;
      val += i;
   }

   return val;
}

// EOF

