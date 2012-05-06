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

private:
   SourceVariable::Pointer var;
};


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

static SourceExpression::Pointer create_int
(bigsint data, VariableType *varType, SRCEXP_EXPR_ARGS)
{
   ObjectExpression::Pointer varData =
      ObjectExpression::create_value_int(data, pos);

   SourceVariable::Pointer var =
      SourceVariable::create_literal(varType, varData, pos);

   return SourceExpression::create_value_variable(var, context, pos);
}


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_value_char
//
SRCEXP_EXPRVAL_DEFN(c, char)
{
   ObjectExpression::Pointer charVarData =
      ObjectExpression::create_value_int(value, pos);

   VariableType::Reference charVarType = VariableType::get_bt_char();

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
      ERROR_P("invalid length for character literal");

   return create_value_char(value[0], context, pos);
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
   return create_int(value, VariableType::get_bt_llong(), context, pos);
}

//
// SourceExpression::create_value_long
//
SRCEXP_EXPRVAL_DEFN(i, long)
{
   return create_int(value, VariableType::get_bt_long(), context, pos);
}

//
// SourceExpression::create_value_real
//
SRCEXP_EXPRVAL_DEFN(r, real)
{
   ObjectExpression::Pointer realVarData =
      ObjectExpression::create_value_float(value, pos);

   VariableType::Reference realVarType = VariableType::get_bt_real();

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
   return create_int(value, VariableType::get_bt_schar(), context, pos);
}

//
// SourceExpression::create_value_short
//
SRCEXP_EXPRVAL_DEFN(i, short)
{
   return create_int(value, VariableType::get_bt_short(), context, pos);
}

//
// SourceExpression::create_value_stackptr
//
SRCEXP_EXPRVAL_DEFN(, stackptr)
{
   ObjectExpression::Pointer stackptrVarData = ObjectExpression::
      create_value_int(option_addr_stack, pos);

   VariableType::Reference stackptrVarType = VariableType::get_bt_uint();

   SourceVariable::Pointer stackptrVar = SourceVariable::create_variable
      (stackptrVarType, stackptrVarData, STORE_WORLDREGISTER, pos);

   return create_value_variable(stackptrVar, context, pos);
}

//
// SourceExpression::create_value_string
//
SRCEXP_EXPRVAL_DEFN(s, string)
{
   std::string stringVarData = ObjectData_String::add(value);

   VariableType::Reference stringVarType = VariableType::get_bt_string();

   SourceVariable::Pointer stringVariable
      = SourceVariable::create_literal(stringVarType, stringVarData, pos);

   return create_value_variable(stringVariable, context, pos);
}

//
// SourceExpression::create_value_uchar
//
SRCEXP_EXPRVAL_DEFN(i, uchar)
{
   return create_int(value, VariableType::get_bt_uchar(), context, pos);
}

//
// SourceExpression::create_value_uint
//
SRCEXP_EXPRVAL_DEFN(i, uint)
{
   return create_int(value, VariableType::get_bt_uint(), context, pos);
}

//
// SourceExpression::create_value_ullong
//
SRCEXP_EXPRVAL_DEFN(i, ullong)
{
   return create_int(value, VariableType::get_bt_ullong(), context, pos);
}

//
// SourceExpression::create_value_ulong
//
SRCEXP_EXPRVAL_DEFN(i, ulong)
{
   return create_int(value, VariableType::get_bt_ulong(), context, pos);
}

//
// SourceExpression::create_value_ushort
//
SRCEXP_EXPRVAL_DEFN(i, ushort)
{
   return create_int(value, VariableType::get_bt_ushort(), context, pos);
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
      ERROR_NP("incomplete type");
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

