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

/* SourceExpressionDS/CastArray.cpp
**
** Defines the SourceExpressionDS_CastArray class and methods.
*/

#include "Base.hpp"

#include "../print_debug.hpp"
#include "../SourceException.hpp"



class SourceExpressionDS_CastArray : public SourceExpressionDS_Base
{
public:
	SourceExpressionDS_CastArray(SourceExpressionDS const & expr, SourceVariable::VariableType const * const type, SourcePosition const & position);

	virtual SourceExpressionDS_CastArray * clone() const;

	virtual char const * getName() const;

	virtual SourceVariable::VariableType const * getType() const;

	virtual bool isConstant() const;

	virtual void makeObjectsGet(ObjectVector * objects) const;

	virtual void printDebug(std::ostream * const out) const;

private:
	std::vector<SourceExpressionDS> _expressions;
	SourceVariable::VariableType const * _type;
};



SourceExpressionDS SourceExpressionDS::make_expression_cast_array(SourceExpressionDS const & expr, SourceVariable::VariableType const * const type, SourcePosition const & position)
{
	return new SourceExpressionDS_CastArray(expr, type, position);
}



SourceExpressionDS_CastArray::SourceExpressionDS_CastArray(SourceExpressionDS const & expr, SourceVariable::VariableType const * const type, SourcePosition const & position) : SourceExpressionDS_Base(position), _expressions(expr.getVector()), _type(type)
{
	if (_type->type != SourceVariable::VT_ARRAY)
		throw SourceException("type not VT_ARRAY", getPosition(), getName());

	if (_expressions.size() != _type->types.size())
		throw SourceException("insufficient block elements", getPosition(), getName());

	for (size_t i(0); i < _expressions.size(); ++i)
	{
		if (_expressions[i].getType() != _type->types[i])
			_expressions[i] = SourceExpressionDS::make_expression_cast(_expressions[i], _type->types[i], getPosition());
	}
}

SourceExpressionDS_CastArray * SourceExpressionDS_CastArray::clone() const
{
	return new SourceExpressionDS_CastArray(*this);
}

char const * SourceExpressionDS_CastArray::getName() const
{
	return "SourceExpressionDS_CastArray";
}

SourceVariable::VariableType const * SourceExpressionDS_CastArray::getType() const
{
	return _type;
}

bool SourceExpressionDS_CastArray::isConstant() const
{
	// TODO: The block is constant if all expressions are.
	return false;
}

void SourceExpressionDS_CastArray::makeObjectsGet(ObjectVector * objects) const
{
	for (size_t i(0); i < _expressions.size(); ++i)
		_expressions[i].makeObjectsGet(objects);
}

void SourceExpressionDS_CastArray::printDebug(std::ostream * const out) const
{
	*out << "SourceExpressionDS_CastArray(";
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



