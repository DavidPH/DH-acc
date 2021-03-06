//-----------------------------------------------------------------------------
//
// Copyright(C) 2011-2013 David Hill
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
// Base class for source-level unary expression handling.
//
//-----------------------------------------------------------------------------

#ifndef HPP_Unary__SourceExpression_
#define HPP_Unary__SourceExpression_

#include "../SourceExpression.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// SourceExpression_Unary
//
class SourceExpression_Unary : public SourceExpression
{
   CounterPreambleAbstract(SourceExpression_Unary, SourceExpression);

public:
   virtual CounterReference<VariableType> getType() const;

protected:
   SourceExpression_Unary(SRCEXP_EXPRUNA_ARGS);
   SourceExpression_Unary(VariableType *cast, SRCEXP_EXPRUNA_ARGS);

   // isReturn
   virtual bool isReturn() const {return expr->isReturn();}

   //
   // isSideEffect
   //
   virtual bool isSideEffect() const
   {
      return expr->isSideEffect();
   }

   void recurse_makeObjects(ObjectVector *objects, VariableData *dst);

   SourceExpression::Pointer expr;

private:
   bool make;
};

#endif//HPP_Unary__SourceExpression_

