/* Copyright (C) 2011 David Hill
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/* SourceExpressionDS/ValueVariable.cpp
**
** Defines the SourceExpressionDS_ValueVariable class and methods.
*/

#include "Base.hpp"

#include "../SourceException.hpp"
#include "../SourceTokenC.hpp"



class SourceExpressionDS_ValueVariable : public SourceExpressionDS_Base
{
public:
	SourceExpressionDS_ValueVariable(SourceVariable const & var, SourcePosition const & position);

	virtual SourceExpressionDS_ValueVariable * clone() const;

	virtual char const * getName() const;

	virtual SourceVariable::VariableType const * getType() const;

	virtual bool isConstant() const;

	virtual ObjectExpression makeObject() const;
	virtual void makeObjectsCall(ObjectVector * objects, std::vector<SourceExpressionDS> const & args) const;
	virtual void makeObjectsGet(ObjectVector * objects) const;
	virtual void makeObjectsGet(ObjectVector * objects, std::vector<std::string> * names) const;
	virtual void makeObjectsSet(ObjectVector * objects) const;
	virtual void makeObjectsSet(ObjectVector * objects, std::vector<std::string> * names) const;

	virtual void printDebug(std::ostream * const out) const;

private:
	SourceVariable _var;
};



SourceExpressionDS SourceExpressionDS::make_expression_value_char(SourceTokenC const & token)
{
	if (token.getData().size() != 1)
		throw SourceException("invalid length for character literal", token.getPosition(), "SourceExpressionDS");

	SourceVariable::VariableType const * charVarType(SourceVariable::get_VariableType(SourceVariable::VT_CHAR));

	SourceVariable::VariableData_Char charVarData = {charVarType, token.getData()[0]};

	SourceVariable charVariable("", charVarData, token.getPosition());

	return make_expression_value_variable(charVariable, token.getPosition());
}
SourceExpressionDS SourceExpressionDS::make_expression_value_int(SourceTokenC const & token)
{
	SourceVariable::VariableType const * intVarType(SourceVariable::get_VariableType(SourceVariable::VT_INT));

	SourceVariable::VariableData_Int intVarData = {intVarType, ObjectExpression::get_int(token)};

	SourceVariable intVariable("", intVarData, token.getPosition());

	return make_expression_value_variable(intVariable, token.getPosition());
}
SourceExpressionDS SourceExpressionDS::make_expression_value_real(SourceTokenC const & token)
{
	SourceVariable::VariableType const * realVarType(SourceVariable::get_VariableType(SourceVariable::VT_REAL));

	SourceVariable::VariableData_Real realVarData = {realVarType, ObjectExpression::get_float(token)};

	SourceVariable realVariable("", realVarData, token.getPosition());

	return make_expression_value_variable(realVariable, token.getPosition());
}
SourceExpressionDS SourceExpressionDS::make_expression_value_string(SourceTokenC const & token)
{
	SourceVariable::VariableType const * stringVarType(SourceVariable::get_VariableType(SourceVariable::VT_STRING));

	SourceVariable::VariableData_String stringVarData = {stringVarType, -1};

	SourceVariable stringVariable("", stringVarData, token.getPosition(), ObjectExpression::add_string(token.getData() + '\0'));

	return make_expression_value_variable(stringVariable, token.getPosition());
}
SourceExpressionDS SourceExpressionDS::make_expression_value_variable(SourceVariable const & var, SourcePosition const & position)
{
	return new SourceExpressionDS_ValueVariable(var, position);
}



SourceExpressionDS_ValueVariable::SourceExpressionDS_ValueVariable(SourceVariable const & var, SourcePosition const & position) : SourceExpressionDS_Base(position), _var(var)
{

}

SourceExpressionDS_ValueVariable * SourceExpressionDS_ValueVariable::clone() const
{
	return new SourceExpressionDS_ValueVariable(*this);
}

char const * SourceExpressionDS_ValueVariable::getName() const
{
	return "SourceExpressionDS_ValueVariable";
}

SourceVariable::VariableType const * SourceExpressionDS_ValueVariable::getType() const
{
	return _var.getType();
}

bool SourceExpressionDS_ValueVariable::isConstant() const
{
	return _var.isConstant();
}

ObjectExpression SourceExpressionDS_ValueVariable::makeObject() const
{
	return _var.makeObject(getPosition());
}
void SourceExpressionDS_ValueVariable::makeObjectsCall(ObjectVector * objects, std::vector<SourceExpressionDS> const & args) const
{
	_var.makeObjectsCall(objects, args, getPosition());
}
void SourceExpressionDS_ValueVariable::makeObjectsGet(ObjectVector * objects) const
{
	_var.makeObjectsGet(objects, getPosition());
}
void SourceExpressionDS_ValueVariable::makeObjectsGet(ObjectVector * objects, std::vector<std::string> * names) const
{
	_var.makeObjectsGet(objects, names, getPosition());
}
void SourceExpressionDS_ValueVariable::makeObjectsSet(ObjectVector * objects) const
{
	_var.makeObjectsSet(objects, getPosition());
}
void SourceExpressionDS_ValueVariable::makeObjectsSet(ObjectVector * objects, std::vector<std::string> * names) const
{
	_var.makeObjectsSet(objects, names, getPosition());
}

void SourceExpressionDS_ValueVariable::printDebug(std::ostream * const out) const
{
	*out << "SourceExpressionDS_ValueVariable(";
	SourceExpressionDS_Base::printDebug(out);
	*out << " ";
		*out << "var=(";
		print_debug(out, _var);
		*out << ")";
	*out << ")";
}



