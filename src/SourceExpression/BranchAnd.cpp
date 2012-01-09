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
// SourceExpression handling of "operator &&".
//
//-----------------------------------------------------------------------------

#include "BinaryCompare.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../SourceContext.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"



//
// SourceExpression_BranchAnd
//
class SourceExpression_BranchAnd : public SourceExpression_BinaryCompare
{
   MAKE_COUNTER_CLASS_BASE(SourceExpression_BranchAnd,
                           SourceExpression_BinaryCompare);

public:
   SourceExpression_BranchAnd(SourceExpression *exprL, SourceExpression *exprR,
                              SourceContext *context,
                              SourcePosition const &position);

protected:
   virtual void printDebug(std::ostream *out) const;

private:
   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);

   std::string label0;
   std::string labelEnd;
};



//
// SourceExpression::create_branch_and
//
SourceExpression::Pointer SourceExpression::
create_branch_and(SourceExpression *exprL, SourceExpression *exprR,
                  SourceContext *context, SourcePosition const &position)
{
   return new SourceExpression_BranchAnd(exprL, exprR, context, position);
}



//
// SourceExpression_BranchAnd::SourceExpression_BranchAnd
//
SourceExpression_BranchAnd::
SourceExpression_BranchAnd(SourceExpression *_exprL, SourceExpression *_exprR,
                           SourceContext *context,
                           SourcePosition const &_position)
                           : Super(_exprL, _exprR, true, _position)
{
   std::string label(context->makeLabel());

   label0   = label + "_0";
   labelEnd = label + "_end";
}

//
// SourceExpression_BranchAnd::printDebug
//
void SourceExpression_BranchAnd::printDebug(std::ostream *out) const
{
   *out << "SourceExpression_BranchAnd(";
   Super::printDebug(out);
   *out << ")";
}

//
// SourceExpression_BranchAnd::virtual_makeObjects
//
void SourceExpression_BranchAnd::
virtual_makeObjects(ObjectVector *objects, VariableData *dst)
{
   Super::recurse_makeObjects(objects, dst);

   VariableType const *srcType = VariableType::get_vt_boolhard();
   bigsint             srcSize = srcType->size(position);

   VariableData::Pointer src = VariableData::create_stack(srcSize);
   VariableData::Pointer tmp = VariableData::create_stack(srcSize);

   if (dst->type == VariableData::MT_VOID)
   {
      exprL->makeObjects(objects, tmp);
      objects->setPosition(position);
      objects->addToken(OCODE_BRANCH_ZERO, objects->getValue(labelEnd));

      exprR->makeObjects(objects, dst);

      objects->addLabel(labelEnd);
   }
   else
   {
      make_objects_memcpy_prep(objects, dst, src, position);

      exprL->makeObjects(objects, tmp);
      objects->setPosition(position);
      objects->addToken(OCODE_BRANCH_ZERO, objects->getValue(label0));

      exprR->makeObjects(objects, tmp);
      objects->setPosition(position);
      objects->addToken(OCODE_BRANCH_ZERO, objects->getValue(label0));

      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue(1));
      objects->addToken(OCODE_BRANCH_GOTO_IMM, objects->getValue(labelEnd));

      objects->addLabel(label0);
      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue(0));

      objects->addLabel(labelEnd);

      make_objects_memcpy_post(objects, dst, src, position);
   }
}



// EOF

