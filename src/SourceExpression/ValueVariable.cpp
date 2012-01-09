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



class SourceExpression_ValueVariable : public SourceExpression
{
   MAKE_COUNTER_CLASS_BASE(SourceExpression_ValueVariable, SourceExpression);

public:
   SourceExpression_ValueVariable(SourceVariable *var,
                                  SourcePosition const &position);

   virtual bool canGetData() const;

   virtual VariableData::Pointer getData() const;

   virtual VariableType const *getType() const;

protected:
   virtual void printDebug(std::ostream *out) const;

private:
   SourceVariable::Pointer var;
};



//
// SourceExpression::create_value_char
//
SourceExpression::Pointer SourceExpression::
create_value_char(SourceTokenC const &token)
{
   if (token.getData().size() != 1)
   {
      throw SourceException("invalid length for character literal",
                            token.getPosition(), "SourceExpression");
   }

   ObjectExpression::Pointer charVarData
      = ObjectExpression::create_value_int(token.getData()[0],
                                           token.getPosition());

   VariableType const *charVarType = VariableType::get_vt_char();

   SourceVariable::Pointer charVariable
      = SourceVariable::create_literal(charVarType, charVarData,
                                       token.getPosition());

   return create_value_variable(charVariable, token.getPosition());
}

//
// SourceExpression::create_value_int
//
SourceExpression::Pointer SourceExpression::
create_value_int(bigsint value, SourcePosition const &position)
{
   ObjectExpression::Pointer intVarData
      = ObjectExpression::create_value_int(value, position);

   VariableType const *intVarType = VariableType::get_vt_int();

   SourceVariable::Pointer intVariable
      = SourceVariable::create_literal(intVarType, intVarData, position);

   return create_value_variable(intVariable, position);
}

//
// SourceExpression::create_value_int
//
SourceExpression::Pointer SourceExpression::
create_value_int(SourceTokenC const &token)
{
   return create_value_int(get_bigsint(token), token.getPosition());
}

//
// SourceExpression::create_value_real
//
SourceExpression::Pointer SourceExpression::
create_value_real(SourceTokenC const &token)
{
   ObjectExpression::Pointer realVarData
      = ObjectExpression::create_value_float(get_bigreal(token),
                                             token.getPosition());

   VariableType const *realVarType = VariableType::get_vt_real();

   SourceVariable::Pointer realVariable
      = SourceVariable::create_literal(realVarType, realVarData,
                                       token.getPosition());

   return create_value_variable(realVariable, token.getPosition());
}

//
// SourceExpression::create_value_string
//
SourceExpression::Pointer SourceExpression::
create_value_string(SourceTokenC const &token)
{
   std::string stringVarData = ObjectData_String::add(token.getData());

   VariableType const *stringVarType = VariableType::get_vt_string();

   SourceVariable::Pointer stringVariable
      = SourceVariable::create_literal(stringVarType, stringVarData,
                                       token.getPosition());

   return create_value_variable(stringVariable, token.getPosition());
}

//
// SourceExpression::create_value_variable
//
SourceExpression::Pointer SourceExpression::
create_value_variable(SourceVariable *var, SourcePosition const &position)
{
   return new SourceExpression_ValueVariable(var, position);
}



//
// SourceExpression_ValueVariable::SourceExpression_ValueVariable
//
SourceExpression_ValueVariable::
SourceExpression_ValueVariable(SourceVariable *_var,
                               SourcePosition const &_position)
                               : Super(_position), var(_var)
{
   if (!_var->getType()->complete)
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
VariableType const *SourceExpression_ValueVariable::getType() const
{
   return var->getType();
}

//
// SourceExpression_ValueVariable::printDebug
//
void SourceExpression_ValueVariable::printDebug(std::ostream *out) const
{
   *out << "SourceExpression_ValueVariable(";
   Super::printDebug(out);
   *out << " ";
      *out << "var=(";
      print_debug(out, var);
      *out << ")";
   *out << ")";
}



// EOF

