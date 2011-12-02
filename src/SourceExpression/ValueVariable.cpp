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
#include "../SourceException.hpp"
#include "../SourceTokenC.hpp"
#include "../VariableType.hpp"



class SourceExpression_ValueVariable : public SourceExpression
{
	MAKE_COUNTER_CLASS_BASE(SourceExpression_ValueVariable, SourceExpression);

public:
	SourceExpression_ValueVariable(SourceVariable * var, SourcePosition const & position);

	virtual bool canMakeObject() const;
	virtual bool canMakeObjectAddress() const;

	virtual bool canMakeObjectsAddress() const;

	virtual VariableType const * getType() const;

	virtual CounterPointer<ObjectExpression> makeObject() const;
	virtual CounterPointer<ObjectExpression> makeObjectAddress() const;

protected:
	virtual void printDebug(std::ostream * out) const;

private:
	virtual void virtual_makeObjects(ObjectVector * objects);

	virtual void virtual_makeObjectsAccess(ObjectVector * objects);
	virtual void virtual_makeObjectsAccessArray(ObjectVector * objects, std::vector<SourceExpression::Pointer> * dimensions);
	virtual void virtual_makeObjectsAccessMember(ObjectVector * objects, std::vector<std::string> * names);

	virtual void virtual_makeObjectsAddress(ObjectVector * objects);

	virtual void virtual_makeObjectsGet(ObjectVector * objects);
	virtual void virtual_makeObjectsGetArray(ObjectVector * objects, std::vector<SourceExpression::Pointer> * dimensions);
	virtual void virtual_makeObjectsGetMember(ObjectVector * objects, std::vector<std::string> * names);

	virtual void virtual_makeObjectsSet(ObjectVector * objects);
	virtual void virtual_makeObjectsSetArray(ObjectVector * objects, std::vector<SourceExpression::Pointer> * dimensions);
	virtual void virtual_makeObjectsSetMember(ObjectVector * objects, std::vector<std::string> * names);

	SourceVariable::Pointer _var;
};



SourceExpression::Pointer SourceExpression::create_value_char(SourceTokenC const & token)
{
	if (token.getData().size() != 1)
		throw SourceException("invalid length for character literal", token.getPosition(), "SourceExpression");

	ObjectExpression::Pointer charVarData(ObjectExpression::create_value_int(token.getData()[0], token.getPosition()));

	VariableType const * charVarType(VariableType::get_vt_char());

	SourceVariable::Pointer charVariable(SourceVariable::create_literal(charVarType, charVarData, token.getPosition()));

	return create_value_variable(charVariable, token.getPosition());
}
SourceExpression::Pointer SourceExpression::create_value_int(bigsint value, SourcePosition const & position)
{
	ObjectExpression::Pointer intVarData(ObjectExpression::create_value_int(value, position));

	VariableType const * intVarType(VariableType::get_vt_int());

	SourceVariable::Pointer intVariable(SourceVariable::create_literal(intVarType, intVarData, position));

	return create_value_variable(intVariable, position);
}
SourceExpression::Pointer SourceExpression::create_value_int(SourceTokenC const & token)
{
	return create_value_int(get_bigsint(token), token.getPosition());
}
SourceExpression::Pointer SourceExpression::create_value_real(SourceTokenC const & token)
{
	ObjectExpression::Pointer realVarData(ObjectExpression::create_value_float(get_bigreal(token), token.getPosition()));

	VariableType const * realVarType(VariableType::get_vt_real());

	SourceVariable::Pointer realVariable(SourceVariable::create_literal(realVarType, realVarData, token.getPosition()));

	return create_value_variable(realVariable, token.getPosition());
}
SourceExpression::Pointer SourceExpression::create_value_string(SourceTokenC const & token)
{
	std::string stringVarData(ObjectExpression::add_string(token.getData() + '\0'));

	VariableType const * stringVarType(VariableType::get_vt_string());

	SourceVariable::Pointer stringVariable(SourceVariable::create_literal(stringVarType, stringVarData, token.getPosition()));

	return create_value_variable(stringVariable, token.getPosition());
}
SourceExpression::Pointer SourceExpression::create_value_variable(SourceVariable * var, SourcePosition const & position)
{
	return new SourceExpression_ValueVariable(var, position);
}



