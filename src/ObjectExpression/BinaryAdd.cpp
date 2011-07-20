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

/* ObjectExpression/BinaryAdd.cpp
**
** Defines the ObjectExpression_BinaryAdd class and methods.
*/

#include "Binary.hpp"



class ObjectExpression_BinaryAdd : public ObjectExpression_Binary
{
public:
	ObjectExpression_BinaryAdd(ObjectExpression const & exprL, ObjectExpression const & exprR, SourcePosition const & position);

	virtual ObjectExpression_BinaryAdd * clone() const;

	virtual char const * getName() const;

	virtual void printDebug(std::ostream * out) const;

	virtual ObjectExpression::float_t resolveFloat() const;
	virtual ObjectExpression::int_t resolveInt() const;
};



ObjectExpression ObjectExpression::create_binary_add(ObjectExpression const & exprL, ObjectExpression const & exprR, SourcePosition const & position)
{
	return ObjectExpression_BinaryAdd(exprL, exprR, position);
}



ObjectExpression_BinaryAdd::ObjectExpression_BinaryAdd(ObjectExpression const & exprL, ObjectExpression const & exprR, SourcePosition const & position) : ObjectExpression_Binary(exprL, exprR, position)
{

}

ObjectExpression_BinaryAdd * ObjectExpression_BinaryAdd::clone() const
{
	return new ObjectExpression_BinaryAdd(*this);
}

char const * ObjectExpression_BinaryAdd::getName() const
{
	return "ObjectExpression_BinaryAdd";
}

void ObjectExpression_BinaryAdd::printDebug(std::ostream * const out) const
{
	*out << "ObjectExpression_BinaryAdd(";
	ObjectExpression_Binary::printDebug(out);
	*out << ")";
}

ObjectExpression::float_t ObjectExpression_BinaryAdd::resolveFloat() const
{
	switch (getType())
	{
	case ObjectExpression::ET_FLOAT:
		return _exprL.resolveFloat() + _exprR.resolveFloat();

	case ObjectExpression::ET_INT:
		return (ObjectExpression::float_t)(_exprL.resolveInt() + _exprR.resolveInt());
	}

	return ObjectExpression_Binary::resolveFloat();
}
ObjectExpression::int_t ObjectExpression_BinaryAdd::resolveInt() const
{
	switch (getType())
	{
	case ObjectExpression::ET_FLOAT:
		return (ObjectExpression::int_t)(_exprL.resolveFloat() + _exprR.resolveFloat());

	case ObjectExpression::ET_INT:
		return _exprL.resolveInt() + _exprR.resolveInt();
	}

	return ObjectExpression_Binary::resolveInt();
}



