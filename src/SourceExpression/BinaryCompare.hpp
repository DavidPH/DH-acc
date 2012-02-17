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

#ifndef HPP_BinaryCompare__SourceExpression_
#define HPP_BinaryCompare__SourceExpression_

#include "Binary.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// SourceExpression_BinaryCompare
//
class SourceExpression_BinaryCompare : public SourceExpression_Binary
{
   MAKE_ABSTRACT_COUNTER_CLASS_BASE(SourceExpression_BinaryCompare,
                                    SourceExpression_Binary);

public:
   virtual bool canMakeObject() const;

   virtual CounterReference<VariableType> getType() const;

protected:
   SourceExpression_BinaryCompare(SRCEXP_EXPRBIN_ARGS);
   SourceExpression_BinaryCompare(bool branchChecked, SRCEXP_EXPRBIN_ARGS);
};

#endif//HPP_BinaryCompare__SourceExpression_

