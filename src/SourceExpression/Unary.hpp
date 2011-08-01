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

/* SourceExpression/Unary.hpp
**
** Defines the SourceExpression_Unary class.
*/

#ifndef HPP_Unary__SourceExpression_
#define HPP_Unary__SourceExpression_

#include "../SourceExpression.hpp"



class SourceExpression_Unary : public SourceExpression
{
	MAKE_ABSTRACT_COUNTER_CLASS_BASE(SourceExpression_Unary, SourceExpression);

public:
	SourceExpression_Unary(SourceExpression * expr, SourcePosition const & position);

	virtual SourceVariable::VariableType const * getType() const;

	virtual void makeObjectsGet(ObjectVector * objects) const;

	virtual void printDebug(std::ostream * out) const;

protected:
	SourceExpression::Pointer expr;
};



#endif /* HPP_Binary__SourceExpression_ */



