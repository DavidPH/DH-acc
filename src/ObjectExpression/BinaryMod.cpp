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

/* ObjectExpression/BinaryMod.cpp
**
** Defines the ObjectExpression_BinaryAdd class and methods.
*/

#include "Binary.hpp"

#include <cmath>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// ObjectExpression_BinaryMod
//
class ObjectExpression_BinaryMod : public ObjectExpression_Binary
{
	MAKE_COUNTER_CLASS_BASE(ObjectExpression_BinaryMod, ObjectExpression_Binary);

public:
	ObjectExpression_BinaryMod(ObjectExpression * exprL, ObjectExpression * exprR, SourcePosition const & position);
	ObjectExpression_BinaryMod(std::istream * in);

	virtual bigreal resolveFloat() const;
	virtual bigsint resolveInt() const;

protected:
	virtual void writeObject(std::ostream * out) const;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// ObjectExpression::create_binary_mod
//
ObjectExpression::Pointer ObjectExpression::create_binary_mod(ObjectExpression * exprL, ObjectExpression * exprR, SourcePosition const & position)
{
	return new ObjectExpression_BinaryMod(exprL, exprR, position);
}

//
// ObjectExpression::create_binary_mod
//
ObjectExpression::Pointer ObjectExpression::create_binary_mod(std::istream * in)
{
	return new ObjectExpression_BinaryMod(in);
}

//
// ObjectExpression_BinaryMod::ObjectExpression_BinaryMod
//
ObjectExpression_BinaryMod::ObjectExpression_BinaryMod(ObjectExpression * exprL_, ObjectExpression * exprR_, SourcePosition const & position_) : Super(exprL_, exprR_, position_)
{
}

//
// ObjectExpression_BinaryMod::ObjectExpression_BinaryMod
//
ObjectExpression_BinaryMod::ObjectExpression_BinaryMod(std::istream * in) : Super(in)
{
}

//
// ObjectExpression_BinaryMod::resolveFloat
//
bigreal ObjectExpression_BinaryMod::resolveFloat() const
{
	if (getType() == ET_FLOAT) return std::fmod(exprL->resolveFloat(), exprR->resolveFloat());

	return Super::resolveFloat();
}

//
// ObjectExpression_BinaryMod::resolveInt
//
bigsint ObjectExpression_BinaryMod::resolveInt() const
{
	if (getType() == ET_INT) return exprL->resolveInt() % exprR->resolveInt();

	return Super::resolveInt();
}

//
// ObjectExpression_BinaryMod::writeObject
//
void ObjectExpression_BinaryMod::writeObject(std::ostream * out) const
{
	write_object(out, OT_BINARY_MOD);

	Super::writeObject(out);
}

// EOF

