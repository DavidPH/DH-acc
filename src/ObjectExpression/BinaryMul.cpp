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

/* ObjectExpression/BinaryMul.cpp
**
** Defines the ObjectExpression_BinaryMul class and methods.
*/

#include "Binary.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// ObjectExpression_BinaryMul
//
class ObjectExpression_BinaryMul : public ObjectExpression_Binary
{
	MAKE_COUNTER_CLASS_BASE(ObjectExpression_BinaryMul, ObjectExpression_Binary);

public:
	ObjectExpression_BinaryMul(ObjectExpression * exprL, ObjectExpression * exprR, SourcePosition const & position);
	ObjectExpression_BinaryMul(std::istream * in);

	virtual bigreal resolveFloat() const;
	virtual bigsint resolveInt() const;

protected:
	virtual void writeObject(std::ostream * out) const;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// ObjectExpression::create_binary_mul
//
ObjectExpression::Pointer ObjectExpression::create_binary_mul(ObjectExpression * exprL, ObjectExpression * exprR, SourcePosition const & position)
{
	return new ObjectExpression_BinaryMul(exprL, exprR, position);
}

//
// ObjectExpression::create_binary_mul
//
ObjectExpression::Pointer ObjectExpression::create_binary_mul(std::istream * in)
{
	return new ObjectExpression_BinaryMul(in);
}

//
// ObjectExpression_BinaryMul::ObjectExpression_BinaryMul
//
ObjectExpression_BinaryMul::ObjectExpression_BinaryMul(ObjectExpression * exprL_, ObjectExpression * exprR_, SourcePosition const & position_) : Super(exprL_, exprR_, position_)
{
}

//
// ObjectExpression_BinaryMul::ObjectExpression_BinaryMul
//
ObjectExpression_BinaryMul::ObjectExpression_BinaryMul(std::istream * in) : Super(in)
{
}

//
// ObjectExpression_BinaryMul::resolveFloat
//
bigreal ObjectExpression_BinaryMul::resolveFloat() const
{
	if (getType() == ET_FLOAT) return exprL->resolveFloat() * exprR->resolveFloat();

	return Super::resolveFloat();
}

//
// ObjectExpression_BinaryMul::resolveInt
//
bigsint ObjectExpression_BinaryMul::resolveInt() const
{
	if (getType() == ET_INT) return exprL->resolveInt() * exprR->resolveInt();

	return Super::resolveInt();
}

//
// ObjectExpression_BinaryMul::writeObject
//
void ObjectExpression_BinaryMul::writeObject(std::ostream * out) const
{
	write_object(out, OT_BINARY_MUL);

	Super::writeObject(out);
}

// EOF

