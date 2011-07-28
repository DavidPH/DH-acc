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

/* SourceExpression/Binary.hpp
**
** Defines the SourceExpression_Binary class.
*/

#ifndef HPP_Binary__SourceExpression_
#define HPP_Binary__SourceExpression_

#include "../SourceExpression.hpp"



class SourceExpression_Binary : public SourceExpression
{
	MAKE_ABSTRACT_COUNTER_CLASS_BASE(SourceExpression_Binary, SourceExpression);

public:
	SourceExpression_Binary(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position);
	SourceExpression_Binary(SourceExpression * exprL, SourceExpression * exprR, bool const castL, SourcePosition const & position);

	virtual SourceVariable::VariableType const * getType() const;

	virtual bool isConstant() const;

	virtual void makeObjectsGet(ObjectVector * objects) const;

	virtual void printDebug(std::ostream * out) const;

protected:
	SourceExpression::Pointer exprL;
	SourceExpression::Pointer exprR;
};



#endif /* HPP_Binary__SourceExpression_ */



