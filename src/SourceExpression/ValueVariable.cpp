//-----------------------------------------------------------------------------
//
// Copyright(C) 2011, 2012 David Hill
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
// Types                                                                      |
//

//
// SourceExpression_ValueVariable
//
class SourceExpression_ValueVariable : public SourceExpression
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(SourceExpression_ValueVariable,
                                   SourceExpression);

public:
   SourceExpression_ValueVariable(SourceVariable *var, SRCEXP_EXPR_ARGS);

   virtual bool canGetData() const;

   virtual VariableData::Pointer getData() const;

   virtual VariableType::Reference getType() const;

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
// Static Functions                                                           |
//

static SourceExpression::Pointer CreateFIX(bigreal data, VariableType *type, SRCEXP_EXPR_ARGS)
{
   ObjectExpression::Pointer varData = ObjectExpression::create_value_fix(data, pos);

   SourceVariable::Pointer var = SourceVariable::create_literal(type, varData, pos);

   return SourceExpression::create_value_variable(var, context, pos);
}

static SourceExpression::Pointer CreateFLT(bigreal data, VariableType *type, SRCEXP_EXPR_ARGS)
{
   ObjectExpression::Pointer varData = ObjectExpression::create_value_flt(data, pos);

   SourceVariable::Pointer var = SourceVariable::create_literal(type, varData, pos);

   return SourceExpression::create_value_variable(var, context, pos);
}

static SourceExpression::Pointer create_int
(bigsint data, VariableType *varType, SRCEXP_EXPR_ARGS)
{
   ObjectExpression::Pointer varData =
      ObjectExpression::create_value_int(data, pos);

   SourceVariable::Pointer var =
      SourceVariable::create_literal(varType, varData, pos);

   return SourceExpression::create_value_variable(var, context, pos);
}

static SourceExpression::Pointer create_uns(
   bigsint data, VariableType *varType, SRCEXP_EXPR_ARGS)
{
   ObjectExpression::Pointer varData =
      ObjectExpression::create_value_uns(data, pos);

   SourceVariable::Pointer var =
      SourceVariable::create_literal(varType, varData, pos);

   return SourceExpression::create_value_variable(var, context, pos);
}


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_value_acc_hh
//
SRCEXP_EXPRVAL_DEFN(r, acc_hh)
{
   return CreateFIX(value, VariableType::get_bt_acc_hh(), context, pos);
}

//
// SourceExpression::create_value_acc_h
//
SRCEXP_EXPRVAL_DEFN(r, acc_h)
{
   return CreateFIX(value, VariableType::get_bt_acc_h(), context, pos);
}

//
// SourceExpression::create_value_acc
//
SRCEXP_EXPRVAL_DEFN(r, acc)
{
   return CreateFIX(value, VariableType::get_bt_acc(), context, pos);
}

//
// SourceExpression::create_value_acc_l
//
SRCEXP_EXPRVAL_DEFN(r, acc_l)
{
   return CreateFIX(value, VariableType::get_bt_acc_l(), context, pos);
}

//
// SourceExpression::create_value_acc_ll
//
SRCEXP_EXPRVAL_DEFN(r, acc_ll)
{
   return CreateFIX(value, VariableType::get_bt_acc_ll(), context, pos);
}

//
// SourceExpression::create_value_ang_hh
//
SRCEXP_EXPRVAL_DEFN(r, ang_hh)
{
   return CreateFIX(value, VariableType::get_bt_ang_hh(), context, pos);
}

//
// SourceExpression::create_value_ang_h
//
SRCEXP_EXPRVAL_DEFN(r, ang_h)
{
   return CreateFIX(value, VariableType::get_bt_ang_h(), context, pos);
}

//
// SourceExpression::create_value_ang
//
SRCEXP_EXPRVAL_DEFN(r, ang)
{
   return CreateFIX(value, VariableType::get_bt_ang(), context, pos);
}

//
// SourceExpression::create_value_ang_l
//
SRCEXP_EXPRVAL_DEFN(r, ang_l)
{
   return CreateFIX(value, VariableType::get_bt_ang_l(), context, pos);
}

