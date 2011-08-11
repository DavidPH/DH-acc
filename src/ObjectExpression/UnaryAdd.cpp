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

/* ObjectExpression/UnaryAdd.cpp
**
** Defines the ObjectExpression_UnaryAdd class and methods.
*/

#include "Unary.hpp"



class ObjectExpression_UnaryAdd : public ObjectExpression_Unary
{
	MAKE_COUNTER_CLASS_BASE(ObjectExpression_UnaryAdd, ObjectExpression_Unary);

public:
	ObjectExpression_UnaryAdd(ObjectExpression * expr, SourcePosition const & position);
	ObjectExpression_UnaryAdd(std::istream * in);

	virtual void printDebug(std::ostream * out) const;

	virtual float_t resolveFloat() const;
	virtual int_t resolveInt() const;

protected:
	virtual void writeObject(std::ostream * out) const;
};



ObjectExpression::Pointer ObjectExpression::create_unary_add(ObjectExpression * expr, SourcePosition const & position)
{
	return new ObjectExpression_UnaryAdd(expr, position);
}
ObjectExpression::Pointer ObjectExpression::create_unary_add(std::istream * in)
{
	return new ObjectExpression_UnaryAdd(in);
}



ObjectExpression_UnaryAdd::ObjectExpression_UnaryAdd(ObjectExpression * expr, SourcePosition const & position) : Super(expr, position)
{

}
ObjectExpression_UnaryAdd::ObjectExpression_UnaryAdd(std::istream * in) : Super(in)
{

}

void ObjectExpression_UnaryAdd::printDebug(std::ostream * out) const
{
	*out << "ObjectExpression_UnaryAdd(";
	Super::printDebug(out);
	*out << ")";
}

ObjectExpression::float_t ObjectExpression_UnaryAdd::resolveFloat() const
{
	if (getType() == ET_FLOAT) return expr->resolveFloat();

	return Super::resolveFloat();
}
ObjectExpression::int_t ObjectExpression_UnaryAdd::resolveInt() const
{
	if (getType() == ET_INT) return expr->resolveInt();

	return Super::resolveInt();
}

void ObjectExpression_UnaryAdd::writeObject(std::ostream * out) const
{
	write_object(out, OT_UNARY_ADD);

	Super::writeObject(out);
}



