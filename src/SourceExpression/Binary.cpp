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

#include "Binary.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../SourceContext.hpp"
#include "../SourceException.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression_Binary::SourceExpression_Binary
//
SourceExpression_Binary::
SourceExpression_Binary(SRCEXP_EXPRBIN_PARM)
                        : Super(SRCEXP_EXPR_PASS),
                          exprL(_exprL), exprR(_exprR), arithmetic(2)
{
   doCast();
}

//
// SourceExpression_Binary::SourceExpression_Binary
//
SourceExpression_Binary::
SourceExpression_Binary(bool _arithmetic, SRCEXP_EXPRBIN_PARM)
                        : Super(SRCEXP_EXPR_PASS),
                          exprL(_exprL), exprR(_exprR), arithmetic(_arithmetic)
{
   doCast();
}

//
// SourceExpression_Binary::SourceExpression_Binary
//
SourceExpression_Binary::
SourceExpression_Binary(VariableType *castL, VariableType *castR,
                        SRCEXP_EXPRBIN_PARM)
                        : Super(SRCEXP_EXPR_PASS),
                          exprL(_exprL), exprR(_exprR), arithmetic(2)
{
   doCast(castL, castR);
}

//
// SourceExpression_Binary::SourceExpression_Binary
//
SourceExpression_Binary::
SourceExpression_Binary(VariableType *castL, VariableType *castR,
                        bool _arithmetic, SRCEXP_EXPRBIN_PARM)
                        : Super(SRCEXP_EXPR_PASS),
                          exprL(_exprL), exprR(_exprR), arithmetic(_arithmetic)
{
   doCast(castL, castR);
}

//
// SourceExpression_Binary::canMakeObject
//
bool SourceExpression_Binary::canMakeObject() const
{
   return exprL->canMakeObject() && exprR->canMakeObject();
}

//
// SourceExpression_Binary::doAssignBase
//
void SourceExpression_Binary::doAssignBase
(ObjectVector *objects, VariableData *dst, VariableData *src,
 ObjectCode ocodeOp, ObjectCode ocodeGet)
{
   ObjectExpression::Pointer tmpA = context->getTempVar(0);
   VariableType::Reference typeL = exprL->getType();

   VariableData::Pointer tmp = VariableData::create_stack(src->size);

   if (dst->type != VariableData::MT_VOID)
      make_objects_memcpy_prep(objects, dst, tmp, position);

   // MT_REGISTERARRAY addressing.
   if (src->type == VariableData::MT_REGISTERARRAY)
   {
      src->offsetTemp = VariableData::create_stack
                        (src->offsetExpr->getType()->getSize(position));
      src->offsetExpr->makeObjects(objects, src->offsetTemp);

      // Extra address for get.
      if (dst->type != VariableData::MT_VOID)
         objects->addToken(OCODE_STACK_DUP32);
   }

   create_value_cast_implicit(exprR, typeL, context, position)
   ->makeObjects(objects, tmp);

   // MT_POINTER addressing.
   if (src->type == VariableData::MT_POINTER)
   {
      src->offsetTemp = VariableData::create_stack
                        (src->offsetExpr->getType()->getSize(position));
      src->offsetExpr->makeObjects(objects, src->offsetTemp);

      // Extra address for get.
      if (dst->type != VariableData::MT_VOID)
      {
         objects->addToken(OCODE_SET_TEMP, tmpA);
         objects->addToken(OCODE_GET_TEMP, tmpA);
      }
   }

   objects->addToken(ocodeOp, src->address);

   if (dst->type != VariableData::MT_VOID)
   {
      // MT_POINTER addressing.
      if (src->type == VariableData::MT_POINTER)
         objects->addToken(OCODE_GET_TEMP, tmpA);

      objects->addToken(ocodeGet, src->address);

      make_objects_memcpy_post(objects, dst, tmp, typeL, context, position);
   }
}

