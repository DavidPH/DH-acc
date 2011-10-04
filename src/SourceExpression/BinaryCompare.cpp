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
#include "../VariableType.hpp"



SourceExpression_BinaryCompare::SourceExpression_BinaryCompare(SourceExpression * exprL_, SourceExpression * exprR_, SourcePosition const & position_) : Super(exprL, exprR, false, position)
{

}
SourceExpression_BinaryCompare::SourceExpression_BinaryCompare(SourceExpression * exprL_, SourceExpression * exprR_, bool branchChecked, SourcePosition const & position_) : Super(exprL_, exprR_, VariableType::get_vt_boolsoft(), VariableType::get_vt_boolsoft(), position_)
{
	(void)branchChecked; // dummy arg
}

bool SourceExpression_BinaryCompare::canMakeObject() const
{
	return false;
}

VariableType const * SourceExpression_BinaryCompare::getType() const
{
	return VariableType::get_vt_boolhard();
}

void SourceExpression_BinaryCompare::printDebug(std::ostream * out) const
{
	*out << "SourceExpression_BinaryCompare(";
	Super::printDebug(out);
	*out << ")";
}


