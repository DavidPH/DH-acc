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
// Base class for source-level expression handling.
//
//-----------------------------------------------------------------------------

#include "SourceExpression.hpp"

#include "ObjectExpression.hpp"
#include "ObjectVector.hpp"
#include "option.hpp"
#include "SourceContext.hpp"
#include "SourceException.hpp"
#include "VariableData.hpp"
#include "VariableType.hpp"


//----------------------------------------------------------------------------|
// Static Variables                                                           |
//

option::option_dptr<int> option_auto_array_handler
('\0', "auto-array", "features",
 "Selects which world array to use for automatic variable stack allocations. "
 "Also used for other miscellaneous features even if auto-stack-part is 0. 0 "
 "by default.", NULL, &option_auto_array);

option::option_dptr<int> option_auto_stack_part_handler
('\0', "auto-stack-size", "features",
 "Sets the size of each thread's automatic variable stack. Use 0 to disable "
 "per-thread stack pointers. 8192 by default.", NULL, &option_auto_stack_size);


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

int option_auto_array = 0;
int option_auto_stack_size = 8192;


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::SourceExpression
//
SourceExpression::SourceExpression(SRCEXP_EXPR_PARM)
 : pos(_pos), context(_context), evaluated(false)
{
}

//
// SourceExpression::~SourceExpression
//
SourceExpression::~SourceExpression()
{
}

//
// SourceExpression::addLabel
//
void SourceExpression::addLabel(std::string const &_label)
{
   labels.push_back(_label);
}

//
// SourceExpression::canGetData
//
bool SourceExpression::canGetData() const
{
   return false;
}

//
// SourceExpression::canMakeObject
//
bool SourceExpression::canMakeObject() const
{
   if (!canGetData()) return false;

   VariableData::Pointer data = getData();

   if (data->type != VariableData::MT_LITERAL)
      return false;

   return true;
}