//
// SourceExpression_Binary::doEvaluateBase
//
void SourceExpression_Binary::doEvaluateBase
(ObjectVector *objects, VariableData *dst, VariableData *src, ObjectCode ocode)
{
   VariableType::Reference type = getType();

   make_objects_memcpy_prep(objects, dst, src, position);

   create_value_cast_implicit(exprL, type, context, position)
   ->makeObjects(objects, src);

   create_value_cast_implicit(exprR, type, context, position)
   ->makeObjects(objects, src);

   objects->addToken(ocode);

   make_objects_memcpy_post(objects, dst, src, type, context, position);
}

//
// SourceExpression_Binary::doEvaluateBaseLLAS
//
void SourceExpression_Binary::doEvaluateBaseLLAS
(ObjectVector *objects, VariableData *dst, VariableData *src, bool add)
{
   VariableType::Reference type = getType();

   std::string labelCmp = label + "_cmp";
   std::string labelEnd = label + "_end";
   std::string labelOvr = label + "_ovr";
   std::string labelPos = label + "_pos";

   ObjectExpression::Pointer tmpL = context->getTempVar(0);
   ObjectExpression::Pointer tmpH = context->getTempVar(1);
   ObjectExpression::Pointer tmpI = context->getTempVar(2);

   make_objects_memcpy_prep(objects, dst, src, position);

   create_value_cast_implicit(exprR, type, context, position)
   ->makeObjects(objects, src);

   create_value_cast_implicit(exprL, type, context, position)
   ->makeObjects(objects, src);

   objects->addToken(OCODE_SET_TEMP, tmpH);
   objects->addToken(OCODE_SET_TEMP, tmpL);
   objects->addToken(OCODE_GET_TEMP, tmpL);
   objects->addToken(OCODE_SET_TEMP, tmpI);

   if (add)
   {
      objects->addToken(OCODE_SETOP_ADD_TEMP, tmpH);
      objects->addToken(OCODE_SETOP_ADD_TEMP, tmpL);
   }
   else
   {
      objects->addToken(OCODE_SETOP_SUB_TEMP, tmpH);
      objects->addToken(OCODE_SETOP_SUB_TEMP, tmpL);
   }

   // if (l & 0x80000000)
   objects->addToken(OCODE_GET_TEMP, tmpL);
   objects->addToken(OCODE_GET_LITERAL32I, objects->getValue(0x80000000));
   objects->addToken(OCODE_BITWISE_AND32);
   objects->addToken(OCODE_BRANCH_ZERO, objects->getValue(labelPos));

   // ... then
   //    if (!(i & 0x80000000)) goto ovr/end;
   objects->addToken(OCODE_GET_TEMP, tmpI);
   objects->addToken(OCODE_GET_LITERAL32I, objects->getValue(0x80000000));
   objects->addToken(OCODE_BITWISE_AND32);
   objects->addToken(OCODE_BRANCH_ZERO, objects->getValue(add ? labelEnd : labelOvr));
   objects->addToken(OCODE_BRANCH_GOTO_IMM, objects->getValue(labelCmp));

   // ... else
   //    if (i & 0x80000000) goto end/ovr;
   objects->addLabel(labelPos);
   objects->addToken(OCODE_GET_TEMP, tmpI);
   objects->addToken(OCODE_GET_LITERAL32I, objects->getValue(0x80000000));
   objects->addToken(OCODE_BITWISE_AND32);
   objects->addToken(OCODE_BRANCH_TRUE, objects->getValue(add ? labelOvr : labelEnd));

   objects->addLabel(labelCmp);
   objects->addToken(OCODE_GET_TEMP, tmpL);
   objects->addToken(OCODE_GET_TEMP, tmpI);
   objects->addToken(add ? OCODE_CMP_LT32I : OCODE_CMP_GT32I);
   objects->addToken(OCODE_BRANCH_ZERO, objects->getValue(labelEnd));

   objects->addLabel(labelOvr);
   objects->addToken(OCODE_SETOP_INC_TEMP, tmpH);
   objects->addLabel(labelEnd);
   objects->addToken(OCODE_GET_TEMP, tmpL);
   objects->addToken(OCODE_GET_TEMP, tmpH);

   make_objects_memcpy_post(objects, dst, src, type, context, position);
}

