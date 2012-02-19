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
// SourceExpression handling of "operator <<" and "operator >>" and
// "operator <<=" and operator ">>=".
//
//-----------------------------------------------------------------------------

#include "Binary.hpp"

#include "../ObjectVector.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// SourceExpression_BinaryShift
//
class SourceExpression_BinaryShift : public SourceExpression_Binary
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(SourceExpression_BinaryShift,
                                   SourceExpression_Binary);

public:
   SourceExpression_BinaryShift(bool right, SRCEXP_EXPRBIN_ARGS);

   virtual bool canMakeObject() const;

private:
   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);

   bool right;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_binary_lsh
//
SRCEXP_EXPRBIN_DEFN(lsh)
{
   return new SourceExpression_BinaryShift
              (false, exprL, exprR, context, position);
}

//
// SourceExpression::create_binary_lsh_eq
//
SRCEXP_EXPRBIN_DEFN(lsh_eq)
{
   return create_binary_assign
          (exprL, create_binary_lsh
                  (exprL, exprR, context, position),
           context, position);
}

//
// SourceExpression::create_binary_rsh
//
SRCEXP_EXPRBIN_DEFN(rsh)
{
   return new SourceExpression_BinaryShift
              (true, exprL, exprR, context, position);
}

//
// SourceExpression::create_binary_rsh_eq
//
SRCEXP_EXPRBIN_DEFN(rsh_eq)
{
   return create_binary_assign
          (exprL, create_binary_rsh
                  (exprL, exprR, context, position),
           context, position);
}

//
// SourceExpression_BinaryShift::SourceExpression_BinaryShift
//
SourceExpression_BinaryShift::
SourceExpression_BinaryShift(bool _right, SRCEXP_EXPRBIN_PARM)
 : Super(VariableType::get_bt_int(), VariableType::get_bt_int(),
         false, SRCEXP_EXPRBIN_PASS), right(_right)
{
}

//
// SourceExpression_BinaryShift::canMakeObject
//
bool SourceExpression_BinaryShift::canMakeObject() const
{
   return false;
}

//
// SourceExpression_BinaryShift::virtual_makeObjects
//
void SourceExpression_BinaryShift::virtual_makeObjects
(ObjectVector *objects, VariableData *dst)
{
   Super::recurse_makeObjects(objects, dst);

   objects->addToken(right ? OCODE_BITWISE_SHIFTR32 : OCODE_BITWISE_SHIFTL32);

   make_objects_memcpy_post
   (objects, dst, VariableData::create_stack(getType()->getSize(position)), position);
}

// EOF

