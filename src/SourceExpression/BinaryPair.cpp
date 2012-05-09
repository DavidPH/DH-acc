//-----------------------------------------------------------------------------
//
// Copyright(C) 2012 David Hill
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
   MAKE_NOCLONE_COUNTER_CLASS_BASE(SourceExpression_BinaryPair,
      SourceExpression);

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
      return exprL->canMakeObject() && exprR->canMakeObject();
   }

   //
   // ::getType
   //
   virtual VariableType::Reference getType() const
   {
      return exprR->getType();
   }

   //
   // ::makeObject
   //
   virtual ObjectExpression::Pointer makeObject() const
   {
      return exprL->makeObject(), exprR->makeObject();
   }

private:
   //
   // ::virtual_makeObjects
   //
   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst)
   {
      Super::recurse_makeObjects(objects, dst);

      exprL->makeObjects(objects, VariableData::create_void(0));
      exprR->makeObjects(objects, dst);
   }

   std::string name;
   SourceExpression::Pointer exprL, exprR;
};


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

