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
// SourceExpression handling of "operator ++" and "operator --".
//
//-----------------------------------------------------------------------------

#include "Unary.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../SourceException.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Macros                                                                     |
//

#define DELTA_F 0
#define DELTA_I 1
#define DELTA_U 2
#define DELTA_MASK 3
#define DELTA_EXPR 4


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// SourceExpression_UnaryDecInc
//
// Handles prefix/suffix decrement/increment.
//
class SourceExpression_UnaryDecInc : public SourceExpression_Unary
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(SourceExpression_UnaryDecInc,
                                   SourceExpression_Unary);

public:
   SourceExpression_UnaryDecInc(bool inc, bool suf, SRCEXP_EXPRUNA_ARGS);

private:
   int doDelta(ObjectVector *objects);
   void doDst(ObjectVector *objects, VariableData *dst, VariableData *src);
   void doSrc(ObjectVector *objects, VariableData *src, int deltaType);

   virtual void
   virtual_makeObjects(ObjectVector *objects, VariableData *dst);

   bool inc;
   bool suf;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_unary_dec_pre
//
SRCEXP_EXPRUNA_DEFN(dec_pre)
{
   return new SourceExpression_UnaryDecInc(false, false, expr, context,
                                           position);
}

//
// SourceExpression::create_unary_dec_suf
//
SRCEXP_EXPRUNA_DEFN(dec_suf)
{
   return new SourceExpression_UnaryDecInc(false, true, expr, context,
                                           position);
}

//
// SourceExpression::create_unary_inc_pre
//
SRCEXP_EXPRUNA_DEFN(inc_pre)
{
   return new SourceExpression_UnaryDecInc(true, false, expr, context,
                                           position);
}

//
// SourceExpression::create_unary_inc_suf
//
SRCEXP_EXPRUNA_DEFN(inc_suf)
{
   return new SourceExpression_UnaryDecInc(true, true, expr, context,
                                           position);
}

//
// SourceExpression_UnaryDecInc::SourceExpression_UnaryDecInc
//
SourceExpression_UnaryDecInc::
SourceExpression_UnaryDecInc(bool _inc, bool _suf, SRCEXP_EXPRUNA_PARM)
                             : Super(SRCEXP_EXPRUNA_PASS),
                               inc(_inc), suf(_suf)
{
}

//
// SourceExpression_UnaryDecInc::doDelta
//
int SourceExpression_UnaryDecInc::
doDelta(ObjectVector *objects)
{
   ObjectExpression::Pointer deltaExpr;
   int deltaType = 0; // OCODE offset. Order is always F I U.

   VariableType const *type = getType();

   switch (type->vt)
   {
   case VariableType::VT_CHAR:
   case VariableType::VT_INT:
      deltaType |= DELTA_I;
      break;

   case VariableType::VT_POINTER:
   {
      bigsint i = type->refType->size(position);
      if (i != 1)
      {
         deltaExpr = objects->getValue(i);
         deltaType |= DELTA_EXPR;
      }
   }
      deltaType |= DELTA_U;
      break;

   case VariableType::VT_REAL:
      deltaExpr = objects->getValue(1.0);
      deltaType |= DELTA_F|DELTA_EXPR;
      break;

   default:
      throw SourceException("invalid VT", position, getName());
   }

   if (deltaExpr)
   {
      objects->addToken(deltaType == DELTA_F ? OCODE_GET_LITERAL32F
                                             : OCODE_GET_LITERAL32I,
                        deltaExpr);
   }

   return deltaType;
}

//
// SourceExpression_UnaryDecInc::doDst
//
void SourceExpression_UnaryDecInc::
doDst(ObjectVector *objects, VariableData *dst, VariableData *src)
{
   make_objects_memcpy_prep(objects, dst, NULL, position);
   make_objects_memcpy_post(objects, dst, src, position);
}

