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

/* ObjectExpression/Binary.hpp
**
** Defines the ObjectExpression_Binary class.
*/

#ifndef HPP_Binary__ObjectExpression_
#define HPP_Binary__ObjectExpression_

#include "../ObjectExpression.hpp"



class ObjectExpression_Binary : public ObjectExpression
{
	MAKE_ABSTRACT_COUNTER_CLASS_BASE(ObjectExpression_Binary, ObjectExpression);

public:
	virtual ExpressionType getType() const;

	virtual void printDebug(std::ostream * out) const;

protected:
	ObjectExpression_Binary(ObjectExpression * exprL, ObjectExpression * exprR, SourcePosition const & position);
	ObjectExpression_Binary(std::istream * in);

	virtual void writeObject(std::ostream * out) const;

	ObjectExpression::Pointer exprL;
	ObjectExpression::Pointer exprR;
};



#endif /* HPP_Binary__ObjectExpression_ */