//
// SourceExpression::create_value_ang_ll
//
SRCEXP_EXPRVAL_DEFN(r, ang_ll)
{
   return CreateFIX(value, VariableType::get_bt_ang_ll(), context, pos);
}

//
// SourceExpression::create_value_char
//
SRCEXP_EXPRVAL_DEFN(c, char)
{
   ObjectExpression::Pointer charVarData =
      ObjectExpression::create_value_int(value, pos);

   VariableType::Reference charVarType = VariableType::get_bt_chr();

   SourceVariable::Pointer charVariable =
      SourceVariable::create_literal(charVarType, charVarData, pos);

   return create_value_variable(charVariable, context, pos);
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
// SourceExpression::create_value_fix_hh
//
SRCEXP_EXPRVAL_DEFN(r, fix_hh)
{
   return CreateFIX(value, VariableType::get_bt_fix_hh(), context, pos);
}

//
// SourceExpression::create_value_fix_h
//
SRCEXP_EXPRVAL_DEFN(r, fix_h)
{
   return CreateFIX(value, VariableType::get_bt_fix_h(), context, pos);
}

//
// SourceExpression::create_value_fix
//
SRCEXP_EXPRVAL_DEFN(r, fix)
{
   return CreateFIX(value, VariableType::get_bt_fix(), context, pos);
}

//
// SourceExpression::create_value_fix_l
//
SRCEXP_EXPRVAL_DEFN(r, fix_l)
{
   return CreateFIX(value, VariableType::get_bt_fix_l(), context, pos);
}

//
// SourceExpression::create_value_fix_ll
//
SRCEXP_EXPRVAL_DEFN(r, fix_ll)
{
   return CreateFIX(value, VariableType::get_bt_fix_ll(), context, pos);
}

//
// SourceExpression::create_value_flt_hh
//
SRCEXP_EXPRVAL_DEFN(r, flt_hh)
{
   return CreateFLT(value, VariableType::get_bt_flt_hh(), context, pos);
}

//
// SourceExpression::create_value_flt_h
//
SRCEXP_EXPRVAL_DEFN(r, flt_h)
{
   return CreateFLT(value, VariableType::get_bt_flt_h(), context, pos);
}

//
// SourceExpression::create_value_flt
//
SRCEXP_EXPRVAL_DEFN(r, flt)
{
   return CreateFLT(value, VariableType::get_bt_flt(), context, pos);
}

//
// SourceExpression::create_value_flt_l
//
SRCEXP_EXPRVAL_DEFN(r, flt_l)
{
   return CreateFLT(value, VariableType::get_bt_flt_l(), context, pos);
}

//
// SourceExpression::create_value_flt_ll
//
SRCEXP_EXPRVAL_DEFN(r, flt_ll)
{
   return CreateFLT(value, VariableType::get_bt_flt_ll(), context, pos);
}

//
// SourceExpression::create_value_fra_hh
//
SRCEXP_EXPRVAL_DEFN(r, fra_hh)
{
   return CreateFIX(value, VariableType::get_bt_fra_hh(), context, pos);
}

//
// SourceExpression::create_value_fra_h
//
SRCEXP_EXPRVAL_DEFN(r, fra_h)
{
   return CreateFIX(value, VariableType::get_bt_fra_h(), context, pos);
}

//
// SourceExpression::create_value_fra
//
SRCEXP_EXPRVAL_DEFN(r, fra)
{
   return CreateFIX(value, VariableType::get_bt_fra(), context, pos);
}

//
// SourceExpression::create_value_fra_l
//
SRCEXP_EXPRVAL_DEFN(r, fra_l)
{
   return CreateFIX(value, VariableType::get_bt_fra_l(), context, pos);
}

//
// SourceExpression::create_value_fra_ll
//
SRCEXP_EXPRVAL_DEFN(r, fra_ll)
{
   return CreateFIX(value, VariableType::get_bt_fra_ll(), context, pos);
}

//
// SourceExpression::create_value_int
//
SRCEXP_EXPRVAL_DEFN(i, int)
{
   return create_int(value, VariableType::get_bt_int(), context, pos);
}

//
// SourceExpression::create_value_llong
//
SRCEXP_EXPRVAL_DEFN(i, llong)
{
   return create_int(value, VariableType::get_bt_int_ll(), context, pos);
}

//
// SourceExpression::create_value_long
//
SRCEXP_EXPRVAL_DEFN(i, long)
{
   return create_int(value, VariableType::get_bt_int_l(), context, pos);
}

//
// SourceExpression::create_value_real
//
SRCEXP_EXPRVAL_DEFN(r, real)
{
   ObjectExpression::Pointer realVarData =
      ObjectExpression::create_value_fix(value, pos);

   VariableType::Reference realVarType = VariableType::get_bt_fix();

   SourceVariable::Pointer realVariable =
      SourceVariable::create_literal(realVarType, realVarData, pos);

   return create_value_variable(realVariable, context, pos);
}

//
// SourceExpression::create_value_real
//
SRCEXP_EXPRVAL_DEFN(s, real)
{
   return create_value_real(get_bigreal(value, pos), context, pos);
}

//
// SourceExpression::create_value_schar
//
SRCEXP_EXPRVAL_DEFN(i, schar)
{
   return create_int(value, VariableType::get_bt_int_hh(), context, pos);
}

//
// SourceExpression::create_value_short
//
SRCEXP_EXPRVAL_DEFN(i, short)
{
   return create_int(value, VariableType::get_bt_int_h(), context, pos);
}

//
// SourceExpression::create_value_string
//
SRCEXP_EXPRVAL_DEFN(s, string)
{
   std::string stringVarData = ObjectData::String::Add(value);

   VariableType::Reference stringVarType = VariableType::get_bt_str();

   SourceVariable::Pointer stringVariable
      = SourceVariable::create_literal(stringVarType, stringVarData, pos);

   return create_value_variable(stringVariable, context, pos);
}

//
// SourceExpression::create_value_uchar
//
SRCEXP_EXPRVAL_DEFN(i, uchar)
{
   return create_uns(value, VariableType::get_bt_uns_hh(), context, pos);
}

//
// SourceExpression::create_value_uint
//
SRCEXP_EXPRVAL_DEFN(i, uint)
{
   return create_uns(value, VariableType::get_bt_uns(), context, pos);
}

//
// SourceExpression::create_value_ullong
//
SRCEXP_EXPRVAL_DEFN(i, ullong)
{
   return create_uns(value, VariableType::get_bt_uns_ll(), context, pos);
}

//
// SourceExpression::create_value_ulong
//
SRCEXP_EXPRVAL_DEFN(i, ulong)
{
   return create_uns(value, VariableType::get_bt_uns_l(), context, pos);
}

//
// SourceExpression::create_value_ushort
//
SRCEXP_EXPRVAL_DEFN(i, ushort)
{
   return create_uns(value, VariableType::get_bt_uns_h(), context, pos);
}

//
// SourceExpression::create_value_variable
//
SRCEXP_EXPRVAL_DEFN(v, variable)
{
   return new SourceExpression_ValueVariable(var, context, pos);
}

//
// SourceExpression_ValueVariable::SourceExpression_ValueVariable
//
SourceExpression_ValueVariable::
SourceExpression_ValueVariable(SourceVariable *_var, SRCEXP_EXPR_PARM)
                               : Super(SRCEXP_EXPR_PASS),
                                 var(_var)
{
   if (!_var->getType()->getComplete())
      Error_NP("incomplete type");
}

//
// SourceExpression_ValueVariable::canGetData
//
bool SourceExpression_ValueVariable::canGetData() const
{
   return true;
}

//
// SourceExpression_ValueVariable::getData
//
VariableData::Pointer SourceExpression_ValueVariable::getData() const
{
   return var->getData();
}

//
// SourceExpression_ValueVariable::getType
//
VariableType::Reference SourceExpression_ValueVariable::getType() const
{
   return var->getType();
}

// EOF

