//-----------------------------------------------------------------------------
//
// Copyright(C) 2011 David Hill
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
// SourceExpression handling of "operator ^^".
//
//-----------------------------------------------------------------------------

#include "BinaryCompare.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../SourceContext.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"



//
// SourceExpression_BranchXOr
//
class SourceExpression_BranchXOr : public SourceExpression_BinaryCompare
{
   MAKE_COUNTER_CLASS_BASE(SourceExpression_BranchXOr,
                           SourceExpression_BinaryCompare);

public:
   SourceExpression_BranchXOr(SourceExpression *exprL, SourceExpression *exprR,
                              SourceContext *context,
                              SourcePosition const &position);

private:
   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);

   std::string labelL0;
   std::string label0;
   std::string label1;
   std::string labelEnd;
};



//
// SourceExpression::create_branch_xor
//
SourceExpression::Pointer SourceExpression::
create_branch_xor(SourceExpression *exprL, SourceExpression *exprR,
                  SourceContext *context, SourcePosition const &position)
{
   return new SourceExpression_BranchXOr(exprL, exprR, context, position);
}

//
// SourceExpression_BranchXOr::SourceExpression_BranchXOr
//
SourceExpression_BranchXOr::
SourceExpression_BranchXOr(SourceExpression *_exprL, SourceExpression *_exprR,
                           SourceContext *context,
                           SourcePosition const &_position)
                           : Super(_exprL, _exprR, true, _position)
{
   std::string label(context->makeLabel());

   labelL0  = label + "_L0";
   label0   = label + "_0";
   label1   = label + "_1";
   labelEnd = label + "_end";
}

//
// SourceExpression_BranchXOr::virtual_makeObjects
//
void SourceExpression_BranchXOr::
virtual_makeObjects(ObjectVector *objects, VariableData *dst)
{
   Super::recurse_makeObjects(objects, dst);

   VariableType const *srcType = VariableType::get_vt_boolhard();
   bigsint             srcSize = srcType->size(position);

   VariableData::Pointer src = VariableData::create_stack(srcSize);
   VariableData::Pointer tmp = VariableData::create_stack(srcSize);

   if (dst->type == VariableData::MT_VOID)
   {
      exprL->makeObjects(objects, dst);
      exprR->makeObjects(objects, dst);
   }
   else
   {
      make_objects_memcpy_prep(objects, dst, src, position);

      exprL->makeObjects(objects, tmp);
      exprR->makeObjects(objects, tmp);

      objects->setPosition(position);
      objects->addToken(OCODE_BRANCH_ZERO,     objects->getValue(labelL0));

      objects->addToken(OCODE_BRANCH_ZERO,     objects->getValue(label1));
      objects->addToken(OCODE_BRANCH_GOTO_IMM, objects->getValue(label0));

      objects->addLabel(labelL0);
      objects->addToken(OCODE_BRANCH_ZERO,     objects->getValue(label0));

      objects->addLabel(label1);
      objects->addToken(OCODE_GET_LITERAL32I,  objects->getValue(1));
      objects->addToken(OCODE_BRANCH_GOTO_IMM, objects->getValue(labelEnd));

      objects->addLabel(label0);
      objects->addToken(OCODE_GET_LITERAL32I,  objects->getValue(0));

      objects->addLabel(labelEnd);

      make_objects_memcpy_post(objects, dst, src, position);
   }
}

// EOF