//
// SourceExpression::get_promoted_type
//
VariableType::Reference SourceExpression::
get_promoted_type(VariableType *_type1, VariableType *_type2,
                  SourcePosition const &)
{
   VariableType::Reference type1(_type1);
   VariableType::Reference type2(_type2);

   VariableType::BasicType bt1 = type1->getBasicType();
   VariableType::BasicType bt2 = type2->getBasicType();

   // Arrays have special promotion.
   if (bt1 == VariableType::BT_ARR && bt2 == VariableType::BT_ARR)
   {
      if (type1 == type2) return type1;

      return VariableType::get_bt_void();
   }

   // Decay arrays to pointers and de-qualify everything else.
   if (bt1 == VariableType::BT_ARR)
   {
      type1 = type1->getReturn()->getPointer();
      bt1 = VariableType::BT_PTR;
   }
   else
      type1 = type1->getUnqualified();

   if (bt2 == VariableType::BT_ARR)
   {
      type2 = type2->getReturn()->getPointer();
      bt2 = VariableType::BT_PTR;
   }
   else
      type2 = type2->getUnqualified();

   // nullptr should always try to promote to the other type.
   if (bt1 == VariableType::BT_PTR_NUL) return type2;
   if (bt2 == VariableType::BT_PTR_NUL) return type1;

   // Pointers have special promotion.
   if (bt1 == VariableType::BT_PTR && bt2 == VariableType::BT_PTR)
   {
      if (type1 == type2) return type1;

      VariableType::Reference retn1 = type1->getReturn();
      VariableType::Reference retn2 = type2->getReturn();

      // If one is more qualified than the other, use it.
      if (retn1->getQualifiers())
      {
         if (!retn2->getQualifiers())
            return type1->getPointer();

         if (!retn2->getQualifier(retn1->getQualifiers()))
            return type1;

         return type2;
      }
      else if (retn2->getQualifiers())
         return type2;

      // Neither has qualifiers, so go by the basic types.
      bt1 = retn1->getBasicType();
      bt2 = retn2->getBasicType();

      // Promote to void.
      if (bt1 == VariableType::BT_VOID) return type1;
      if (bt2 == VariableType::BT_VOID) return type2;

      // Nothing seems better, so just return the left-hand type.
      return type1;
   }

   if (bt1 == VariableType::BT_VOID) return type1;
   if (bt2 == VariableType::BT_VOID) return type2;

   if (bt1 == VariableType::BT_PTR) return type1;
   if (bt2 == VariableType::BT_PTR) return type2;

   if (bt1 == VariableType::BT_FLT_LL) return type1;
   if (bt2 == VariableType::BT_FLT_LL) return type2;

   if (bt1 == VariableType::BT_FLT_L) return type1;
   if (bt2 == VariableType::BT_FLT_L) return type2;

   if (bt1 == VariableType::BT_FLT) return type1;
   if (bt2 == VariableType::BT_FLT) return type2;

   if (bt1 == VariableType::BT_FLT_H) return type1;
   if (bt2 == VariableType::BT_FLT_H) return type2;

   if (bt1 == VariableType::BT_FLT_HH) return type1;
   if (bt2 == VariableType::BT_FLT_HH) return type2;

   if (bt1 == VariableType::BT_FIX_LL) return type1;
   if (bt2 == VariableType::BT_FIX_LL) return type2;

   if (bt1 == VariableType::BT_FIX_L) return type1;
   if (bt2 == VariableType::BT_FIX_L) return type2;

   if (bt1 == VariableType::BT_FIX) return type1;
   if (bt2 == VariableType::BT_FIX) return type2;

   if (bt1 == VariableType::BT_FIX_H) return type1;
   if (bt2 == VariableType::BT_FIX_H) return type2;

   if (bt1 == VariableType::BT_FIX_HH) return type1;
   if (bt2 == VariableType::BT_FIX_HH) return type2;

   if (bt1 == VariableType::BT_INT_LL) return type1;
   if (bt2 == VariableType::BT_INT_LL) return type2;

   if (bt1 == VariableType::BT_UNS_LL) return type1;
   if (bt2 == VariableType::BT_UNS_LL) return type2;

   if (bt1 == VariableType::BT_INT_L) return type1;
   if (bt2 == VariableType::BT_INT_L) return type2;

   if (bt1 == VariableType::BT_UNS_L) return type1;
   if (bt2 == VariableType::BT_UNS_L) return type2;

   if (bt1 == VariableType::BT_INT) return type1;
   if (bt2 == VariableType::BT_INT) return type2;

   if (bt1 == VariableType::BT_UNS) return type1;
   if (bt2 == VariableType::BT_UNS) return type2;

   if (bt1 == VariableType::BT_INT_H) return type1;
   if (bt2 == VariableType::BT_INT_H) return type2;

   if (bt1 == VariableType::BT_UNS_H) return type1;
   if (bt2 == VariableType::BT_UNS_H) return type2;

   if (bt1 == VariableType::BT_INT_HH) return type1;
   if (bt2 == VariableType::BT_INT_HH) return type2;

   if (bt1 == VariableType::BT_UNS_HH) return type1;
   if (bt2 == VariableType::BT_UNS_HH) return type2;

   if (bt1 == VariableType::BT_CHR) return type1;
   if (bt2 == VariableType::BT_CHR) return type2;

   if (bt1 == VariableType::BT_ENUM) return VariableType::get_bt_int();
   if (bt2 == VariableType::BT_ENUM) return VariableType::get_bt_int();

   if (bt1 == VariableType::BT_STR) return type1;
   if (bt2 == VariableType::BT_STR) return type2;

   if (bt1 == VariableType::BT_BIT_HRD) return type1;
   if (bt2 == VariableType::BT_BIT_HRD) return type2;

   if (bt1 == VariableType::BT_BIT_SFT) return type1;
   if (bt2 == VariableType::BT_BIT_SFT) return type2;

   return VariableType::get_bt_void();
}

//
// SourceExpression::getData
//
VariableData::Pointer SourceExpression::getData() const
{
   ERROR_NP("expected lvalue");
}

//
// SourceExpression::getFunction
//
CounterReference<SourceFunction> SourceExpression::getFunction() const
{
   ERROR_NP("expected function designator");
}

//
// SourceExpression::getType
//
VariableType::Reference SourceExpression::getType() const
{
   return VariableType::get_bt_void();
}

