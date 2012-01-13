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



//
// SourceExpression_ValueMember
//
class SourceExpression_ValueMember : public SourceExpression
{
   MAKE_COUNTER_CLASS_BASE(SourceExpression_ValueMember, SourceExpression);

public:
   SourceExpression_ValueMember(SourceExpression *expr,
                                SourceTokenC const &token);

   virtual bool canGetData() const;

   virtual VariableData::Pointer getData() const;

   virtual VariableType const *getType() const;

private:
   SourceExpression::Pointer expr;
   std::string name;
};



//
// SourceExpression::create_value_member
//
SourceExpression::Pointer SourceExpression::
create_value_member(SourceExpression *expr, SourceTokenC const &token)
{
   return new SourceExpression_ValueMember(expr, token);
}

//
// SourceExpression_ValueMember::SourceExpression_ValueMember
//
SourceExpression_ValueMember::
SourceExpression_ValueMember(SourceExpression *_expr, SourceTokenC const &token)
                             : Super(token.getPosition()), expr(_expr),
                               name(token.getData())
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

   VariableData::Pointer src     = expr->getData();
   VariableType const   *srcType = expr->getType();

   if (!src->address)
      throw SourceException("cannot getData", position, getName());

   // Member data.
   bigsint             memberOffset = srcType->getOffset(name, position);
   VariableType const *memberType   = srcType->getType(name, position);
   bigsint             memberSize   = memberType->size(position);

   if (src->type == VariableData::MT_REGISTERARRAY)
   {
      SourceExpression::Pointer memberOffsetExpr =
         create_value_int(memberOffset, position);

      SourceExpression::Pointer offset =
         create_binary_add(src->offsetExpr, memberOffsetExpr, position);

      return VariableData::create_registerarray(memberSize, src->sectionRA,
                                                src->address, offset);
   }

   ObjectExpression::Pointer memberOffsetObj =
      ObjectExpression::create_value_int(memberOffset, position);

   ObjectExpression::Pointer address =
      ObjectExpression::create_binary_add(src->address, memberOffsetObj,
                                          position);

   switch (src->type)
   {
   case VariableData::MT_AUTO:
      return VariableData::create_auto(memberSize, address);

   case VariableData::MT_POINTER:
      return VariableData::create_pointer(memberSize, address, src->offsetExpr);

   case VariableData::MT_REGISTER:
      return VariableData::create_register(memberSize, src->sectionR, address);

   case VariableData::MT_STATIC:
      return VariableData::create_static(memberSize, address);

   default:
      throw SourceException("cannot getData", position, getName());
   }

   return data;
}

//
// SourceExpression_ValueMember::getType
//
VariableType const *SourceExpression_ValueMember::getType() const
{
   return expr->getType()->getType(name, position);
}

// EOF

