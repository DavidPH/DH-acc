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

#include "../ObjectExpression.hpp"
#include "../SourceException.hpp"
#include "../SourceTokenC.hpp"
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
// Global Functions                                                           |
//

//
// SourceExpression::create_value_char
//
SRCEXP_EXPRVAL_DEFN(c, char)
{
   ObjectExpression::Pointer charVarData =
      ObjectExpression::create_value_int(value, position);

   VariableType::Reference charVarType = VariableType::get_bt_char();

   SourceVariable::Pointer charVariable =
      SourceVariable::create_literal(charVarType, charVarData, position);

   return create_value_variable(charVariable, context, position);
}

//
// SourceExpression::create_value_char
//
SRCEXP_EXPRVAL_DEFN(s, char)
{
   if (value.size() != 1)
   {
      throw SourceException("invalid length for character literal", position,
                            "SourceExpression");
   }

   return create_value_char(value[0], context, position);
}

//
// SourceExpression::create_value_int
//
SRCEXP_EXPRVAL_DEFN(i, int)
{
   ObjectExpression::Pointer intVarData =
      ObjectExpression::create_value_int(value, position);

   VariableType::Reference intVarType = VariableType::get_bt_int();

   SourceVariable::Pointer intVariable =
      SourceVariable::create_literal(intVarType, intVarData, position);

   return create_value_variable(intVariable, context, position);
}

//
// SourceExpression::create_value_int
//
SRCEXP_EXPRVAL_DEFN(s, int)
{
   return create_value_int(get_bigsint(value, position), context, position);
}

//
// SourceExpression::create_value_real
//
SRCEXP_EXPRVAL_DEFN(r, real)
{
   ObjectExpression::Pointer realVarData =
      ObjectExpression::create_value_float(value, position);

   VariableType::Reference realVarType = VariableType::get_bt_real();

   SourceVariable::Pointer realVariable =
      SourceVariable::create_literal(realVarType, realVarData, position);

   return create_value_variable(realVariable, context, position);
}

//
// SourceExpression::create_value_real
//
SRCEXP_EXPRVAL_DEFN(s, real)
{
   return create_value_real(get_bigreal(value, position), context, position);
}

//
// SourceExpression::create_value_string
//
SRCEXP_EXPRVAL_DEFN(s, string)
{
   std::string stringVarData = ObjectData_String::add(value);

   VariableType::Reference stringVarType = VariableType::get_bt_string();

   SourceVariable::Pointer stringVariable
      = SourceVariable::create_literal(stringVarType, stringVarData, position);

   return create_value_variable(stringVariable, context, position);
}

//
// SourceExpression::create_value_uint
//
SRCEXP_EXPRVAL_DEFN(i, uint)
{
   ObjectExpression::Pointer uintVarData =
      ObjectExpression::create_value_int(value, position);

   VariableType::Reference uintVarType = VariableType::get_bt_uint();

   SourceVariable::Pointer uintVariable =
      SourceVariable::create_literal(uintVarType, uintVarData, position);

   return create_value_variable(uintVariable, context, position);
}

//
// SourceExpression::create_value_variable
//
SRCEXP_EXPRVAL_DEFN(v, variable)
{
   return new SourceExpression_ValueVariable(var, context, position);
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
      throw SourceException("incomplete type", position, getName());
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

