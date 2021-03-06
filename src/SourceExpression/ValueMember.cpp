//-----------------------------------------------------------------------------
//
// Copyright(C) 2011-2013 David Hill
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
// SourceExpression handling of "operator .".
//
//-----------------------------------------------------------------------------

#include "../SourceExpression.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../SourceException.hpp"
#include "../SourceTokenC.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// SourceExpression_ValueMember
//
class SourceExpression_ValueMember : public SourceExpression
{
   CounterPreambleNoClone(SourceExpression_ValueMember, SourceExpression);

public:
   SourceExpression_ValueMember(SourceExpression *expr,
                                std::string const &value, SRCEXP_EXPR_ARGS);

   virtual bool canGetData() const;

   virtual VariableData::Pointer getData() const;

   virtual VariableType::Reference getType() const;

   //
   // isSideEffect
   //
   virtual bool isSideEffect() const
   {
      return expr->isSideEffect();
   }

private:
   SourceExpression::Pointer expr;
   std::string name;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_value_member
//
SRCEXP_EXPRVAL_DEFN(es, member)
{
   return new SourceExpression_ValueMember(expr, value, context, pos);
}

//
// SourceExpression_ValueMember::SourceExpression_ValueMember
//
SourceExpression_ValueMember::
SourceExpression_ValueMember(SourceExpression *_expr, std::string const &value,
                             SRCEXP_EXPR_PARM)
                             : Super(SRCEXP_EXPR_PASS),
                               expr(_expr), name(value)
{
}

//
// SourceExpression_ValueMember::canGetData
//
bool SourceExpression_ValueMember::canGetData() const
{
   return true;
}

//
// SourceExpression_ValueMember::getData
//
VariableData::Pointer SourceExpression_ValueMember::getData() const
{
   VariableData::Pointer data;

   VariableData::Pointer   src     = expr->getData();
   VariableType::Reference srcType = expr->getType();

   if (!src->address)
      Error_NP("cannot getData");

   // Member data.
   bigsint                 memberOffset = srcType->getOffset(name, pos);
   VariableType::Reference memberType   = srcType->getType(name, pos);
   bigsint                 memberSize   = memberType->getSize(pos);

   // Need special handling for array storage, since they don't use
   // address for an offset.
   if(src->type == VariableData::MT_ARRAY)
   {
      VariableType::Reference ptrType = VariableType::get_bt_chr()->setStorage(srcType)->getPointer();

      SourceExpression::Pointer offset;

      if(memberOffset) offset = create_value_uint(memberOffset, context, pos);

      if(src->offsetExpr)
      {
         SourceExpression::Pointer base = create_value_cast_explicit(src->offsetExpr,
            ptrType, context, pos);

         offset = offset ? create_binary_add(base, offset, context, pos) : base;
      }

      if(offset)
         offset = create_value_cast_explicit(offset, memberType->getPointer(), context, pos);

      return VariableData::create_array(memberSize, src->sectionA, src->address, offset);
   }

   if(src->type == VariableData::MT_LITERAL)
      return VariableData::create_literal(memberSize, src->address->resolveMAP(name));

   ObjectExpression::Pointer memberOffsetObj =
      ObjectExpression::CreateValueUNS(memberOffset, pos);

   ObjectExpression::Pointer address =
      ObjectExpression::create_binary_add(src->address, memberOffsetObj, pos);

   switch (src->type)
   {
   case VariableData::MT_AUTO:
      return VariableData::create_auto(memberSize, address);

   case VariableData::MT_FARPTR:
      return VariableData::create_farptr(memberSize, address, src->offsetExpr);

   case VariableData::MT_POINTER:
      return VariableData::create_pointer(memberSize, address, src->offsetExpr);

   case VariableData::MT_REGISTER:
      return VariableData::create_register(memberSize, src->sectionR, address);

   case VariableData::MT_STATIC:
      return VariableData::create_static(memberSize, address);

   default:
      Error_NP("cannot getData");
   }

   return data;
}

//
// SourceExpression_ValueMember::getType
//
VariableType::Reference SourceExpression_ValueMember::getType() const
{
   return expr->getType()->getType(name, pos);
}

// EOF

