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
// SourceExpression handling of "operator =".
//
//-----------------------------------------------------------------------------

#include "Binary.hpp"

#include "../ObjectVector.hpp"
#include "../SourceException.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// SourceExpression_BinaryAssign
//
class SourceExpression_BinaryAssign : public SourceExpression_Binary
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(SourceExpression_BinaryAssign,
                                   SourceExpression_Binary);

public:
   SourceExpression_BinaryAssign(bool allowConst, SRCEXP_EXPRBIN_ARGS);

   virtual bool canMakeObject() const;

private:
   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);

   bool allowConst;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_binary_assign
//
SRCEXP_EXPRBIN_DEFN(assign)
{
   return new SourceExpression_BinaryAssign
              (false, exprL, exprR, context, position);
}

//
// SourceExpression::create_binary_assign_const
//
SRCEXP_EXPRBIN_DEFN(assign_const)
{
   return new SourceExpression_BinaryAssign
              (true, exprL, exprR, context, position);
}

//
// SourceExpression_BinaryAssign::SourceExpression_BinaryAssign
//
SourceExpression_BinaryAssign::
SourceExpression_BinaryAssign(bool _allowConst, SRCEXP_EXPRBIN_PARM)
                              : Super(NULL, _exprL->getType(),
                                      SRCEXP_EXPRBIN_PASS),
                                allowConst(_allowConst)
{
}

//
// SourceExpression_BinaryAssign::canMakeObject
//
// Nothing with side-effects can be made into an object. More specifically,
// code must be emitted even if the yielded value is unused.
//
bool SourceExpression_BinaryAssign::canMakeObject() const
{
   return false;
}

//
// SourceExpression_BinaryAssign::virtual_makeObjects
//
void SourceExpression_BinaryAssign::
virtual_makeObjects(ObjectVector *objects, VariableData *dst)
{
   Super::recurse_makeObjects(objects, dst);

   if (!allowConst && exprL->getType()->getQualifier(VariableType::QUAL_CONST))
      throw SourceException("assignment to const", position, getClassName());

   if (dst->type == VariableData::MT_VOID)
   {
      exprR->makeObjects(objects, exprL->getData());
   }
   else
   {
      VariableType::Reference type = getType();
      VariableData::Pointer   dup  = exprL->getData();
      VariableData::Pointer   src  = VariableData::create_stack(dup->size);

      make_objects_memcpy_prep(objects, dst, dup, src, position);
      exprR->makeObjects(objects, src);
      make_objects_memcpy_post(objects, dst, dup, src, type, context, position);
   }
}

// EOF

