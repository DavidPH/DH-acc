//-----------------------------------------------------------------------------
//
// Copyright(C) 2011, 2012 David Hill
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------
//
// Base class for source-level binary comparison expression handling.
//
//-----------------------------------------------------------------------------

#include "BinaryCompare.hpp"

#include "../SourceException.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression_BinaryCompare::SourceExpression_BinaryCompare
//
SourceExpression_BinaryCompare::
SourceExpression_BinaryCompare(SRCEXP_EXPRBIN_PARM)
                               : Super(false, SRCEXP_EXPRBIN_PASS)
{
}

//
// SourceExpression_BinaryCompare::SourceExpression_BinaryCompare
//
SourceExpression_BinaryCompare::
SourceExpression_BinaryCompare(bool, SRCEXP_EXPRBIN_PARM)
 : Super(VariableType::get_bt_boolsoft(), VariableType::get_bt_boolsoft(),
         SRCEXP_EXPRBIN_PASS)
{
}

//
// SourceExpression_BinaryCompare::canMakeObject
//
bool SourceExpression_BinaryCompare::canMakeObject() const
{
   return false;
}

//
// SourceExpression_BinaryCompare::getType
//
VariableType::Reference SourceExpression_BinaryCompare::getType() const
{
   return VariableType::get_bt_boolhard();
}

// EOF