//
// SourceExpression::make_objects_auto_alloc
//
void SourceExpression::make_objects_auto_alloc(ObjectVector *objects, SourceContext *context)
{
   std::string label = context->makeLabel();

   if(option_auto_stack_size)
   {
      // One past the end of the auto-pointer allocation map. This is the
      // address that points into that map to indicate where to start searching.
      bigsint stackEnd = (((1 << 31) / option_auto_stack_size) + 31) / 32;

      std::string labelItr1   = label + "_itr1";
      std::string labelItr2   = label + "_itr2";
      std::string labelStart1 = label + "_start1";

      ObjectExpression::Pointer jumpItr1   = objects->getValue(labelItr1);
      ObjectExpression::Pointer jumpItr2   = objects->getValue(labelItr2);
      ObjectExpression::Pointer jumpStart1 = objects->getValue(labelStart1);

      ObjectExpression::Pointer slotFul = objects->getValue(0xFFFFFFFF);

      ObjectExpression::Pointer autoArr = objects->getValue(option_auto_array);
      ObjectExpression::Pointer autoStk = objects->getValue(stackEnd);
      ObjectExpression::Pointer autoItr = context->getTempVar(0);

      // Load the initial search index.
      objects->addToken(OCODE_GET_IMM,    autoStk);
      objects->addToken(OCODE_GET_WLDARR, autoArr);
      objects->addToken(OCODE_SET_TEMP,   autoItr);
      objects->addToken(OCODE_JMP_IMM,    jumpStart1);

      // Find a free slot. Each bit is a slot.
      objects->addLabel(labelItr1);
      objects->addToken(OCODE_INC_TEMP_U, autoItr);
      objects->addLabel(labelStart1);
      objects->addToken(OCODE_GET_TEMP,   autoItr);
      objects->addToken(OCODE_GET_WLDARR, autoArr);
      objects->addToken(OCODE_JMP_VAL,    slotFul, jumpItr1);
      objects->addToken(OCODE_STK_DROP);

      // Found free slot, set the iterator for next time.
      objects->addToken(OCODE_GET_IMM,    autoStk);
      objects->addToken(OCODE_GET_TEMP,   autoItr);
      objects->addToken(OCODE_SET_WLDARR, autoArr);

      // Stash temp in stack-pointer.
      // stackPtr = autoItr;
      objects->addToken(OCODE_GET_TEMP,   autoItr);
      objects->addToken(OCODE_SET_AUTPTR);

      // Find which slot is free.
      // for(autoItr = -1; autoArr[stackPtr] & (1 << ++autoItr););
      objects->addToken(OCODE_GET_IMM,    objects->getValue(-1));
      objects->addToken(OCODE_SET_TEMP,   autoItr);
      objects->addLabel(labelItr2);
      objects->addToken(OCODE_INC_TEMP_U, autoItr);
      objects->addToken(OCODE_GET_IMM,    objects->getValue(1));
      objects->addToken(OCODE_GET_TEMP,   autoItr);
      objects->addToken(OCODE_LSH_STK_U);
      objects->addToken(OCODE_GET_AUTPTR);
      objects->addToken(OCODE_GET_WLDARR, autoArr);
      objects->addToken(OCODE_AND_STK_U);
      objects->addToken(OCODE_JMP_TRU,    jumpItr2);

      // Mark slot allocated.
      // autoArr[stackPtr] |= (1 << autoItr);
      objects->addToken(OCODE_GET_AUTPTR);
      objects->addToken(OCODE_GET_IMM,      objects->getValue(1));
      objects->addToken(OCODE_GET_TEMP,     autoItr);
      objects->addToken(OCODE_LSH_STK_U);
      objects->addToken(OCODE_IOR_WLDARR_U, autoArr);

      // Found which slot, set stack-pointer.
      // stackPtr = ((stackPtr * 32) + autoItr) * auto_stack_size + 0x80000000;
      objects->addToken(OCODE_GET_AUTPTR);
      objects->addToken(OCODE_GET_IMM,    objects->getValue(5));
      objects->addToken(OCODE_LSH_STK_U);
      objects->addToken(OCODE_GET_TEMP,   autoItr);
      objects->addToken(OCODE_ADD_STK_U);
      objects->addToken(OCODE_GET_IMM,    objects->getValue(option_auto_stack_size));
      objects->addToken(OCODE_MUL_STK_U);
      objects->addToken(OCODE_GET_IMM,    objects->getValue(0x80000000));
      objects->addToken(OCODE_ADD_STK_U);
      objects->addToken(OCODE_SET_AUTPTR);
   }
   else
   {
      // Just make sure the stack pointer isn't null.
      objects->addToken(OCODE_GET_AUTPTR);
      objects->addToken(OCODE_JMP_TRU, objects->getValue(label));
      objects->addToken(OCODE_ADD_AUTPTR_IMM, objects->getValue(0x80000000));
      objects->addLabel(label);
   }
}

