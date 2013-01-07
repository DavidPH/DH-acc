//-----------------------------------------------------------------------------
//
// Copyright(C) 2012-2013 David Hill
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
// SourceExpression handling of stackptr access.
//
//-----------------------------------------------------------------------------

#include "../SourceExpression.hpp"

#include "../ObjectExpression.hpp"
#include "../option.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// SourceExpression_BinaryPair
//
class SourceExpression_BinaryPair : public SourceExpression
{
   CounterPreambleNoClone(SourceExpression_BinaryPair, SourceExpression);

public:
   //
   // ::SourceExpression_BinaryPair
   //
   SourceExpression_BinaryPair(SRCEXP_EXPRBIN_PARM)
    : Super(SRCEXP_EXPR_PASS), exprL(_exprL), exprR(_exprR)
   {
   }

   //
   // ::canMakeObject
   //
   virtual bool canMakeObject() const
   {
      if(!exprR->canMakeObject()) return false;

      // Ideally, the above is the only requirement to makeObject. However, this
      // is largely untested and can easily result in erroneous code discarding.
      if(AllowMakeObject || !exprL->isSideEffect()) return true;

      return exprL->canMakeObject();
   }

   //
   // ::getType
   //
   virtual VariableType::Reference getType() const
   {
      return exprR->getType();
   }

   //
   // isSideEffect
   //
   virtual bool isSideEffect() const
   {
      return exprL->isSideEffect() || exprR->isSideEffect();
   }

   //
   // ::makeObject
   //
   virtual ObjectExpression::Pointer makeObject() const
   {
      return exprR->makeObject();
   }


   static bool AllowMakeObject;

private:
   //
   // ::virtual_makeObjects
   //
   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst)
   {
      Super::recurse_makeObjects(objects, dst);

      exprL->makeObjects(objects);
      exprR->makeObjects(objects, dst);
   }

   SourceExpression::Pointer exprL, exprR;
};


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

bool SourceExpression_BinaryPair::AllowMakeObject = true;

option::option_dptr<bool> Option_PairMakeObject('\0', "pair-make-object",
   "optimization", "Allows the compiler to use the compile-time result of a "
   "comma operator even when it needs to evaluate the left expression for side "
   "effects. If the compiler is omitting code for such expressions or if "
   "variables are not being initialized, disable this. On by default.", NULL,
   &SourceExpression_BinaryPair::AllowMakeObject);


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_binary_pair
//
SRCEXP_EXPRBIN_DEFN(pair)
{
   return new SourceExpression_BinaryPair(exprL, exprR, context, pos);
}

// EOF

