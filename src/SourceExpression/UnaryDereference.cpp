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
#include "../SourceException.hpp"
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
   //
   // ::SourceExpression_UnaryDereference
   //
   SourceExpression_UnaryDereference(SRCEXP_EXPRUNA_PARM)
    : Super(SRCEXP_EXPRUNA_PASS)
   {
      VariableType::Reference type = expr->getType()->getReturn()->getPointer();
      expr = create_value_cast_implicit(expr, type, context, pos);
   }

   //
   // ::canGetData
   //
   virtual bool canGetData() const
   {
      if(VariableType::is_bt_function(expr->getType()->getReturn()->getBasicType()))
         return false;

      return true;
   }

   //
   // ::getData
   //
   virtual VariableData::Pointer getData() const
   {
      VariableType::Reference type = expr->getType()->getReturn();

      if(VariableType::is_bt_function(type->getBasicType()))
         return Super::getData();

      biguint            size = type->getSize(pos);
      std::string const &area = type->getStoreArea();

      ObjectExpression::Pointer address;
      SourceExpression::Pointer offset;
      VariableData::SectionR sectionR;
      VariableData::SectionA sectionA;

      switch (type->getStoreType())
      {
      case STORE_NONE:
         return VariableData::create_farptr(size, expr);

      case STORE_STATIC:
         if (expr->canMakeObject())
         {
            address = expr->makeObject();
            return VariableData::create_static(size, address);
         }
         else
         {
            address = ObjectExpression::create_value_int(0, pos);
            return VariableData::create_pointer(size, address, expr);
         }

      case STORE_AUTO:
         if (expr->canMakeObject())
         {
            address = expr->makeObject();
            return VariableData::create_auto(size, address);
         }
         else
         {
            address = ObjectExpression::create_value_int(0, pos);
            offset = SourceExpression::create_value_cast_implicit(expr,
               type->setStorage(STORE_STATIC)->getPointer(), context, pos);
            return VariableData::create_pointer(size, address, offset);
         }

      case STORE_CONST:
         ERROR_NP("STORE_CONST");

      case STORE_REGISTER:
         sectionR = VariableData::SR_LOCAL;
      case_register:
         address = expr->makeObject();
         return VariableData::create_register(size, sectionR, address);

      case STORE_MAPREGISTER:
         sectionR = VariableData::SR_MAP;
         goto case_register;

      case STORE_WORLDREGISTER:
         sectionR = VariableData::SR_WORLD;
         goto case_register;

      case STORE_GLOBALREGISTER:
         sectionR = VariableData::SR_GLOBAL;
         goto case_register;

      case STORE_MAPARRAY:
         sectionA = VariableData::SA_MAP;
      case_array:
         address = ObjectExpression::create_value_symbol(area, pos);
         return VariableData::create_array(size, sectionA, address, expr);

      case STORE_WORLDARRAY:
         sectionA = VariableData::SA_WORLD;
         goto case_array;

      case STORE_GLOBALARRAY:
         sectionA = VariableData::SA_GLOBAL;
         goto case_array;

      case STORE_STRING:
         return VariableData::create_string(size, expr);
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
   return new SourceExpression_UnaryDereference(expr, context, pos);
}

//
// SourceExpression_UnaryDereference::getType
//
VariableType::Reference SourceExpression_UnaryDereference::getType() const
{
   VariableType::Reference type = expr->getType()->getReturn();

   if (type->getStoreType() == STORE_AUTO && !expr->canMakeObject())
      return type->setStorage(STORE_STATIC);

   return type;
}

//
// SourceExpression_UnaryDereference::virtual_makeObjects
//
void SourceExpression_UnaryDereference::virtual_makeObjects
(ObjectVector *objects, VariableData *dst)
{
   Super::recurse_makeObjects(objects, dst);

   VariableType::Reference type = getType();

   if(VariableType::is_bt_function(type->getBasicType()))
      return expr->makeObjects(objects, dst);

   VariableData::Pointer src = getData();

   make_objects_memcpy_prep(objects, dst, src, pos);
   make_objects_memcpy_post(objects, dst, src, type, context, pos);
}

// EOF

