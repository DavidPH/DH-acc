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
// DS number parsing.
//
//-----------------------------------------------------------------------------

#include "../SourceExpressionDS.hpp"

#include "../SourceException.hpp"


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

static inline int char_to_int(char c)
{
   switch (c)
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
// SourceExpressionDS::create_value_integer
//
SourceExpression::Pointer SourceExpressionDS::create_value_integer
(std::string const &value, SRCEXP_EXPR_ARGS)
{
   static bigsint const maxs_HH = 0x7FFFFFFF;
   static bigsint const maxs_H  = 0x7FFFFFFF;
   static bigsint const maxs    = 0x7FFFFFFF;
   static bigsint const maxs_L  = 0x7FFFFFFF;
   static bigsint const maxs_LL = 0x7FFFFFFFFFFFFFFF; // FIXME

   static bigsint const maxu_HH = 0xFFFFFFFF;
   static bigsint const maxu_H  = 0xFFFFFFFF;
   static bigsint const maxu    = 0xFFFFFFFF;
   static bigsint const maxu_L  = 0xFFFFFFFF;
   static bigsint const maxu_LL = 0xFFFFFFFFFFFFFFFF; // FIXME

   int         base = 10;
   char const *str  = value.c_str();
   bigsint     val  = 0;

   if (*str == '0')
   {
      ++str;

      if (*str == 'X' || *str == 'x')
      {
         base = 16;
         ++str;
      }
      else if (*str == 'B' || *str == 'b')
      {
         base = 2;
         ++str;
      }
      else
         base = 8;
   }

   for (char c; (c = *str) != '\0'; ++str)
   {
      int i = char_to_int(c);

      if (i > base)
         throw SourceException("character out of range", position, __func__);

      if (i < 0)
      {
         if (c == 'U' || c == 'u' || // unsigned
             c == 'L' || c == 'l' || // long
             c == 'H' || c == 'h')   // short
            break;

         throw SourceException("character out of range", position, __func__);
      }

      val *= base;
      val += i;
   }

   bool isU  = false; // Only accept unsigned.
   bool isUU = base != 10; // Non-decimals try to promote to unsigned.
   bool isL  = false; // Start at long.
   bool isLL = false; // Start at long long
   bool isH  = false; // Start at short.
   bool isHH = false; // Start at char.

   for (char c; (c = *str++) != '\0';)
   {
      switch (c)
      {
      case 'U':
      case 'u':
         isU = true;
         break;

      case 'L':
      case 'l':
         if (c == *str)
         {
            isLL = true;
            ++str;
         }
         else
            isL = true;

         break;

      case 'H':
      case 'h':
         if (c == *str)
         {
            isHH = true;
            ++str;
         }
         else
            isH = true;

         break;

      default:
         throw SourceException("character out of range", position, __func__);
      }
   }

   if (isHH) goto dointHH;
   if (isH)  goto dointH;
   if (isL)  goto dointL;
   if (isLL) goto dointLL;
   goto doint;

dointHH:
   if (!isU && val <= maxs_HH)
      return create_value_schar(val, context, position);

   if ((isU || isUU) && val <= maxu_HH)
      return create_value_uchar(val, context, position);

dointH:
   if (!isU && val <= maxs_H)
      return create_value_short(val, context, position);

   if ((isU || isUU) && val <= maxu_H)
      return create_value_ushort(val, context, position);

doint:
   if (!isU && val <= maxs)
      return create_value_int(val, context, position);

   if ((isU || isUU) && val <= maxu)
      return create_value_uint(val, context, position);

dointL:
   if (!isU && val <= maxs_L)
      return create_value_long(val, context, position);

   if ((isU || isUU) && val <= maxu_L)
      return create_value_ulong(val, context, position);

dointLL:
   if (!isU /*&& val <= maxs_LL*/) // FIXME
      return create_value_llong(val, context, position);

   if ((isU || isUU) /*&& val <= maxu_LL*/) // FIXME
      return create_value_ullong(val, context, position);

   throw SourceException("integer literal out of range", position, __func__);
}

// EOF