//
// SourceExpression::make_objects_auto_free
//
void SourceExpression::make_objects_auto_free(ObjectVector *objects, SourceContext *context)
{
   std::string label = context->makeLabel();

   if(option_auto_stack_size)
   {
      // One past the end of the auto-pointer allocation map. This is the
      // address that points into that map to indicate where to start searching.
      bigsint stackEnd = (((1 << 31) / option_auto_stack_size) + 31) / 32;

      std::string labelEnd = label + "_end";

      ObjectExpression::Pointer jumpEnd = objects->getValue(labelEnd);

      ObjectExpression::Pointer autoArr = objects->getValue(option_auto_array);
      ObjectExpression::Pointer autoStk = objects->getValue(stackEnd);
      ObjectExpression::Pointer autoItr = context->getTempVar(0);

      // Set stackPtr to the index into autoArr. (But save the bit number in autoItr.)
      // stackPtr = (autoItr = (stackPtr - 0x80000000) / auto_stack_size) / 32;
      objects->addToken(OCODE_GET_AUTPTR);
      objects->addToken(OCODE_GET_IMM,    objects->getValue(0x80000000));
      objects->addToken(OCODE_SUB_STK_U);
      objects->addToken(OCODE_GET_IMM,    objects->getValue(option_auto_stack_size));
      objects->addToken(OCODE_DIV_STK_I);
      objects->addToken(OCODE_SET_TEMP,   autoItr);
      objects->addToken(OCODE_GET_TEMP,   autoItr);
      objects->addToken(OCODE_GET_IMM,    objects->getValue(5));
      objects->addToken(OCODE_RSH_STK_I);
      objects->addToken(OCODE_SET_AUTPTR);

      // autoArr[stackPtr] &= ~(1 << (autoItr % 32));
      objects->addToken(OCODE_GET_AUTPTR);
      objects->addToken(OCODE_GET_IMM,      objects->getValue(1));
      objects->addToken(OCODE_GET_TEMP,     autoItr);
      objects->addToken(OCODE_GET_IMM,      objects->getValue(31));
      objects->addToken(OCODE_AND_STK_U);
      objects->addToken(OCODE_LSH_STK_U);
      objects->addToken(OCODE_INV_STK_U);
      objects->addToken(OCODE_AND_WLDARR_U, autoArr);

      // if(stackPtr < autoArr[autoStk]) autoArr[autoStk] = stackPtr;
      objects->addToken(OCODE_GET_AUTPTR);
      objects->addToken(OCODE_GET_IMM,    autoStk);
      objects->addToken(OCODE_GET_WLDARR, autoArr);
      objects->addToken(OCODE_CMP_LT_I);
      objects->addToken(OCODE_JMP_NIL,    jumpEnd);
      objects->addToken(OCODE_GET_IMM,    autoStk);
      objects->addToken(OCODE_GET_AUTPTR);
      objects->addToken(OCODE_SET_WLDARR, autoArr);
      objects->addLabel(labelEnd);
   }
   else
   {
      // Do nothing.
   }
}

