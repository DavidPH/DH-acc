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

#include "../ObjectExpression.hpp"
#include "../option.hpp"
#include "../SourceContext.hpp"
#include "../SourceException.hpp"
#include "../SourceTokenC.hpp"
#include "../SourceTokenizerC.hpp"
#include "../SourceVariable.hpp"


//----------------------------------------------------------------------------|
// Static Variables                                                           |
//

static option::option_data<bool> option_fixed_constants('\0', "fixed-constants",
 "features", "Makes unsuffixed floating-constants be fixed-constants. On by default.",
 NULL, true);


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpressionC::ParseFloat
//
SourceExpression::Pointer SourceExpressionC::ParseFloat(std::string const &value, SRCEXP_EXPR_ARGS)
{
   char const *str  = value.c_str();

   int base;
   char expL, expU;

   // Hexadecimal float.
   if(str[0] == '0' && str[1] == 'x')
   {
      str += 2;
      base = 16;
      expL = 'p';
      expU = 'P';
   }
   else
   {
      base = 10;
      expL = 'e';
      expU = 'E';
   }

   // Parse integral part.
   bigreal valI = 0;
   for(; *str; ++str)
   {
      int i = ParseNumber(*str);

      if(0 > i || i >= base) break;

      valI *= base;
      valI += i;
   }

   // Parse fractional part, if any.
   bigreal valF = 0;
   if(*str == '.')
   {
      for(++str; *str; ++str)
      {
         int i = ParseNumber(*str);

         if(0 > i || i >= base) break;
      }

      for(char const *s = str; *--s != '.';)
      {
         valF += ParseNumber(*s);
         valF /= base;
      }
   }

   // Parse exponent part, if any.
   int valE = 0, valS = 0;
   if(*str == expL || *str == expU)
   {
         if(*++str == '-') ++str, valS = -1;
      else if(*str == '+') ++str, valS = +1;

      for(; *str; ++str)
      {
         int i = ParseNumber(*str);

         if(0 > i || i >= 10) break;

         valE *= 10;
         valE += i;
      }
   }

   // Parse suffix part, if any.
   int sufF = 0, sufL = 0, sufU = 0, sufH = 0, sufK = 0, sufR = 0;
   while(*str) switch(*str++)
   {
   case 'F': case 'f': ++sufF; break;
   case 'H': case 'h': ++sufH; break;
   case 'K': case 'k': ++sufK; break;
   case 'L': case 'l': ++sufL; break;
   case 'R': case 'r': ++sufR; break;
   case 'U': case 'u': ++sufU; break;
   default: Error_P("unexpected character in floating-constant: '%c'", *--str);
   }

   // Process integer and fractional part.
   bigreal val = valI + valF;

   // Process exponent part.
   if(base == 10)
   {
      if(valS < 0) while(valE--) val /= 10;
      else         while(valE--) val *= 10;
   }
   else
   {
      if(valS < 0) while(valE--) val /= 2;
      else         while(valE--) val *= 2;
   }

   // Process suffix part.
   if(!sufF && !sufK && !sufR)
   {
      if(option_fixed_constants.data && !sufH && !sufL && !sufU)
         ++sufK;
      else
         ++sufF, ++sufL;
   }

   if(sufF > 1) Error_P("floating-constant: FF");
   if(sufH > 2) Error_P("floating-constant: HHH");
   if(sufK > 1) Error_P("floating-constant: KK");
   if(sufL > 2) Error_P("floating-constant: LLL");
   if(sufR > 1) Error_P("floating-constant: RR");
   if(sufU > 1) Error_P("floating-constant: UU");

   if(sufH && sufL) Error_P("floating-constant: HL");
   if(sufF && sufK) Error_P("floating-constant: FK");
   if(sufF && sufR) Error_P("floating-constant: FR");
   if(sufK && sufR) Error_P("floating-constant: KR");

   // Create value.
   if(sufF)
   {
      if(sufU)
      {
         Error_P("floating-constant: FU");
      }
      else
      {
         if(sufH == 2) return create_value_flt_hh(val, context, pos);
         if(sufH == 1) return create_value_flt_h(val, context, pos);
         if(sufL == 1) return create_value_flt_l(val, context, pos);
         if(sufL == 2) return create_value_flt_ll(val, context, pos);
                       return create_value_flt(val, context, pos);
      }
   }
   else if(sufK)
   {
      if(sufU)
      {
         if(sufH == 2) return create_value_acc_hh(val, context, pos);
         if(sufH == 1) return create_value_acc_h(val, context, pos);
         if(sufL == 1) return create_value_acc_l(val, context, pos);
         if(sufL == 2) return create_value_acc_ll(val, context, pos);
                       return create_value_acc(val, context, pos);
      }
      else
      {
         if(sufH == 2) return create_value_fix_hh(val, context, pos);
         if(sufH == 1) return create_value_fix_h(val, context, pos);
         if(sufL == 1) return create_value_fix_l(val, context, pos);
         if(sufL == 2) return create_value_fix_ll(val, context, pos);
                       return create_value_fix(val, context, pos);
      }
   }
   else if(sufR)
   {
      if(sufU)
      {
         if(sufH == 2) return create_value_ang_hh(val, context, pos);
         if(sufH == 1) return create_value_ang_h(val, context, pos);
         if(sufL == 1) return create_value_ang_l(val, context, pos);
         if(sufL == 2) return create_value_ang_ll(val, context, pos);
                       return create_value_ang(val, context, pos);
      }
      else
      {
         if(sufH == 2) return create_value_fra_hh(val, context, pos);
         if(sufH == 1) return create_value_fra_h(val, context, pos);
         if(sufL == 1) return create_value_fra_l(val, context, pos);
         if(sufL == 2) return create_value_fra_ll(val, context, pos);
                       return create_value_fra(val, context, pos);
      }
   }

   Error_P("floating-constant out of range");
}

