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

/* SourceExpression/BinaryCompare.cpp
**
** Defines the SourceExpression_BinaryCompare methods.
*/

#include "BinaryCompare.hpp"

#include "../SourceException.hpp"



SourceExpression_BinaryCompare::SourceExpression_BinaryCompare(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position) : Super(exprL, exprR, false, position)
{

}
SourceExpression_BinaryCompare::SourceExpression_BinaryCompare(SourceExpression * exprL, SourceExpression * exprR, bool branchChecked, SourcePosition const & position) : Super(exprL, exprR, position)
{
	if (branchChecked) switch (exprL->getType()->type)
	{
	case SourceVariable::VT_CHAR:
	case SourceVariable::VT_FUNCTION:
	case SourceVariable::VT_INT:
	case SourceVariable::VT_LINESPEC:
	case SourceVariable::VT_NATIVE:
	case SourceVariable::VT_POINTER:
	case SourceVariable::VT_REAL:
	case SourceVariable::VT_SCRIPT:
	case SourceVariable::VT_STRING:
		break;

	default:
		throw SourceException("invalid VT for branch", position, getName());
	}
}

SourceVariable::VariableType const * SourceExpression_BinaryCompare::getType() const
{
	return SourceVariable::get_VariableType(SourceVariable::VT_INT);
}

void SourceExpression_BinaryCompare::printDebug(std::ostream * out) const
{
	*out << "SourceExpression_BinaryCompare(";
	Super::printDebug(out);
	*out << ")";
}