//
// SourceExpression::make_objects_auto_load
//
void SourceExpression::make_objects_auto_load(ObjectVector *objects, SourceContext *context)
{
   std::string label = context->makeLabel();

   if(option_auto_stack_size)
   {
      std::string labelAlloc = label + "_allloc";
      std::string labelEnd   = label + "_end";

      ObjectExpression::Pointer jumpAlloc = objects->getValue(labelAlloc);
      ObjectExpression::Pointer jumpEnd   = objects->getValue(labelEnd);

      ObjectExpression::Pointer autoSav = context->getTempVar(0);

      // If auto stack was unused, re-alloc.
      // Of course, if this context is using auto variables, then it's not unsued.
      if(!context->getLimit(STORE_AUTO))
      {
         // If we're in a script, then we just proved statically that the stack
         // allocation was unused, so just alloc it.
         if(context->getTypeRoot() == SourceContext::CT_SCRIPT)
         {
            make_objects_auto_alloc(objects, context);
         }
         // Otherwise, this has to be a run-time check.
         else
         {
            // If auto-pointer is marked as free, alloc it.
            objects->addToken(OCODE_GET_TEMP, autoSav);
            objects->addToken(OCODE_JMP_VAL,  objects->getValue(0xFFFFFFFF), jumpAlloc);
            // Otherwise, load it.
            objects->addToken(OCODE_SET_AUTPTR);
            objects->addToken(OCODE_JMP_IMM,  jumpEnd);

            // Free the allocation.
            objects->addLabel(labelAlloc);
            make_objects_auto_alloc(objects, context);
         }
      }
      // If it's known to be used in this context, then just load the pointer.
      else
      {
         objects->addToken(OCODE_GET_TEMP, autoSav);
         objects->addToken(OCODE_SET_AUTPTR);
      }

      objects->addLabel(labelEnd);
   }
   else
   {
      // Not worth trying to save it.
   }
}

//
// SourceExpression::make_objects_auto_save
//
void SourceExpression::make_objects_auto_save(ObjectVector *objects, SourceContext *context)
{
   std::string label = context->makeLabel();

   if(option_auto_stack_size)
   {
      std::string labelEnd = label + "_end";
      std::string labelRem = label + "_rem";

      ObjectExpression::Pointer jumpEnd = objects->getValue(labelEnd);
      ObjectExpression::Pointer jumpRem = objects->getValue(labelRem);

      ObjectExpression::Pointer autoSav = context->getTempVar(0);

      // If auto stack is unused, free it and re-alloc in load.
      // Of course, if this context is using auto variables, then it's not unsued.
      if(!context->getLimit(STORE_AUTO))
      {
         // If we're in a script, then we just proved statically that the stack
         // allocation is unused, so just free it.
         if(context->getTypeRoot() == SourceContext::CT_SCRIPT)
         {
            make_objects_auto_free(objects, context);
         }
         // Otherwise, this has to be a run-time check.
         else
         {
            // If auto-pointer is at its initial position, free it.
            objects->addToken(OCODE_GET_AUTPTR);
            objects->addToken(OCODE_GET_IMM, objects->getValue(option_auto_stack_size));
            objects->addToken(OCODE_MOD_STK_I);
            objects->addToken(OCODE_JMP_NIL, jumpRem);
            // Otherwise, save it.
            objects->addToken(OCODE_GET_AUTPTR);
            objects->addToken(OCODE_SET_TEMP, autoSav);
            objects->addToken(OCODE_JMP_IMM,  jumpEnd);

            // Free the allocation.
            objects->addLabel(labelRem);
            make_objects_auto_free(objects, context);
         }

         // Mark temp as deallocated.
         objects->addToken(OCODE_GET_IMM,  objects->getValue(0xFFFFFFFF));
         objects->addToken(OCODE_SET_TEMP, autoSav);
      }
      // If it's known to be used in this context, then just save the pointer.
      else
      {
         objects->addToken(OCODE_GET_AUTPTR);
         objects->addToken(OCODE_SET_TEMP, autoSav);
      }

      objects->addLabel(labelEnd);
   }
   else
   {
      // Not worth trying to save it.
   }
}

//
// SourceExpression::makeExpressionFunction
//
SourceExpression::Pointer SourceExpression::makeExpressionFunction(
   VariableType::Vector const &)
{
   VariableType::Reference type = getType();

   if(VariableType::is_bt_function(type->getBasicType()))
      return this;

   ERROR_NP("expected function designator");
}