SourceExpression_ValueVariable::SourceExpression_ValueVariable(SourceVariable * var, SourcePosition const & position_) : Super(position_), _var(var)
{
	if (!_var->getType()->complete)
		throw SourceException("incomplete type", position, getName());
}

bool SourceExpression_ValueVariable::canMakeObject() const
{
	return _var->canMakeObject();
}
bool SourceExpression_ValueVariable::canMakeObjectAddress() const
{
	return _var->canMakeObjectAddress();
}

bool SourceExpression_ValueVariable::canMakeObjectsAddress() const
{
	return _var->canMakeObjectsAddress();
}

VariableType const * SourceExpression_ValueVariable::getType() const
{
	return _var->getType();
}

CounterPointer<ObjectExpression> SourceExpression_ValueVariable::makeObject() const
{
	return _var->makeObject(position);
}
CounterPointer<ObjectExpression> SourceExpression_ValueVariable::makeObjectAddress() const
{
	return _var->makeObjectAddress(position);
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

void SourceExpression_ValueVariable::virtual_makeObjects(ObjectVector * objects)
{
	Super::recurse_makeObjects(objects);

	//_var->makeObjects(objects, position);
}

void SourceExpression_ValueVariable::virtual_makeObjectsAccess(ObjectVector * objects)
{
	Super::recurse_makeObjectsAccess(objects);

	_var->makeObjectsAccess(objects, position);
}
void SourceExpression_ValueVariable::virtual_makeObjectsAccessArray(ObjectVector * objects, std::vector<SourceExpression::Pointer> * dimensions)
{
	Super::recurse_makeObjectsAccessArray(objects, dimensions);

	_var->makeObjectsAccessArray(objects, dimensions, position);
}
void SourceExpression_ValueVariable::virtual_makeObjectsAccessMember(ObjectVector * objects, std::vector<std::string> * names)
{
	Super::recurse_makeObjectsAccessMember(objects, names);

	_var->makeObjectsAccessMember(objects, names, position);
}

void SourceExpression_ValueVariable::virtual_makeObjectsAddress(ObjectVector * objects)
{
	Super::recurse_makeObjectsAddress(objects);

	_var->makeObjectsAddress(objects, position);
}

void SourceExpression_ValueVariable::virtual_makeObjectsGet(ObjectVector * objects)
{
	Super::recurse_makeObjectsGet(objects);

	_var->makeObjectsGet(objects, position);
}
void SourceExpression_ValueVariable::virtual_makeObjectsGetArray(ObjectVector * objects, std::vector<SourceExpression::Pointer> * dimensions)
{
	Super::recurse_makeObjectsGetArray(objects, dimensions);

	_var->makeObjectsGetArray(objects, dimensions, position);
}
void SourceExpression_ValueVariable::virtual_makeObjectsGetMember(ObjectVector * objects, std::vector<std::string> * names)
{
	Super::recurse_makeObjectsGetMember(objects, names);

	_var->makeObjectsGetMember(objects, names, position);
}

void SourceExpression_ValueVariable::virtual_makeObjectsSet(ObjectVector * objects)
{
	Super::recurse_makeObjectsSet(objects);

	_var->makeObjectsSet(objects, position);
}
void SourceExpression_ValueVariable::virtual_makeObjectsSetArray(ObjectVector * objects, std::vector<SourceExpression::Pointer> * dimensions)
{
	Super::recurse_makeObjectsSetArray(objects, dimensions);

	_var->makeObjectsSetArray(objects, dimensions, position);
}
void SourceExpression_ValueVariable::virtual_makeObjectsSetMember(ObjectVector * objects, std::vector<std::string> * names)
{
	Super::recurse_makeObjectsSetMember(objects, names);

	_var->makeObjectsSetMember(objects, names, position);
}


