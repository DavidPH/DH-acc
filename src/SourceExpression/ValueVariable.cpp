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

/* SourceExpression/ValueVariable.cpp
**
** Defines the SourceExpression_ValueVariable class and methods.
*/

#include "../SourceExpression.hpp"

#include "../SourceException.hpp"
#include "../SourceTokenC.hpp"



class SourceExpression_ValueVariable : public SourceExpression
{
	MAKE_COUNTER_CLASS_BASE(SourceExpression_ValueVariable, SourceExpression);

public:
	SourceExpression_ValueVariable(SourceVariable const & var, SourcePosition const & position);

	virtual bool canMakeObject() const;
	virtual bool canMakeObjectAddress() const;

	virtual bool canMakeObjectsAddress() const;

	virtual SourceVariable::VariableType const * getType() const;

	virtual ObjectExpression::Pointer makeObject() const;
	virtual ObjectExpression::Pointer makeObjectAddress() const;

	virtual void makeObjectsAddress(ObjectVector * objects) const;

	virtual void makeObjectsCall(ObjectVector * objects, std::vector<SourceExpression::Pointer> const & args, ObjectExpression * stack) const;

	virtual void makeObjectsGet(ObjectVector * objects) const;
	virtual void makeObjectsGetArray(ObjectVector * objects, std::vector<SourceExpression::Pointer> * dimensions) const;
	virtual void makeObjectsGetMember(ObjectVector * objects, std::vector<std::string> * names) const;

	virtual void makeObjectsSet(ObjectVector * objects) const;
	virtual void makeObjectsSetArray(ObjectVector * objects, std::vector<SourceExpression::Pointer> * dimensions) const;
	virtual void makeObjectsSetMember(ObjectVector * objects, std::vector<std::string> * names) const;

	virtual void printDebug(std::ostream * out) const;

private:
	SourceVariable _var;
};



SourceExpression::Pointer SourceExpression::create_value_char(SourceTokenC const & token)
{
	if (token.getData().size() != 1)
		throw SourceException("invalid length for character literal", token.getPosition(), "SourceExpressionDS");

	SourceVariable::VariableType const * charVarType(SourceVariable::get_VariableType(SourceVariable::VT_CHAR));

	SourceVariable::VariableData_Char charVarData = {charVarType, token.getData()[0]};

	SourceVariable charVariable("", charVarData, token.getPosition());

	return create_value_variable(charVariable, token.getPosition());
}
SourceExpression::Pointer SourceExpression::create_value_int(int value, SourcePosition const & position)
{
	SourceVariable::VariableType const * intVarType(SourceVariable::get_VariableType(SourceVariable::VT_INT));

	SourceVariable::VariableData_Int intVarData = {intVarType, value};

	SourceVariable intVariable("", intVarData, position);

	return create_value_variable(intVariable, position);
}
SourceExpression::Pointer SourceExpression::create_value_int(SourceTokenC const & token)
{
	SourceVariable::VariableType const * intVarType(SourceVariable::get_VariableType(SourceVariable::VT_INT));

	SourceVariable::VariableData_Int intVarData = {intVarType, ObjectExpression::get_int(token)};

	SourceVariable intVariable("", intVarData, token.getPosition());

	return create_value_variable(intVariable, token.getPosition());
}
SourceExpression::Pointer SourceExpression::create_value_real(SourceTokenC const & token)
{
	SourceVariable::VariableType const * realVarType(SourceVariable::get_VariableType(SourceVariable::VT_REAL));

	SourceVariable::VariableData_Real realVarData = {realVarType, ObjectExpression::get_float(token)};

	SourceVariable realVariable("", realVarData, token.getPosition());

	return create_value_variable(realVariable, token.getPosition());
}
SourceExpression::Pointer SourceExpression::create_value_string(SourceTokenC const & token)
{
	SourceVariable::VariableType const * stringVarType(SourceVariable::get_VariableType(SourceVariable::VT_STRING));

	SourceVariable::VariableData_String stringVarData = {stringVarType, -1};

	SourceVariable stringVariable("", stringVarData, token.getPosition(), ObjectExpression::add_string(token.getData() + '\0'));

	return create_value_variable(stringVariable, token.getPosition());
}
SourceExpression::Pointer SourceExpression::create_value_variable(SourceVariable const & var, SourcePosition const & position)
{
	return new SourceExpression_ValueVariable(var, position);
}



SourceExpression_ValueVariable::SourceExpression_ValueVariable(SourceVariable const & var, SourcePosition const & position) : Super(position), _var(var)
{

}

bool SourceExpression_ValueVariable::canMakeObject() const
{
	return _var.canMakeObject();
}
bool SourceExpression_ValueVariable::canMakeObjectAddress() const
{
	return _var.canMakeObjectAddress();
}

bool SourceExpression_ValueVariable::canMakeObjectsAddress() const
{
	return _var.canMakeObjectsAddress();
}

SourceVariable::VariableType const * SourceExpression_ValueVariable::getType() const
{
	return _var.getType();
}

ObjectExpression::Pointer SourceExpression_ValueVariable::makeObject() const
{
	return _var.makeObject(position);
}
ObjectExpression::Pointer SourceExpression_ValueVariable::makeObjectAddress() const
{
	return _var.makeObjectAddress(position);
}

void SourceExpression_ValueVariable::makeObjectsAddress(ObjectVector * objects) const
{
	_var.makeObjectsAddress(objects, position);
}

void SourceExpression_ValueVariable::makeObjectsCall(ObjectVector * objects, std::vector<SourceExpression::Pointer> const & args, ObjectExpression * stack) const
{
	_var.makeObjectsCall(objects, args, stack, position);
}

void SourceExpression_ValueVariable::makeObjectsGet(ObjectVector * objects) const
{
	_var.makeObjectsGet(objects, position);
}
void SourceExpression_ValueVariable::makeObjectsGetArray(ObjectVector * objects, std::vector<SourceExpression::Pointer> * dimensions) const
{
	_var.makeObjectsGetArray(objects, dimensions, position);
}
void SourceExpression_ValueVariable::makeObjectsGetMember(ObjectVector * objects, std::vector<std::string> * names) const
{
	_var.makeObjectsGetMember(objects, names, position);
}

void SourceExpression_ValueVariable::makeObjectsSet(ObjectVector * objects) const
{
	_var.makeObjectsSet(objects, position);
}
void SourceExpression_ValueVariable::makeObjectsSetArray(ObjectVector * objects, std::vector<SourceExpression::Pointer> * dimensions) const
{
	_var.makeObjectsSetArray(objects, dimensions, position);
}
void SourceExpression_ValueVariable::makeObjectsSetMember(ObjectVector * objects, std::vector<std::string> * names) const
{
	_var.makeObjectsSetMember(objects, names, position);
}

void SourceExpression_ValueVariable::printDebug(std::ostream * out) const
{
	*out << "SourceExpression_ValueVariable(";
	Super::printDebug(out);
	*out << " ";
		*out << "var=(";
		print_debug(out, _var);
		*out << ")";
	*out << ")";
}