//
// SourceExpression::makeObject
//
ObjectExpression::Pointer SourceExpression::makeObject() const
{
   VariableData::Pointer src = getData();

   if(src->type != VariableData::MT_LITERAL)
      ERROR_NP("expected constant");

   return src->address;
}

//
// SourceExpression::makeObjects
//
void SourceExpression::
makeObjects(ObjectVector *objects, VariableData *dst)
{
   makeObjectsBase(objects, dst);

   if (canMakeObject())
   {
      recurse_makeObjects(objects, dst);

      VariableType::Reference type = getType();

      // Don't try to makeObject on a void expression.
      if (type->getBasicType() == VariableType::BT_VOID) return;

      VariableData::Pointer src =
         VariableData::create_literal(type->getSize(pos), makeObject());

      make_objects_memcpy_prep(objects, dst, src, pos);
      make_objects_memcpy_post(objects, dst, src, type, context, pos);
   }
   else
      virtual_makeObjects(objects, dst);
}

//
// SourceExpression::makeObjectsBase
//
void SourceExpression::makeObjectsBase(ObjectVector *objects, VariableData *)
{
   if(!labels.empty())
   {
      if(evaluated)
         ERROR_NP("multiple-evaluation of labeled expression");

      objects->addLabel(labels);
   }

   evaluated = true;
}

//
// SourceExpression::makeObjectsCast
//
void SourceExpression::makeObjectsCast
(ObjectVector *objects, VariableData *dst, VariableType *type)
{
   // If casting to void, just use a void destination for better codegen.
   if (type->getBasicType() == VariableType::BT_VOID && !dst->offsetTemp)
   {
      makeObjects(objects, VariableData::create_void(dst->size));

      // If there's an offset temp, it needs to be dealt with. (Well, only if
      // it's on the stack, but this ensures anything is handled.)
      if (dst->offsetTemp)
         make_objects_memcpy_void(objects, dst->offsetTemp, pos);
   }
   else
   {
      makeObjectsBase(objects, dst);

      virtual_makeObjectsCast(objects, dst, type);
   }
}

//
// SourceExpression::recurse_makeObjects
//
void SourceExpression::
recurse_makeObjects(ObjectVector *objects, VariableData *dst)
{
   recurse_makeObjectsBase(objects, dst);
}

//
// SourceExpression::recurse_makeObjectsBase
//
void SourceExpression::
recurse_makeObjectsBase(ObjectVector *objects, VariableData *)
{
   objects->setPosition(pos);
}

//
// SourceExpression::recurse_makeObjectsCast
//
void SourceExpression::recurse_makeObjectsCast
(ObjectVector *objects, VariableData *dst, VariableType *)
{
   recurse_makeObjectsBase(objects, dst);
}

//
// SourceExpression::virtual_makeObjects
//
void SourceExpression::
virtual_makeObjects(ObjectVector *objects, VariableData *dst)
{
   recurse_makeObjects(objects, dst);

   VariableData::Pointer   src  = getData();
   VariableType::Reference type = getType();

   make_objects_memcpy_prep(objects, dst, src, pos);
   make_objects_memcpy_post(objects, dst, src, type, context, pos);
}

//
// SourceExpression::virtual_makeObjectsCast
//
void SourceExpression::virtual_makeObjectsCast
(ObjectVector * objects, VariableData *dst, VariableType *dstType)
{
   VariableType::Reference srcType = getType();

   if (canGetData())
   {
      VariableData::Pointer src = getData();

      make_objects_memcpy_prep(objects, dst, src, pos);
      make_objects_memcpy_cast
         (objects, dst, src, dstType, srcType, context, pos);
   }
   else
   {
      VariableData::Pointer src =
         VariableData::create_stack(srcType->getSize(pos));

      make_objects_memcpy_prep(objects, dst, src, pos);
      virtual_makeObjects(objects, src);
      make_objects_memcpy_cast
         (objects, dst, src, dstType, srcType, context, pos);
   }
}

// EOF

