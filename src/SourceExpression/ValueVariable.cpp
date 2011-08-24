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

#include "../ObjectExpression.hpp"
#include "../SourceContext.hpp"
#include "../SourceException.hpp"
#include "../SourceTokenC.hpp"
#include "../VariableType.hpp"



class SourceExpression_ValueVariable : public SourceExpression
{
	MAKE_COUNTER_CLASS_BASE(SourceExpression_ValueVariable, SourceExpression);

public:
	SourceExpression_ValueVariable(SourceVariable const & var, SourcePosition const & position);

	virtual bool canMakeObject() const;
	virtual bool canMakeObjectAddress() const;

	virtual bool canMakeObjectsAddress() const;

	virtual VariableType const * getType() const;

	virtual CounterPointer<ObjectExpression> makeObject() const;
	virtual CounterPointer<ObjectExpression> makeObjectAddress() const;

	virtual void makeObjectsAddress(ObjectVector * objects);

	virtual void makeObjectsGet(ObjectVector * objects);
	virtual void makeObjectsGetArray(ObjectVector * objects, std::vector<SourceExpression::Pointer> * dimensions);
	virtual void makeObjectsGetMember(ObjectVector * objects, std::vector<std::string> * names);

	virtual void makeObjectsSet(ObjectVector * objects);
	virtual void makeObjectsSetArray(ObjectVector * objects, std::vector<SourceExpression::Pointer> * dimensions);
	virtual void makeObjectsSetMember(ObjectVector * objects, std::vector<std::string> * names);

protected:
	virtual void printDebug(std::ostream * out) const;

private:
	SourceVariable _var;
};



SourceExpression::Pointer SourceExpression::create_value_char(SourceTokenC const & token)
{
	if (token.getData().size() != 1)
		throw SourceException("invalid length for character literal", token.getPosition(), "SourceExpressionDS");

	VariableType const * charVarType(SourceContext::global_context.getVariableType(VariableType::VT_CHAR));

	SourceVariable charVariable("", charVarType, ObjectExpression::create_value_int(token.getData()[0], token.getPosition()), token.getPosition());

	return create_value_variable(charVariable, token.getPosition());
}
SourceExpression::Pointer SourceExpression::create_value_int(bigsint value, SourcePosition const & position)
{
	VariableType const * intVarType(SourceContext::global_context.getVariableType(VariableType::VT_INT));

	SourceVariable intVariable("", intVarType, ObjectExpression::create_value_int(value, position), position);

	return create_value_variable(intVariable, position);
}
SourceExpression::Pointer SourceExpression::create_value_int(SourceTokenC const & token)
{
	return create_value_int(get_bigsint(token), token.getPosition());
}
SourceExpression::Pointer SourceExpression::create_value_real(SourceTokenC const & token)
{
	VariableType const * realVarType(SourceContext::global_context.getVariableType(VariableType::VT_REAL));

	SourceVariable realVariable("", realVarType, ObjectExpression::create_value_float(get_bigreal(token), token.getPosition()), token.getPosition());

	return create_value_variable(realVariable, token.getPosition());
}
SourceExpression::Pointer SourceExpression::create_value_string(SourceTokenC const & token)
{
	VariableType const * stringVarType(SourceContext::global_context.getVariableType(VariableType::VT_STRING));

	SourceVariable stringVariable("", stringVarType, ObjectExpression::add_string(token.getData() + '\0'), token.getPosition());

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

VariableType const * SourceExpression_ValueVariable::getType() const
{
	return _var.getType();
}

CounterPointer<ObjectExpression> SourceExpression_ValueVariable::makeObject() const
{
	return _var.makeObject(position);
}
CounterPointer<ObjectExpression> SourceExpression_ValueVariable::makeObjectAddress() const
{
	return _var.makeObjectAddress(position);
}

void SourceExpression_ValueVariable::makeObjectsAddress(ObjectVector * objects)
{
	Super::recurse_makeObjectsAddress(objects);

	_var.makeObjectsAddress(objects, position);
}

void SourceExpression_ValueVariable::makeObjectsGet(ObjectVector * objects)
{
	Super::recurse_makeObjectsGet(objects);

	_var.makeObjectsGet(objects, position);
}
void SourceExpression_ValueVariable::makeObjectsGetArray(ObjectVector * objects, std::vector<SourceExpression::Pointer> * dimensions)
{
	Super::recurse_makeObjectsGetArray(objects, dimensions);

	_var.makeObjectsGetArray(objects, dimensions, position);
}
void SourceExpression_ValueVariable::makeObjectsGetMember(ObjectVector * objects, std::vector<std::string> * names)
{
	Super::recurse_makeObjectsGetMember(objects, names);

	_var.makeObjectsGetMember(objects, names, position);
}

void SourceExpression_ValueVariable::makeObjectsSet(ObjectVector * objects)
{
	Super::recurse_makeObjectsSet(objects);

	_var.makeObjectsSet(objects, position);
}
void SourceExpression_ValueVariable::makeObjectsSetArray(ObjectVector * objects, std::vector<SourceExpression::Pointer> * dimensions)
{
	Super::recurse_makeObjectsSetArray(objects, dimensions);

	_var.makeObjectsSetArray(objects, dimensions, position);
}
void SourceExpression_ValueVariable::makeObjectsSetMember(ObjectVector * objects, std::vector<std::string> * names)
{
	Super::recurse_makeObjectsSetMember(objects, names);

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



