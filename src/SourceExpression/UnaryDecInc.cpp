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
#include "../print_debug.hpp"
#include "../SourceException.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"



#define DELTA_F 0
#define DELTA_I 1
#define DELTA_U 2
#define DELTA_MASK 3
#define DELTA_EXPR 4

//
// SourceExpression_UnaryDecInc
//
// Handles prefix/suffix decrement/increment.
//
class SourceExpression_UnaryDecInc : public SourceExpression_Unary
{
   MAKE_COUNTER_CLASS_BASE(SourceExpression_UnaryDecInc,
                           SourceExpression_Unary);

public:
   SourceExpression_UnaryDecInc(SourceExpression *expr, bool inc, bool suf,
                                SourcePosition const &position);

protected:
   virtual void printDebug(std::ostream *out) const;

private:
   int doDelta(ObjectVector *objects);
   void doDst(ObjectVector *objects, VariableData *dst, VariableData *src);
   void doSrc(ObjectVector *objects, VariableData *src, int deltaType);

   virtual void
   virtual_makeObjects(ObjectVector *objects, VariableData *dst);

   bool inc;
   bool suf;
};



//
// SourceExpression::create_unary_dec_pre
//
SourceExpression::Pointer SourceExpression::
create_unary_dec_pre(SourceExpression * expr, SourcePosition const & position)
{
   return new SourceExpression_UnaryDecInc(expr, false, false, position);
}

//
// SourceExpression::create_unary_dec_suf
//
SourceExpression::Pointer SourceExpression::
create_unary_dec_suf(SourceExpression * expr, SourcePosition const & position)
{
   return new SourceExpression_UnaryDecInc(expr, false, true, position);
}

//
// SourceExpression::create_unary_inc_pre
//
SourceExpression::Pointer SourceExpression::
create_unary_inc_pre(SourceExpression * expr, SourcePosition const & position)
{
   return new SourceExpression_UnaryDecInc(expr, true, false, position);
}

//
// SourceExpression::create_unary_inc_suf
//
SourceExpression::Pointer SourceExpression::
create_unary_inc_suf(SourceExpression * expr, SourcePosition const & position)
{
   return new SourceExpression_UnaryDecInc(expr, true, true, position);
}



