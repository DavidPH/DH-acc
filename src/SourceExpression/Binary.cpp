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
SourceExpression_Binary::SourceExpression_Binary(SRCEXP_EXPRBIN_PARM)
 : Super(SRCEXP_EXPR_PASS), exprL(_exprL), exprR(_exprR), assign(false)
{
   VariableType::Reference type = getType();

   exprL = create_value_cast_implicit(exprL, type, context, pos);
   exprR = create_value_cast_implicit(exprR, type, context, pos);
}

//
// SourceExpression_Binary::SourceExpression_Binary
//
SourceExpression_Binary::SourceExpression_Binary(SRCEXP_EXPRBIN_PARM,
   VariableType *castL, VariableType *castR, bool _assign)
 : Super(SRCEXP_EXPR_PASS), exprL(_exprL), exprR(_exprR), assign(_assign)
{
   if (castL) exprL = create_value_cast_implicit(exprL, castL, context, pos);
   if (castR) exprR = create_value_cast_implicit(exprR, castR, context, pos);
}

//
// SourceExpression_Binary::canMakeObject
//
bool SourceExpression_Binary::canMakeObject() const
{
   return exprL->canMakeObject() && exprR->canMakeObject();
}

//
// SourceExpression_Binary::doGet
//
void SourceExpression_Binary::doGet(ObjectVector *, VariableType *, int)
{
   ERROR_NP("stub");
}

//
// SourceExpression_Binary::doGetBase
//
void SourceExpression_Binary::doGetBase(ObjectVector *objects,
                                        VariableData *dst)
{
   VariableType::Reference type = getType();
   VariableData::Pointer src = VariableData::
      create_stack(type->getSize(pos));

   make_objects_memcpy_prep(objects, dst, src, pos);

   create_value_cast_explicit(exprL, type, context, pos)
      ->makeObjects(objects, src);

   create_value_cast_explicit(exprR, type, context, pos)
      ->makeObjects(objects, src);

   doGet(objects, type, 0);

   make_objects_memcpy_post(objects, dst, src, type, context, pos);
}

//
// SourceExpression_Binary::doGetBaseILLAS
//
void SourceExpression_Binary::doGetBaseILLAS(ObjectVector *objects,
   VariableType *type, int tmpBase, bool add)
{
   VariableData::Pointer tmp = VariableData::create_stack(type->getSize(pos));

   std::string label = context->makeLabel();
   std::string labelCmp = label + "_cmp";
   std::string labelEnd = label + "_end";
   std::string labelOvr = label + "_ovr";
   std::string labelPos = label + "_pos";

   ObjectExpression::Pointer tmpL = context->getTempVar(tmpBase+0);
   ObjectExpression::Pointer tmpH = context->getTempVar(tmpBase+1);
   ObjectExpression::Pointer tmpI = context->getTempVar(tmpBase+2);

   create_value_cast_implicit(exprR, type, context, pos)
   ->makeObjects(objects, tmp);

   create_value_cast_implicit(exprL, type, context, pos)
   ->makeObjects(objects, tmp);

   objects->addToken(OCODE_SET_TEMP, tmpH);
   objects->addToken(OCODE_SET_TEMP, tmpL);
   objects->addToken(OCODE_GET_TEMP, tmpL);
   objects->addToken(OCODE_SET_TEMP, tmpI);

   if (add)
   {
      objects->addToken(OCODE_ADD_TEMP_I, tmpH);
      objects->addToken(OCODE_ADD_TEMP_I, tmpL);
   }
   else
   {
      objects->addToken(OCODE_SUB_TEMP_I, tmpH);
      objects->addToken(OCODE_SUB_TEMP_I, tmpL);
   }

   // if (l & 0x80000000)
   objects->addToken(OCODE_GET_TEMP, tmpL);
   objects->addToken(OCODE_GET_IMM, objects->getValue(0x80000000));
   objects->addToken(OCODE_AND_STK_I);
   objects->addToken(OCODE_JMP_NIL, objects->getValue(labelPos));

   // ... then
   //    if (!(i & 0x80000000)) goto ovr/end;
   objects->addToken(OCODE_GET_TEMP, tmpI);
   objects->addToken(OCODE_GET_IMM, objects->getValue(0x80000000));
   objects->addToken(OCODE_AND_STK_I);
   objects->addToken(OCODE_JMP_NIL, objects->getValue(add ? labelEnd : labelOvr));
   objects->addToken(OCODE_JMP_IMM, objects->getValue(labelCmp));

   // ... else
   //    if (i & 0x80000000) goto end/ovr;
   objects->addLabel(labelPos);
   objects->addToken(OCODE_GET_TEMP, tmpI);
   objects->addToken(OCODE_GET_IMM, objects->getValue(0x80000000));
   objects->addToken(OCODE_AND_STK_I);
   objects->addToken(OCODE_JMP_TRU, objects->getValue(add ? labelOvr : labelEnd));

   objects->addLabel(labelCmp);
   objects->addToken(OCODE_GET_TEMP, tmpL);
   objects->addToken(OCODE_GET_TEMP, tmpI);
   objects->addToken(add ? OCODE_CMP_LT_I : OCODE_CMP_GT_I);
   objects->addToken(OCODE_JMP_NIL, objects->getValue(labelEnd));

   objects->addLabel(labelOvr);
   objects->addToken(OCODE_INC_TEMP_I, tmpH);
   objects->addLabel(labelEnd);
   objects->addToken(OCODE_GET_TEMP, tmpL);
   objects->addToken(OCODE_GET_TEMP, tmpH);
}

