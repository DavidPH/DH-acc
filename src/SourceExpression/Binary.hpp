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
	virtual bool canMakeObject() const;

	virtual VariableType const * getType() const;

protected:
	SourceExpression_Binary(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position);
	SourceExpression_Binary(SourceExpression * exprL, SourceExpression * exprR, bool arithmetic, SourcePosition const & position);
	SourceExpression_Binary(SourceExpression * exprL, SourceExpression * exprR, VariableType const * castL, VariableType const * castR, SourcePosition const & position);
	SourceExpression_Binary(SourceExpression * exprL, SourceExpression * exprR, VariableType const * castL, VariableType const * castR, bool arithmetic, SourcePosition const & position);

	void recurse_makeObjectsGet(ObjectVector * objects);

	virtual void printDebug(std::ostream * out) const;

	SourceExpression::Pointer exprL;
	SourceExpression::Pointer exprR;

private:
	void doCast();
	void doCast(VariableType const * castL, VariableType const * castR);

	int _arithmetic;
};



#endif /* HPP_Binary__SourceExpression_ */



