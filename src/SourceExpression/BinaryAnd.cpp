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
// SourceExpression handling of "operator &" and "operator &=".
//
//-----------------------------------------------------------------------------

#include "Binary.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../SourceException.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// SourceExpression_BinaryAnd
//
class SourceExpression_BinaryAnd : public SourceExpression_Binary
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(SourceExpression_BinaryAnd,
                                   SourceExpression_Binary);

public:
   SourceExpression_BinaryAnd(bool assign, SRCEXP_EXPRBIN_ARGS);

   virtual CounterPointer<ObjectExpression> makeObject() const;

private:
   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);

   bool assign;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_binary_and
//
SRCEXP_EXPRBIN_DEFN(and)
{
   return new SourceExpression_BinaryAnd
              (false, exprL, exprR, context, position);
}

//
// SourceExpression::create_binary_and_eq
//
SRCEXP_EXPRBIN_DEFN(and_eq)
{
   return create_binary_assign
          (exprL, create_binary_and
                  (exprL, exprR, context, position),
           context, position);
   //return new SourceExpression_BinaryAnd
   //           (true, exprL, exprR, context, position);
}

//
// SourceExpression_BinaryAnd::SourceExpression_BinaryAnd
//
SourceExpression_BinaryAnd::SourceExpression_BinaryAnd
(bool _assign, SRCEXP_EXPRBIN_PARM)
 : Super(false, SRCEXP_EXPRBIN_PASS), assign(_assign)
{
}

//
// SourceExpression_BinaryAnd::makeObject
//
ObjectExpression::Pointer SourceExpression_BinaryAnd::makeObject() const
{
   return ObjectExpression::create_binary_and
          (exprL->makeObject(), exprR->makeObject(), position);
}

//
// SourceExpression_BinaryAnd::virtual_makeObjects
//
void SourceExpression_BinaryAnd::virtual_makeObjects
(ObjectVector *objects, VariableData *dst)
{
   Super::recurse_makeObjects(objects, dst);

   switch (getType()->getBasicType())
   {
   case VariableType::BT_CHAR:
   case VariableType::BT_INT:
   case VariableType::BT_UINT:
      objects->addToken(OCODE_BITWISE_AND32);
      break;

   default:
      throw SourceException("invalid BT", position, getName());
   }

   make_objects_memcpy_post
   (objects, dst, VariableData::create_stack(getType()->getSize(position)), position);
}

// EOF