//
// SourceExpression_Binary::doGetBaseILLB
//
void SourceExpression_Binary::doGetBaseILLB(ObjectVector *objects,
   VariableType *, int tmpBase, ObjectCode ocode)
{
   ObjectExpression::Pointer tmpL = context->getTempVar(tmpBase+0);
   ObjectExpression::Pointer tmpH = context->getTempVar(tmpBase+1);

   objects->addToken(OCODE_SET_TEMP, tmpH);
   objects->addToken(OCODE_SET_TEMP, tmpL);

   objects->addToken(ocode, tmpH);
   objects->addToken(ocode, tmpL);

   objects->addToken(OCODE_GET_TEMP, tmpL);
   objects->addToken(OCODE_GET_TEMP, tmpH);
}

//
// SourceExpression_Binary::doSet
//
bool SourceExpression_Binary::doSet(ObjectVector *, VariableData *,
                                    VariableType *, int)
{
   return false;
}

//
// SourceExpression_Binary::doSetBase
//
void SourceExpression_Binary::doSetBase(ObjectVector *objects,
                                        VariableData *dst)
{
   int tmpBase = 0;
   ObjectExpression::Pointer tmpA;
   VariableType::Reference typeL = exprL->getType();

   VariableData::Pointer src = exprL->getData();
   VariableData::Pointer tmp = VariableData::create_stack(src->size);

   if (dst->type != VariableData::MT_VOID)
      make_objects_memcpy_prep(objects, dst, tmp, pos);

   // MT_REGISTERARRAY addressing.
   if (src->type == VariableData::MT_REGISTERARRAY)
   {
      src->offsetTemp = VariableData::create_stack
                        (src->offsetExpr->getType()->getSize(pos));
      src->offsetExpr->makeObjects(objects, src->offsetTemp);

      // Extra address for get.
      if (dst->type != VariableData::MT_VOID)
         objects->addToken(OCODE_STK_COPY);
   }

   create_value_cast_explicit(exprR, typeL, context, pos)
      ->makeObjects(objects, tmp);

   // MT_POINTER addressing.
   if (src->type == VariableData::MT_POINTER)
   {
      src->offsetTemp = VariableData::create_stack
                     (src->offsetExpr->getType()->getSize(pos));
      src->offsetExpr->makeObjects(objects, src->offsetTemp);

      // Extra address for get.
      if (dst->type != VariableData::MT_VOID)
      {
         tmpA = context->getTempVar(tmpBase++);
         objects->addToken(OCODE_SET_TEMP, tmpA);
         objects->addToken(OCODE_GET_TEMP, tmpA);
      }
   }

   // If doSet fails, need to emulate using doGet.
   if (!doSet(objects, src, typeL, tmpBase))
   {
      if (src->type == VariableData::MT_POINTER)
      {
         if (!tmpA) tmpA = context->getTempVar(tmpBase++);

         objects->addToken(OCODE_SET_TEMP, tmpA);
      }

      // However, need the operands in the right order.
      // TODO: Only for certain operators.
      ObjectExpression::Vector tmpV; tmpV.reserve(src->size);
      for (bigsint i = src->size; i--;)
         tmpV.push_back(context->getTempVar(tmpBase+i));

      // Stash exprR.
      for (bigsint i = src->size; i--;)
         objects->addToken(OCODE_SET_TEMP, tmpV[i]);

      // With exprR out of the way, RA address needs to be duplicated.
      if (src->type == VariableData::MT_REGISTERARRAY)
         objects->addToken(OCODE_STK_SWAP);

      // Fetch exprL.
      if (src->type == VariableData::MT_POINTER)
         objects->addToken(OCODE_GET_TEMP, tmpA);
      doSetBaseGet(objects, src);

      // Fetch exprR.
      for (bigsint i = 0; i < src->size; ++i)
         objects->addToken(OCODE_GET_TEMP, tmpV[i]);

      doGet(objects, typeL, tmpBase);

      if (src->type == VariableData::MT_POINTER)
         objects->addToken(OCODE_GET_TEMP, tmpA);
      doSetBaseSet(objects, src);
   }

   if (dst->type != VariableData::MT_VOID)
   {
      // MT_POINTER addressing.
      if (src->type == VariableData::MT_POINTER)
         objects->addToken(OCODE_GET_TEMP, tmpA);

      doSetBaseGet(objects, src);

      make_objects_memcpy_post(objects, dst, tmp, typeL, context, pos);
   }
}

