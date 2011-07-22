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

/* SourceExpressionDS/CastStruct.cpp
**
** Defines the SourceExpressionDS_CastStruct class and methods.
*/

#include "Base.hpp"

#include "../print_debug.hpp"
#include "../SourceException.hpp"



class SourceExpressionDS_CastStruct : public SourceExpressionDS_Base
{
public:
	SourceExpressionDS_CastStruct(SourceExpressionDS const & expr, SourceVariable::VariableType const * const type, SourcePosition const & position);

	virtual SourceExpressionDS_CastStruct * clone() const;

	virtual char const * getName() const;

	virtual SourceVariable::VariableType const * getType() const;

	virtual bool isConstant() const;

	virtual void makeObjectsGet(ObjectVector * objects) const;
	virtual void makeObjectsGet(ObjectVector * objects, std::vector<std::string> * names) const;

	virtual void printDebug(std::ostream * const out) const;

private:
	std::vector<SourceExpressionDS> _expressions;
	SourceVariable::VariableType const * _type;
};



SourceExpressionDS SourceExpressionDS::make_expression_cast_struct(SourceExpressionDS const & expr, SourceVariable::VariableType const * const type, SourcePosition const & position)
{
	return new SourceExpressionDS_CastStruct(expr, type, position);
}



SourceExpressionDS_CastStruct::SourceExpressionDS_CastStruct(SourceExpressionDS const & expr, SourceVariable::VariableType const * const type, SourcePosition const & position) : SourceExpressionDS_Base(position), _expressions(expr.getVector()), _type(type)
{
	if (_type->type != SourceVariable::VT_STRUCT)
		throw SourceException("type not VT_STRUCT", getPosition(), getName());

	if (_expressions.size() != _type->types.size())
		throw SourceException("insufficient block elements", getPosition(), getName());

	for (size_t i(0); i < _expressions.size(); ++i)
	{
		if (_expressions[i].getType() != _type->types[i])
			_expressions[i] = SourceExpressionDS::make_expression_cast(_expressions[i], _type->types[i], getPosition());
	}
}

SourceExpressionDS_CastStruct * SourceExpressionDS_CastStruct::clone() const
{
	return new SourceExpressionDS_CastStruct(*this);
}

char const * SourceExpressionDS_CastStruct::getName() const
{
	return "SourceExpressionDS_CastStruct";
}

SourceVariable::VariableType const * SourceExpressionDS_CastStruct::getType() const
{
	return _type;
}

bool SourceExpressionDS_CastStruct::isConstant() const
{
	// TODO: The block is constant if all expressions are.
	return false;
}

void SourceExpressionDS_CastStruct::makeObjectsGet(ObjectVector * objects) const
{
	for (size_t i(0); i < _expressions.size(); ++i)
		_expressions[i].makeObjectsGet(objects);
}
void SourceExpressionDS_CastStruct::makeObjectsGet(ObjectVector * objects, std::vector<std::string> * names) const
{
	if (names->empty())
	{
		makeObjectsGet(objects);
		return;
	}

	for (size_t i(0); i < _type->names.size(); ++i)
	{
		if (_type->names[i] == names->back())
		{
			names->pop_back();
			_expressions[i].makeObjectsGet(objects, names);
			return;
		}
	}
}

void SourceExpressionDS_CastStruct::printDebug(std::ostream * const out) const
{
	*out << "SourceExpressionDS_CastStruct(";
	SourceExpressionDS_Base::printDebug(out);
	*out << " ";
		*out << "expressions=(";
		print_debug(out, _expressions);
		*out << ")";

		*out << ", ";

		*out << "type=(";
		print_debug(out, _type);
		*out << ")";
	*out << ")";
}