//
// SourceExpression_UnaryDecInc::SourceExpression_UnaryDecInc
//
SourceExpression_UnaryDecInc::
SourceExpression_UnaryDecInc(SourceExpression *_expr, bool _inc, bool _suf,
                             SourcePosition const &_position)
                             : Super(_expr, _position), inc(_inc), suf(_suf)
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

   switch (src->type)
   {
   case VariableData::MT_AUTO:
      if (deltaType & DELTA_EXPR)
         ocode = inc ? OCODE_SETOP_ADD_AUTO_VAR32F
                     : OCODE_SETOP_SUB_AUTO_VAR32F;
      else
         ocode = inc ? OCODE_SETOP_INC_AUTO_VAR32F
                     : OCODE_SETOP_DEC_AUTO_VAR32F;
      break;

   case VariableData::MT_POINTER:
      if (deltaType & DELTA_EXPR)
         ocode = inc ? OCODE_SETOP_ADD_POINTER_VAR32F
                     : OCODE_SETOP_SUB_POINTER_VAR32F;
      else
         ocode = inc ? OCODE_SETOP_INC_POINTER_VAR32F
                     : OCODE_SETOP_DEC_POINTER_VAR32F;
      break;

   case VariableData::MT_REGISTER:
      switch (src->sectionR)
      {
      case VariableData::SR_LOCAL:
         if (deltaType & DELTA_EXPR)
            ocode = inc ? OCODE_SETOP_ADD_REGISTER_VAR32F
                        : OCODE_SETOP_SUB_REGISTER_VAR32F;
         else
            ocode = inc ? OCODE_SETOP_INC_REGISTER_VAR32F
                        : OCODE_SETOP_DEC_REGISTER_VAR32F;
         break;

      case VariableData::SR_MAP:
         if (deltaType & DELTA_EXPR)
            ocode = inc ? OCODE_SETOP_ADD_MAPREGISTER_VAR32F
                        : OCODE_SETOP_SUB_MAPREGISTER_VAR32F;
         else
            ocode = inc ? OCODE_SETOP_INC_MAPREGISTER_VAR32F
                        : OCODE_SETOP_DEC_MAPREGISTER_VAR32F;
         break;

      case VariableData::SR_WORLD:
         if (deltaType & DELTA_EXPR)
            ocode = inc ? OCODE_SETOP_ADD_WORLDREGISTER_VAR32F
                        : OCODE_SETOP_SUB_WORLDREGISTER_VAR32F;
         else
            ocode = inc ? OCODE_SETOP_INC_WORLDREGISTER_VAR32F
                        : OCODE_SETOP_DEC_WORLDREGISTER_VAR32F;
         break;

      case VariableData::SR_GLOBAL:
         if (deltaType & DELTA_EXPR)
            ocode = inc ? OCODE_SETOP_ADD_GLOBALREGISTER_VAR32F
                        : OCODE_SETOP_SUB_GLOBALREGISTER_VAR32F;
         else
            ocode = inc ? OCODE_SETOP_INC_GLOBALREGISTER_VAR32F
                        : OCODE_SETOP_DEC_GLOBALREGISTER_VAR32F;
         break;
      }
      break;

   case VariableData::MT_REGISTERARRAY:
      switch (src->sectionRA)
      {
      case VariableData::SRA_MAP:
         if (deltaType & DELTA_EXPR)
            ocode = inc ? OCODE_SETOP_ADD_MAPARRAY_VAR32F
                        : OCODE_SETOP_SUB_MAPARRAY_VAR32F;
         else
            ocode = inc ? OCODE_SETOP_INC_MAPARRAY_VAR32F
                        : OCODE_SETOP_DEC_MAPARRAY_VAR32F;
         break;

      case VariableData::SRA_WORLD:
         if (deltaType & DELTA_EXPR)
            ocode = inc ? OCODE_SETOP_ADD_WORLDARRAY_VAR32F
                        : OCODE_SETOP_SUB_WORLDARRAY_VAR32F;
         else
            ocode = inc ? OCODE_SETOP_INC_WORLDARRAY_VAR32F
                        : OCODE_SETOP_DEC_WORLDARRAY_VAR32F;
         break;

      case VariableData::SRA_GLOBAL:
         if (deltaType & DELTA_EXPR)
            ocode = inc ? OCODE_SETOP_ADD_GLOBALARRAY_VAR32F
                        : OCODE_SETOP_SUB_GLOBALARRAY_VAR32F;
         else
            ocode = inc ? OCODE_SETOP_INC_GLOBALARRAY_VAR32F
                        : OCODE_SETOP_DEC_GLOBALARRAY_VAR32F;
         break;
      }
      break;

   case VariableData::MT_STATIC:
      if (deltaType & DELTA_EXPR)
         ocode = inc ? OCODE_SETOP_ADD_STATIC_VAR32F
                     : OCODE_SETOP_SUB_STATIC_VAR32F;
      else
         ocode = inc ? OCODE_SETOP_INC_STATIC_VAR32F
                     : OCODE_SETOP_DEC_STATIC_VAR32F;
      break;

   case VariableData::MT_LITERAL:
   case VariableData::MT_STACK:
   case VariableData::MT_VOID:
   case VariableData::MT_NONE:
      throw SourceException("invalid MT", position, getName());
   }

   ocode = static_cast<ObjectCode>(ocode + (deltaType & DELTA_MASK));
   objects->addToken(ocode, src->address);
}

//
// SourceExpression_UnaryDecInc::printDebug
//
void SourceExpression_UnaryDecInc::printDebug(std::ostream *out) const
{
   *out << "SourceExpression_UnaryDecInc(";
   Super::printDebug(out);
      *out << "inc=(";
      print_debug(out, inc);
      *out << ")";

      *out << ", ";

      *out << "suf=(";
      print_debug(out, suf);
      *out << ")";
   *out << ")";
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
            objects->addToken(OCODE_SET_TEMP_VAR);
            objects->addToken(OCODE_GET_TEMP_VAR);
            deltaType = doDelta(objects);
            objects->addToken(OCODE_GET_TEMP_VAR);
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
         objects->addToken(OCODE_SET_TEMP_VAR);

      doDst(objects, dst, src);

      // If we couldn't do delta because stack dst, we do it now.
      if (src->type != VariableData::MT_REGISTERARRAY
       && dst->type == VariableData::MT_STACK)
            deltaType = doDelta(objects);

      if (offset && dst->type == VariableData::MT_STACK)
         objects->addToken(OCODE_GET_TEMP_VAR);
   }

   if (!deltaType)
      deltaType = doDelta(objects);

   doSrc(objects, src, deltaType);

   if (!suf && dst->type != VariableData::MT_VOID)
      doDst(objects, dst, src);
}



// EOF