//
// SourceExpression_Binary::doSetBaseGet
//
void SourceExpression_Binary::doSetBaseGet(ObjectVector *objects,
                                           VariableData *src)
{
   switch (src->type)
   {
   case VariableData::MT_AUTO:
      objects->addToken(OCODE_GET_AUTO, src->address); break;
   case VariableData::MT_POINTER:
      objects->addToken(OCODE_GET_PTR, src->address); break;
   case VariableData::MT_REGISTER:
      switch (src->sectionR)
      {
      case VariableData::SR_LOCAL:
         objects->addToken(OCODE_GET_REG, src->address); break;
      case VariableData::SR_MAP:
         objects->addToken(OCODE_GET_MAPREG, src->address); break;
      case VariableData::SR_WORLD:
         objects->addToken(OCODE_GET_WLDREG, src->address); break;
      case VariableData::SR_GLOBAL:
         objects->addToken(OCODE_GET_GBLREG, src->address); break;
      }
      break;
   case VariableData::MT_REGISTERARRAY:
      switch (src->sectionRA)
      {
      case VariableData::SRA_MAP:
         objects->addToken(OCODE_GET_MAPARR, src->address); break;
      case VariableData::SRA_WORLD:
         objects->addToken(OCODE_GET_WLDARR, src->address); break;
      case VariableData::SRA_GLOBAL:
         objects->addToken(OCODE_GET_GBLARR, src->address); break;
      }
      break;
   case VariableData::MT_STATIC:
      objects->addToken(OCODE_GET_STATIC, src->address); break;
   default: ERROR_NP("src->type");
   }
}

//
// SourceExpression_Binary::doSetBaseSet
//
void SourceExpression_Binary::doSetBaseSet(ObjectVector *objects,
                                           VariableData *src)
{
   switch (src->type)
   {
   case VariableData::MT_AUTO:
      objects->addToken(OCODE_SET_AUTO, src->address); break;
   case VariableData::MT_POINTER:
      objects->addToken(OCODE_SET_PTR, src->address); break;
   case VariableData::MT_REGISTER:
      switch (src->sectionR)
      {
      case VariableData::SR_LOCAL:
         objects->addToken(OCODE_SET_REG, src->address); break;
      case VariableData::SR_MAP:
         objects->addToken(OCODE_SET_MAPREG, src->address); break;
      case VariableData::SR_WORLD:
         objects->addToken(OCODE_SET_WLDREG, src->address); break;
      case VariableData::SR_GLOBAL:
         objects->addToken(OCODE_SET_GBLREG, src->address); break;
      }
      break;
   case VariableData::MT_REGISTERARRAY:
      switch (src->sectionRA)
      {
      case VariableData::SRA_MAP:
         objects->addToken(OCODE_SET_MAPARR, src->address); break;
      case VariableData::SRA_WORLD:
         objects->addToken(OCODE_SET_WLDARR, src->address); break;
      case VariableData::SRA_GLOBAL:
         objects->addToken(OCODE_SET_GBLARR, src->address); break;
      }
      break;
   case VariableData::MT_STATIC:
      objects->addToken(OCODE_SET_STATIC, src->address); break;
   default: ERROR_NP("src->type");
   }
}

//
// SourceExpression_Binary::getType
//
VariableType::Reference SourceExpression_Binary::getType() const
{
   return get_promoted_type(exprL->getType(), exprR->getType(), pos);
}

//
// SourceExpression_Binary::virtual_makeObjects
//
void SourceExpression_Binary::virtual_makeObjects(ObjectVector *objects,
                                                  VariableData *dst)
{
   Super::recurse_makeObjects(objects, dst);

   if (assign)
      doSetBase(objects, dst);
   else
      doGetBase(objects, dst);
}

// EOF

