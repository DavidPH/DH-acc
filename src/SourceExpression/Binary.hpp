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
// Base class for source-level binary expression handling.
//
//-----------------------------------------------------------------------------

#ifndef HPP_Binary__SourceExpression_
#define HPP_Binary__SourceExpression_

#include "../SourceExpression.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// SourceExpression_Binary
//
class SourceExpression_Binary : public SourceExpression
{
   MAKE_ABSTRACT_COUNTER_CLASS_BASE(SourceExpression_Binary, SourceExpression);

public:
   virtual bool canMakeObject() const;

   virtual CounterReference<VariableType> getType() const;

protected:
   SourceExpression_Binary(SRCEXP_EXPRBIN_ARGS);

   SourceExpression_Binary(bool arithmetic, SRCEXP_EXPRBIN_ARGS);

   SourceExpression_Binary(VariableType *castL, VariableType *castR,
                           SRCEXP_EXPRBIN_ARGS);

   SourceExpression_Binary(VariableType *castL, VariableType *castR,
                           bool arithmetic, SRCEXP_EXPRBIN_ARGS);

   void recurse_makeObjects(ObjectVector *objects, VariableData *dst);

   SourceExpression::Pointer exprL;
   SourceExpression::Pointer exprR;

private:
   void doCast();
   void doCast(VariableType *castL, VariableType *castR);

   int arithmetic;
};

#endif /* HPP_Binary__SourceExpression_ */

