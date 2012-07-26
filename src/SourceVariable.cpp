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
// Source-level variable handling.
//
//-----------------------------------------------------------------------------

#include "SourceVariable.hpp"

#include "ObjectData.hpp"
#include "ObjectExpression.hpp"
#include "SourceContext.hpp"
#include "SourceException.hpp"
#include "SourceExpression.hpp"
#include "VariableData.hpp"
#include "VariableType.hpp"


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceVariable::SourceVariable
//
SourceVariable::SourceVariable(std::string const &_nameSrc, VariableType *_type,
   std::string const &_nameObj, ObjectExpression *_expr,
   SourcePosition const &_pos)
 : pos(_pos), nameObj(_nameObj), nameSrc(_nameSrc), expr(_expr), type(_type),
   store(STORE_CONST)
{
}

//
// SourceVariable::SourceVariable
//
SourceVariable::SourceVariable(std::string const &_nameSrc, VariableType *_type,
   std::string const &_nameObj, ObjectExpression *_expr, StoreType _store,
   SourcePosition const &_pos)
 : pos(_pos), nameObj(_nameObj), nameSrc(_nameSrc), expr(_expr), type(_type),
   store(_store)
{
   switch (store)
   {
   case STORE_NONE:
      ERROR_NP("far-storage variable");

   case STORE_MAPARRAY:
   case STORE_WORLDARRAY:
   case STORE_GLOBALARRAY:
      type = type->setStorage(store, nameObj);
      break;

   case STORE_STRING:
      ERROR_NP("string-storage variable");

   default:
      type = type->setStorage(store);
      break;
   }
}

//
// SourceVariable::~SourceVariable
//
SourceVariable::~SourceVariable()
{
}

//
// SourceVariable::getData
//
VariableData::Pointer SourceVariable::getData() const
{
   ObjectExpression::Pointer address;
   SourceExpression::Pointer arrbase;
   bigsint                   size = type->getSize(pos);

   if (expr)
      address = expr;
   else
      address = ObjectExpression::create_value_symbol(nameObj, pos);

   switch (store)
   {
   case STORE_NONE:
      ERROR_NP("STORE_NONE");

   case STORE_STATIC:
      return VariableData::create_static(size, address);

   case STORE_AUTO:
      return VariableData::create_auto(size, address);

   case STORE_CONST:
      return VariableData::create_literal(size, address);

   case STORE_REGISTER:
      return VariableData::create_register
         (size, VariableData::SR_LOCAL, address);

   case STORE_MAPREGISTER:
      return VariableData::create_register
         (size, VariableData::SR_MAP, address);

   case STORE_WORLDREGISTER:
      return VariableData::create_register
         (size, VariableData::SR_WORLD, address);

   case STORE_GLOBALREGISTER:
      return VariableData::create_register
         (size, VariableData::SR_GLOBAL, address);

   case STORE_MAPARRAY:
      if (ObjectData_Array::meta_map(nameObj))
         arrbase = SourceExpression::create_value_uint
            (1, SourceContext::global_context, pos);

      return VariableData::create_array(size, VariableData::SA_MAP, address, arrbase);

   case STORE_WORLDARRAY:
      if (ObjectData_Array::meta_world(nameObj))
         arrbase = SourceExpression::create_value_uint
            (1, SourceContext::global_context, pos);

      return VariableData::create_array(size, VariableData::SA_WORLD, address, arrbase);

   case STORE_GLOBALARRAY:
      if (ObjectData_Array::meta_global(nameObj))
         arrbase = SourceExpression::create_value_uint
            (1, SourceContext::global_context, pos);

      return VariableData::create_array(size, VariableData::SA_GLOBAL, address, arrbase);

   case STORE_STRING:
      ERROR_NP("STORE_STRING");
   }

   ERROR_NP("invalid store");
}

// EOF

