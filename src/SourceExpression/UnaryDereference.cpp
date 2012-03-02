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
// SourceExpression handling of "operator *".
//
//-----------------------------------------------------------------------------

#include "Unary.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../ost_type.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// SourceExpression_UnaryDereference
//
class SourceExpression_UnaryDereference : public SourceExpression_Unary
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(SourceExpression_UnaryDereference,
                                   SourceExpression_Unary);

public:
   SourceExpression_UnaryDereference(SRCEXP_EXPRUNA_ARGS);

   virtual bool canGetData() const;

   //
   // ::getData
   //
   virtual VariableData::Pointer getData() const
   {
      VariableType::Reference type = getType();
      bigsint                 size = type->getSize(position);
      std::string const      &area = type->getStoreArea();

      ObjectExpression::Pointer address;
      VariableData::SectionR sectionR;
      VariableData::SectionRA sectionRA;

      switch (type->getStoreType())
      {
      case VariableType::ST_ADDR:
         if (expr->canMakeObject())
         {
            address = expr->makeObject();
            return VariableData::create_static(size, address);
         }
         else
         {
            address = ObjectExpression::create_value_int(0, position);
            return VariableData::create_pointer(size, address, expr);
         }

      case VariableType::ST_REGISTER:
         sectionR = VariableData::SR_LOCAL;
      case_register:
         address = expr->makeObject();
         return VariableData::create_register(size, sectionR, address);

      case VariableType::ST_MAPREGISTER:
         sectionR = VariableData::SR_MAP;
         goto case_register;

      case VariableType::ST_WORLDREGISTER:
         sectionR = VariableData::SR_WORLD;
         goto case_register;

      case VariableType::ST_GLOBALREGISTER:
         sectionR = VariableData::SR_GLOBAL;
         goto case_register;

      case VariableType::ST_MAPARRAY:
         sectionRA = VariableData::SRA_MAP;
      case_array:
         address = ObjectExpression::create_value_symbol(area, position);
         return VariableData::create_registerarray
                (size, sectionRA, address, expr);

      case VariableType::ST_WORLDARRAY:
         sectionRA = VariableData::SRA_WORLD;
         goto case_array;

      case VariableType::ST_GLOBALARRAY:
         sectionRA = VariableData::SRA_GLOBAL;
         goto case_array;
      }

      return Super::getData();
   }

   virtual VariableType::Reference getType() const;

private:
   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_unary_dereference
//
SRCEXP_EXPRUNA_DEFN(dereference)
{
   return new SourceExpression_UnaryDereference(expr, context, position);
}

//
// SourceExpression_UnaryDereference::SourceExpression_UnaryDereference
//
SourceExpression_UnaryDereference::
SourceExpression_UnaryDereference(SRCEXP_EXPRUNA_PARM)
                                  : Super(SRCEXP_EXPRUNA_PASS)
{
}

//
// SourceExpression_UnaryDereference::canGetData
//
bool SourceExpression_UnaryDereference::canGetData() const
{
   return expr->getType()->getBasicType() != VariableType::BT_STRING;
}

//
// SourceExpression_UnaryDereference::getType
//
VariableType::Reference SourceExpression_UnaryDereference::getType() const
{
   return expr->getType()->getReturn();
}

//
// SourceExpression_UnaryDereference::virtual_makeObjects
//
void SourceExpression_UnaryDereference::virtual_makeObjects
(ObjectVector *objects, VariableData *dst)
{
   Super::recurse_makeObjects(objects, dst);

   VariableType::Reference type = getType();

   if (expr->getType()->getBasicType() == VariableType::BT_STRING)
   {
      VariableData::Pointer src =
         VariableData::create_stack(type->getSize(position));
      VariableData::Pointer tmp =
         VariableData::create_stack(expr->getType()->getSize(position));

      make_objects_memcpy_prep(objects, dst, src, position);

      expr->makeObjects(objects, tmp);
      objects->addTokenPushZero();
      objects->addToken(OCODE_MISC_NATIVE, objects->getValue(2), objects->getValue(15));

      make_objects_memcpy_post(objects, dst, src, type, context, position);

      return;
   }

   VariableData::Pointer src = getData();

   make_objects_memcpy_prep(objects, dst, src, position);
   make_objects_memcpy_post(objects, dst, src, type, context, position);
}

// EOF

