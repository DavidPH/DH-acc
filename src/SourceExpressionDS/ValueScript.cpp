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

/* SourceExpressionDS/ValueScript.cpp
**
** Defines the SourceExpressionDS_ValueScript class and methods.
*/

#include "Base.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectToken.hpp"
#include "../print_debug.hpp"
#include "../SourceException.hpp"



class SourceExpressionDS_ValueScript : public SourceExpressionDS_Base
{
public:
	SourceExpressionDS_ValueScript(int32_t number, SourceVariable::VariableType const * type, SourcePosition const & position);

	virtual SourceExpressionDS_ValueScript * clone() const;

	virtual char const * getName() const;

	virtual SourceVariable::VariableType const * getType() const;

	virtual bool isConstant() const;

	virtual ObjectExpression makeObject() const;
	virtual void makeObjectsCall(std::vector<ObjectToken> * const objects, std::vector<SourceExpressionDS> const & args) const;
	virtual void makeObjectsGet(std::vector<ObjectToken> * const objects) const;

	virtual void printDebug(std::ostream * const out) const;

private:
	int32_t _number;
	SourceVariable::VariableType const * _type;
};



SourceExpressionDS SourceExpressionDS::make_expression_value_script(int32_t number, SourceVariable::VariableType const * type, SourcePosition const & position)
{
	return new SourceExpressionDS_ValueScript(number, type, position);
}



SourceExpressionDS_ValueScript::SourceExpressionDS_ValueScript(int32_t number, SourceVariable::VariableType const * type, SourcePosition const & position) : SourceExpressionDS_Base(position), _number(number), _type(type)
{

}

SourceExpressionDS_ValueScript * SourceExpressionDS_ValueScript::clone() const
{
	return new SourceExpressionDS_ValueScript(*this);
}

char const * SourceExpressionDS_ValueScript::getName() const
{
	return "SourceExpressionDS_ValueScript";
}

SourceVariable::VariableType const * SourceExpressionDS_ValueScript::getType() const
{
	return _type;
}

bool SourceExpressionDS_ValueScript::isConstant() const
{
	return true;
}

ObjectExpression SourceExpressionDS_ValueScript::makeObject() const
{
	return ObjectExpression::create_value_int32(_number, getPosition());
}
void SourceExpressionDS_ValueScript::makeObjectsCall(std::vector<ObjectToken> * const objects, std::vector<SourceExpressionDS> const & args) const
{
	makeObjectsGet(objects);
	SourceExpressionDS::make_objects_call_script(objects, _type, args, getPosition());
}
void SourceExpressionDS_ValueScript::makeObjectsGet(std::vector<ObjectToken> * const objects) const
{
	objects->push_back(ObjectToken(ObjectToken::OCODE_PUSHNUMBER, getPosition(), ObjectExpression::create_value_int32(_number, getPosition())));
}

void SourceExpressionDS_ValueScript::printDebug(std::ostream * const out) const
{
	*out << "SourceExpressionDS_ValueScript(";
	SourceExpressionDS_Base::printDebug(out);
	*out << " ";
		*out << "number=(";
		*out << _number;
		*out << ")";

		*out << ", ";

		*out << "type=(";
		print_debug(out, _type);
		*out << ")";
	*out << ")";
}



