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

/* SourceExpression/BranchNot.cpp
**
** Defines the SourceExpression_BranchNot class and methods.
*/

#include "Unary.hpp"

#include "../ObjectVector.hpp"
#include "../SourceException.hpp"



class SourceExpression_BranchNot : public SourceExpression_Unary
{
	MAKE_COUNTER_CLASS_BASE(SourceExpression_BranchNot, SourceExpression_Unary);

public:
	SourceExpression_BranchNot(SourceExpression * expr, SourcePosition const & position);

	virtual void makeObjectsGet(ObjectVector * objects);

protected:
	virtual void printDebug(std::ostream * const out) const;
};



SourceExpression::Pointer SourceExpression::create_branch_not(SourceExpression * expr, SourceContext * context, SourcePosition const & position)
{
	return new SourceExpression_BranchNot(expr, position);
}



SourceExpression_BranchNot::SourceExpression_BranchNot(SourceExpression * expr, SourcePosition const & position) : Super(expr, SourceVariable::get_VariableType(SourceVariable::VT_BOOLSOFT), position)
{

}

void SourceExpression_BranchNot::makeObjectsGet(ObjectVector * objects)
{
	Super::recurse_makeObjectsGet(objects);

	objects->addToken(OCODE_LOGICALNOT);
}

void SourceExpression_BranchNot::printDebug(std::ostream * out) const
{
	*out << "SourceExpression_BranchNot(";
	Super::printDebug(out);
	*out << ")";
}