//
// SourceExpression_UnaryDecInc::doSrc
//
void SourceExpression_UnaryDecInc::
doSrc(ObjectVector *objects, VariableData *src, int deltaType)
{
   ObjectCode ocode = OCODE_NONE;
   bool       typed = false;

   switch (src->type)
   {
   case VariableData::MT_AUTO:
      if (deltaType & DELTA_EXPR)
         ocode = inc ? OCODE_SETOP_ADD_AUTO32F
                     : OCODE_SETOP_SUB_AUTO32F;
      else
         ocode = inc ? OCODE_SETOP_INC_AUTO32F
                     : OCODE_SETOP_DEC_AUTO32F;
      typed = true;
      break;

   case VariableData::MT_POINTER:
      if (deltaType & DELTA_EXPR)
         ocode = inc ? OCODE_SETOP_ADD_POINTER32F
                     : OCODE_SETOP_SUB_POINTER32F;
      else
         ocode = inc ? OCODE_SETOP_INC_POINTER32F
                     : OCODE_SETOP_DEC_POINTER32F;
      typed = true;
      break;

   case VariableData::MT_REGISTER:
      switch (src->sectionR)
      {
      case VariableData::SR_LOCAL:
         if (deltaType & DELTA_EXPR)
            ocode = inc ? OCODE_SETOP_ADD_REGISTER32F
                        : OCODE_SETOP_SUB_REGISTER32F;
         else
            ocode = inc ? OCODE_SETOP_INC_REGISTER32F
                        : OCODE_SETOP_DEC_REGISTER32F;
         typed = true;
         break;

      case VariableData::SR_MAP:
         if (deltaType & DELTA_EXPR)
            ocode = inc ? OCODE_ACS_SETOP_ADD_MAPREGISTER
                        : OCODE_ACS_SETOP_SUB_MAPREGISTER;
         else
            ocode = inc ? OCODE_ACS_SETOP_INC_MAPREGISTER
                        : OCODE_ACS_SETOP_DEC_MAPREGISTER;
         break;

      case VariableData::SR_WORLD:
         if (deltaType & DELTA_EXPR)
            ocode = inc ? OCODE_ACS_SETOP_ADD_WORLDREGISTER
                        : OCODE_ACS_SETOP_SUB_WORLDREGISTER;
         else
            ocode = inc ? OCODE_ACS_SETOP_INC_WORLDREGISTER
                        : OCODE_ACS_SETOP_DEC_WORLDREGISTER;
         break;

      case VariableData::SR_GLOBAL:
         if (deltaType & DELTA_EXPR)
            ocode = inc ? OCODE_ACSE_SETOP_ADD_GLOBALREGISTER
                        : OCODE_ACSE_SETOP_SUB_GLOBALREGISTER;
         else
            ocode = inc ? OCODE_ACSE_SETOP_INC_GLOBALREGISTER
                        : OCODE_ACSE_SETOP_DEC_GLOBALREGISTER;
         break;
      }
      break;

   case VariableData::MT_REGISTERARRAY:
      switch (src->sectionRA)
      {
      case VariableData::SRA_MAP:
         if (deltaType & DELTA_EXPR)
            ocode = inc ? OCODE_ACSE_SETOP_ADD_MAPARRAY
                        : OCODE_ACSE_SETOP_SUB_MAPARRAY;
         else
            ocode = inc ? OCODE_ACSE_SETOP_INC_MAPARRAY
                        : OCODE_ACSE_SETOP_DEC_MAPARRAY;
         break;

      case VariableData::SRA_WORLD:
         if (deltaType & DELTA_EXPR)
            ocode = inc ? OCODE_ACSE_SETOP_ADD_WORLDARRAY
                        : OCODE_ACSE_SETOP_SUB_WORLDARRAY;
         else
            ocode = inc ? OCODE_ACSE_SETOP_INC_WORLDARRAY
                        : OCODE_ACSE_SETOP_DEC_WORLDARRAY;
         break;

      case VariableData::SRA_GLOBAL:
         if (deltaType & DELTA_EXPR)
            ocode = inc ? OCODE_ACSE_SETOP_ADD_GLOBALARRAY
                        : OCODE_ACSE_SETOP_SUB_GLOBALARRAY;
         else
            ocode = inc ? OCODE_ACSE_SETOP_INC_GLOBALARRAY
                        : OCODE_ACSE_SETOP_DEC_GLOBALARRAY;
         break;
      }
      break;

   case VariableData::MT_STATIC:
      if (deltaType & DELTA_EXPR)
         ocode = inc ? OCODE_SETOP_ADD_STATIC32F
                     : OCODE_SETOP_SUB_STATIC32F;
      else
         ocode = inc ? OCODE_SETOP_INC_STATIC32F
                     : OCODE_SETOP_DEC_STATIC32F;
      typed = true;
      break;

   case VariableData::MT_LITERAL:
   case VariableData::MT_STACK:
   case VariableData::MT_VOID:
   case VariableData::MT_NONE:
      throw SourceException("invalid MT", position, getName());
   }

   if (typed)
      ocode = static_cast<ObjectCode>(ocode + (deltaType & DELTA_MASK));
   objects->addToken(ocode, src->address);
}

//
// SourceExpression_UnaryDecInc::virtual_makeObjects
//
void SourceExpression_UnaryDecInc::
virtual_makeObjects(ObjectVector *objects, VariableData *dst)
{
   Super::recurse_makeObjects(objects, dst);

   int deltaType = 0;

   VariableData::Pointer src = expr->getData();

   bool offset = src->type == VariableData::MT_POINTER
              || src->type == VariableData::MT_REGISTERARRAY;

   // For non-registerarray sources, need to put any possible delta on the
   // stack before the src address.
   if (src->type != VariableData::MT_REGISTERARRAY)
   {
      // If void dst, then just do it now regardless.
      // Don't delta if prefix, because the extra address will be in the way.
      // Or if doing stack dst, because the result will be in the way.
      if (dst->type == VariableData::MT_VOID
       || (suf && dst->type != VariableData::MT_STACK))
         deltaType = doDelta(objects);
   }

   if (offset)
   {
      src->offsetTemp = VariableData::create_stack(
                                  src->offsetExpr->getType()->size(position) );

      src->offsetExpr->makeObjects(objects, src->offsetTemp);

      if (dst->type != VariableData::MT_VOID)
      {
         // If we couldn't do delta because prefix, we do it now.
         if (src->type != VariableData::MT_REGISTERARRAY && !suf)
         {
            objects->addToken(OCODE_SET_TEMP);
            objects->addToken(OCODE_GET_TEMP);
            deltaType = doDelta(objects);
            objects->addToken(OCODE_GET_TEMP);
         }
         else
         {
            objects->addToken(OCODE_STACK_DUP32);
         }
      }
   }

   if (suf && dst->type != VariableData::MT_VOID)
   {
      // If stashing the offset on the stack, and pushing the result to the
      // stack, need to keep the offset in front of the result.
      if (offset && dst->type == VariableData::MT_STACK)
         objects->addToken(OCODE_SET_TEMP);

      doDst(objects, dst, src);

      // If we couldn't do delta because stack dst, we do it now.
      if (src->type != VariableData::MT_REGISTERARRAY
       && dst->type == VariableData::MT_STACK)
            deltaType = doDelta(objects);

      if (offset && dst->type == VariableData::MT_STACK)
         objects->addToken(OCODE_GET_TEMP);
   }

   if (!deltaType)
      deltaType = doDelta(objects);

   doSrc(objects, src, deltaType);

   if (!suf && dst->type != VariableData::MT_VOID)
      doDst(objects, dst, src);
}

// EOF

