//-----------------------------------------------------------------------------
//
// Copyright(C) 2011-2013 David Hill
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
// SourceExpression handling of variables.
//
//-----------------------------------------------------------------------------

#include "../SourceExpression.hpp"

#include "../ObjectData.hpp"
#include "../ObjectExpression.hpp"
#include "../SourceException.hpp"
#include "../SourceTokenC.hpp"
#include "../SourceVariable.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Macros                                                                     |
//

#define CreateValueX(T, BT, ET) \
   CreateValueXPart(T, BT##_hh, ET##_HH, BT##_hh) \
   CreateValueXPart(T, BT##_h,  ET##_H,  BT##_h) \
   CreateValueXPart(T, BT,      ET,      BT) \
   CreateValueXPart(T, BT##_l,  ET##_L,  BT##_l) \
   CreateValueXPart(T, BT##_ll, ET##_LL, BT##_ll)

#define CreateValueXN(T, BT, ET, N1, N2, N3, N4, N5) \
   CreateValueXPart(T, BT##_hh, ET##_HH, N1) \
   CreateValueXPart(T, BT##_h,  ET##_H,  N2) \
   CreateValueXPart(T, BT,      ET,      N3) \
   CreateValueXPart(T, BT##_l,  ET##_L,  N4) \
   CreateValueXPart(T, BT##_ll, ET##_LL, N5)

#define CreateValueXPart(T, BT, ET, N) \
   SRCEXP_EXPRVAL_DEFN(T, N) \
   { \
      auto varType = VariableType::get_bt_##BT(); \
      auto varData = ObjectExpression::CreateValue##ET(value, pos); \
      auto var = SourceVariable::create_literal(varType, varData, pos); \
      \
      return SourceExpression::create_value_variable(var, context, pos); \
   }


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// SourceExpression_ValueVariable
//
class SourceExpression_ValueVariable : public SourceExpression
{
   CounterPreambleNoClone(SourceExpression_ValueVariable, SourceExpression);

public:
   //
   // SourceExpression_ValueVariable
   //
   SourceExpression_ValueVariable(SourceVariable *_var, SRCEXP_EXPR_PARM)
    : Super(SRCEXP_EXPR_PASS), var(_var)
   {
      if (!_var->getType()->getComplete())
         Error_NP("incomplete type");
   }

   virtual bool canGetData() const {return true;}

   virtual VariableData::Pointer getData() const {return var->getData();}

   virtual VariableType::Reference getType() const {return var->getType();}

   //
   // isSideEffect
   //
   virtual bool isSideEffect() const
   {
      // Accessing a volatile variable is a side-effect.
      return var->getType()->getQualifier(VariableType::QUAL_VOLATILE);
   }

private:
   SourceVariable::Pointer var;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_value_acc*
//
CreateValueX(r, acc, FIX)

//
// SourceExpression::create_value_ang*
//
CreateValueX(r, ang, FIX)

//
// SourceExpression::create_value_char
//
SRCEXP_EXPRVAL_DEFN(c, char)
{
   auto varData = ObjectExpression::CreateValueINT(value, pos);
   auto varType = VariableType::get_bt_chr();
   auto var = SourceVariable::create_literal(varType, varData, pos);

   return create_value_variable(var, context, pos);
}

//
// SourceExpression::create_value_char
//
SRCEXP_EXPRVAL_DEFN(s, char)
{
   if (value.size() != 1)
      Error_P("invalid length for character literal");

   return create_value_char(value[0], context, pos);
}

//
// SourceExpression::create_value_fix*
//
CreateValueX(r, fix, FIX)

//
// SourceExpression::create_value_flt*
//
CreateValueX(r, flt, FLT)

//
// SourceExpression::create_value_fra*
//
CreateValueX(r, fra, FIX)

//
// SourceExpression::create_value_int*
//
CreateValueXN(i, int, INT, schar, short, int, long, llong)

//
// SourceExpression::create_value_real
//
SRCEXP_EXPRVAL_DEFN(r, real)
{
   return create_value_fix(value, context, pos);
}

//
// SourceExpression::create_value_real
//
SRCEXP_EXPRVAL_DEFN(s, real)
{
   return create_value_real(get_bigreal(value, pos), context, pos);
}

//
// SourceExpression::create_value_string
//
SRCEXP_EXPRVAL_DEFN(s, string)
{
   auto varData = ObjectData::String::Add(value);
   auto varType = VariableType::get_bt_str();
   auto var = SourceVariable::create_literal(varType, varData, pos);

   return create_value_variable(var, context, pos);
}

//
// SourceExpression::create_value_uns*
//
CreateValueXN(i, uns, UNS, uchar, ushort, uint, ulong, ullong)

//
// SourceExpression::create_value_variable
//
SRCEXP_EXPRVAL_DEFN(v, variable)
{
   return new SourceExpression_ValueVariable(var, context, pos);
}

// EOF