//
// SourceExpression_Binary::doEvaluateBaseLLB
//
void SourceExpression_Binary::doEvaluateBaseLLB
(ObjectVector *objects, VariableData *dst, VariableData *src, ObjectCode ocode)
{
   VariableType::Reference type = getType();

   make_objects_memcpy_prep(objects, dst, src, position);

   create_value_cast_implicit(exprL, type, context, position)
   ->makeObjects(objects, src);

   create_value_cast_implicit(exprR, type, context, position)
   ->makeObjects(objects, src);

   ObjectExpression::Pointer tmpL = context->getTempVar(0);
   ObjectExpression::Pointer tmpH = context->getTempVar(1);

   objects->addToken(OCODE_SET_TEMP, tmpH);
   objects->addToken(OCODE_SET_TEMP, tmpL);

   objects->addToken(ocode, tmpH);
   objects->addToken(ocode, tmpL);

   objects->addToken(OCODE_GET_TEMP, tmpL);
   objects->addToken(OCODE_GET_TEMP, tmpH);

   make_objects_memcpy_post(objects, dst, src, type, context, position);
}

//
// SourceExpression_Binary::doCast
//
void SourceExpression_Binary::doCast()
{
   VariableType::Reference type = getType();

   exprL = create_value_cast_implicit(exprL, type, context, position);
   exprR = create_value_cast_implicit(exprR, type, context, position);
}

//
// SourceExpression_Binary::doCast
//
void SourceExpression_Binary::
doCast(VariableType *castL, VariableType *castR)
{
   if (castL)
      exprL = create_value_cast_implicit(exprL, castL, context, position);

   if (castR)
      exprR = create_value_cast_implicit(exprR, castR, context, position);
}

//
// SourceExpression_Binary::getOcodeType
//
int SourceExpression_Binary::getOcodeType
(VariableType::BasicType bt, int *ocodeGetType)
{
   int ocodeOpType;
   switch (bt)
   {
   case VariableType::BT_CHAR:
   case VariableType::BT_INT:
   case VariableType::BT_LONG:
   case VariableType::BT_SCHAR:
   case VariableType::BT_SHORT:
      ocodeOpType = 1;
      if (ocodeGetType) *ocodeGetType = 1;
      break;

   case VariableType::BT_FIXED:
   case VariableType::BT_REAL:
      ocodeOpType = 0;
      if (ocodeGetType) *ocodeGetType = 0;
      break;

   case VariableType::BT_POINTER:
   case VariableType::BT_UCHAR:
   case VariableType::BT_UINT:
   case VariableType::BT_ULONG:
   case VariableType::BT_USHORT:
      ocodeOpType = 2;
      if (ocodeGetType) *ocodeGetType = 1;
      break;

   default:
      ERROR_N(position, "invalid BT: %s", make_string(bt).c_str());
   }

   return ocodeOpType;
}

//
// SourceExpression_Binary::getType
//
VariableType::Reference SourceExpression_Binary::getType() const
{
   return get_promoted_type(exprL->getType(), exprR->getType(), position);
}

//
// SourceExpression_Binary::recurse_makeObjects
//
void SourceExpression_Binary::
recurse_makeObjects(ObjectVector *objects, VariableData *dst)
{
   Super::recurse_makeObjects(objects, dst);

   // Special case, child handles expressions.
   if (arithmetic == 2) return;

   VariableData::Pointer   src =
      VariableData::create_stack(getType()->getSize(position));

   make_objects_memcpy_prep(objects, dst, src, position);
   exprL->makeObjects(objects, src);
   exprR->makeObjects(objects, src);

   objects->setPosition(position);
}

// EOF

