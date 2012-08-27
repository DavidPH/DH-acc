//-----------------------------------------------------------------------------
//
// Copyright(C) 2012 David Hill
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
// C number parsing.
//
//-----------------------------------------------------------------------------

#include "../SourceExpressionC.hpp"

#include "../SourceException.hpp"


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpressionC::ParseInt
//
SourceExpression::Pointer SourceExpressionC::ParseInt(std::string const &value, SRCEXP_EXPR_ARGS)
{
   static biguint const maxs    = 0x7FFFFFFF;
   static biguint const maxs_L  = 0x7FFFFFFFFFFFFFFF;
   static biguint const maxs_LL = 0x7FFFFFFFFFFFFFFF;

   static biguint const maxu    = 0xFFFFFFFF;
   static biguint const maxu_L  = 0xFFFFFFFFFFFFFFFF;
   static biguint const maxu_LL = 0xFFFFFFFFFFFFFFFF;

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
      int i = ParseNumber(c);

      if(i >= base)
         Error_P("character out of range '%c'", c);

      if (i < 0)
      {
         if(c == 'U' || c == 'u' || // unsigned
            c == 'L' || c == 'l')   // long
            break;

         Error_P("character out of range '%c'", c);
      }

      val *= base;
      val += i;
   }

   bool isU  = false; // Only accept unsigned.
   bool isUU = base != 10; // Non-decimals try to promote to unsigned.
   bool isL  = false; // Start at long.
   bool isLL = false; // Start at long long

   for(char c; (c = *str++) != '\0';)
   {
      switch(c)
      {
      case 'U':
      case 'u':
         isU = true;
         break;

      case 'L':
      case 'l':
         if(*str == c) { isLL = true; ++str; } else isL = true;
         break;

      default:
         Error_P("expected suffix character, got '%c'", c);
      }
   }

   if(isL)  goto dointL;
   if(isLL) goto dointLL;

   if(!isU && val <= maxs)
      return create_value_int(val, context, pos);

   if((isU || isUU) && val <= maxu)
      return create_value_uint(val, context, pos);

dointL:
   if(!isU && val <= maxs_L)
      return create_value_long(val, context, pos);

   if((isU || isUU) && val <= maxu_L)
      return create_value_ulong(val, context, pos);

dointLL:
   if(!isU && val <= maxs_LL)
      return create_value_llong(val, context, pos);

   if((isU || isUU) && val <= maxu_LL)
      return create_value_ullong(val, context, pos);

   Error_P("integer literal out of range");
}

// EOF