//
// SourceExpressionC::ParseInt
//
SourceExpression::Pointer SourceExpressionC::ParseInt(std::string const &value, SRCEXP_EXPR_ARGS)
{
   static biguint const maxs_HH = 0x7FFFFFFF;
   static biguint const maxs_H  = 0x7FFFFFFF;
   static biguint const maxs    = 0x7FFFFFFF;
   static biguint const maxs_L  = 0x7FFFFFFFFFFFFFFF;
   static biguint const maxs_LL = 0x7FFFFFFFFFFFFFFF;

   static biguint const maxu_HH = 0xFFFFFFFF;
   static biguint const maxu_H  = 0xFFFFFFFF;
   static biguint const maxu    = 0xFFFFFFFF;
   static biguint const maxu_L  = 0xFFFFFFFFFFFFFFFF;
   static biguint const maxu_LL = 0xFFFFFFFFFFFFFFFF;

   char const *str = value.c_str();

   // Parse prefix.
   int base;
   if(*str == '0')
   {
         if(*++str == 'X' || *str == 'x') ++str, base = 16;
      else if(*str == 'B' || *str == 'b') ++str, base =  2;
      else                                       base =  8;
   }
   else
      base = 10;

   // Parse integral part.
   biguint val = 0;
   for(; *str; ++str)
   {
      int i = ParseNumber(*str);

      if(0 > i || i >= base) break;

      val *= base;
      val += i;
   }

   // Parse suffix part.
   bool isUU = base != 10; // Non-decimals try to promote to unsigned.
   int sufH = 0, sufL = 0, sufU = 0;
   while(*str) switch(*str++)
   {
   case 'H': case 'h': ++sufH; break;
   case 'L': case 'l': ++sufL; break;
   case 'U': case 'u': ++sufU; break;
   default: Error_P("unexpected character in integer-constant: '%c'", *--str);
   }

   // Process suffix part.
   if(sufH > 2) Error_P("integer-constant: HHH");
   if(sufL > 2) Error_P("integer-constant: LLL");
   if(sufU > 1) Error_P("integer-constant: UU");

   if(sufH && sufL) Error_P("integer-constant: HL");

   // Create value.
   if(sufH == 2) goto dointHH;
   if(sufH == 1) goto dointH;
   if(sufL == 1) goto dointL;
   if(sufL == 2) goto dointLL;
   goto doint;

dointHH:
   if(!sufU && val <= maxs_HH)
      return create_value_schar(val, context, pos);

   if((sufU || isUU) && val <= maxu_HH)
      return create_value_uchar(val, context, pos);

dointH:
   if(!sufU && val <= maxs_H)
      return create_value_short(val, context, pos);

   if((sufU || isUU) && val <= maxu_H)
      return create_value_ushort(val, context, pos);

doint:
   if(!sufU && val <= maxs)
      return create_value_int(val, context, pos);

   if((sufU || isUU) && val <= maxu)
      return create_value_uint(val, context, pos);

dointL:
   if(!sufU && val <= maxs_L)
      return create_value_long(val, context, pos);

   if((sufU || isUU) && val <= maxu_L)
      return create_value_ulong(val, context, pos);

dointLL:
   if(!sufU && val <= maxs_LL)
      return create_value_llong(val, context, pos);

   if((sufU || isUU) && val <= maxu_LL)
      return create_value_ullong(val, context, pos);

   Error_P("integer literal out of range");
}

//
// SourceExpressionC::ParseLabel
//
SourceExpression::Pointer SourceExpressionC::ParseLabel(std::string const &value, SRCEXP_EXPR_ARGS)
{
   std::string varData = ObjectData::Label::Add(value);

   VariableType::Reference varType = VariableType::get_bt_label();

   SourceVariable::Pointer var = SourceVariable::create_literal(varType, varData, pos);

   return create_value_variable(var, context, pos);
}

//
// SourceExpressionC::ParseLabel
//
std::string SourceExpressionC::ParseLabel(SRCEXPC_PARSE_ARG1)
{
   SourceTokenC::Reference tok = in->get(SourceTokenC::TT_NAM);

   if(tok->data == "case")
   {
      SourceExpression::Pointer expr = ParseExpression(in, context);
      return context->getLabelCase(expr->makeObject()->resolveINT(), tok->pos);
   }
   else if(tok->data == "default")
      return context->getLabelCaseDefault(tok->pos);
   else
      return context->getLabelGoto(tok->data, tok->pos);
}

// EOF

