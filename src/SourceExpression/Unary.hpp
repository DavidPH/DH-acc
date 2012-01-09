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
	virtual VariableType const * getType() const;

protected:
	SourceExpression_Unary(SourceExpression * expr, SourcePosition const & position);
	SourceExpression_Unary(SourceExpression * expr, VariableType const * cast, SourcePosition const & position);

	virtual void printDebug(std::ostream * out) const;

	void recurse_makeObjects(ObjectVector *objects, VariableData *dst);

	SourceExpression::Pointer expr;

private:
	bool _make;
};



#endif /* HPP_Binary__SourceExpression_ */



