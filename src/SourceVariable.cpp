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

#include "ObjectExpression.hpp"
#include "ost_type.hpp"
#include "SourceException.hpp"
#include "VariableData.hpp"
#include "VariableType.hpp"


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

extern bool option_string_tag;


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceVariable::SourceVariable
//
SourceVariable::SourceVariable
(std::string const &_nameSource, VariableType *_type, ObjectExpression *_expr,
 SourcePosition const &_position)
 : position(_position), nameSource(_nameSource), expr(_expr), type(_type),
   sc(SC_CONSTANT)
{
}

//
// SourceVariable::SourceVariable
//
SourceVariable::SourceVariable
(std::string const &_nameSource, VariableType *_type,
 std::string const &_nameObject, SourcePosition const &_position)
 : position(_position), nameObject(_nameObject), nameSource(_nameSource),
   type(_type), sc(SC_CONSTANT)
{
}

//
// ourceVariable::SourceVariable
//
SourceVariable::SourceVariable
(std::string const &_nameSource, VariableType *_type,
 std::string const &_nameObject, StorageClass _sc,
 SourcePosition const &_position)
 : position(_position), nameObject(_nameObject), nameSource(_nameSource),
   type(_type), sc(_sc)
{
   switch (sc)
   {
   case SC_AUTO:
   case SC_CONSTANT:
   case SC_STATIC:
      break;

   case SC_REGISTER:
      type = type->setStorage(VariableType::ST_REGISTER);
      break;

   case SC_REGISTER_GLOBAL:
      type = type->setStorage(VariableType::ST_GLOBALREGISTER);
      break;

   case SC_REGISTER_MAP:
      type = type->setStorage(VariableType::ST_MAPREGISTER);
      break;

   case SC_REGISTER_WORLD:
      type = type->setStorage(VariableType::ST_WORLDREGISTER);
      break;

   case SC_REGISTERARRAY_GLOBAL:
      type = type->setStorage(VariableType::ST_GLOBALARRAY, nameObject);
      break;

   case SC_REGISTERARRAY_MAP:
      type = type->setStorage(VariableType::ST_MAPARRAY, nameObject);
      break;

   case SC_REGISTERARRAY_WORLD:
      type = type->setStorage(VariableType::ST_WORLDARRAY, nameObject);
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
// SourceVariable::create_constant
//
SourceVariable::Pointer SourceVariable::create_constant
(std::string const &nameSource, VariableType *type, ObjectExpression *expr,
 SourcePosition const &position)
{
   return new SourceVariable(nameSource, type, expr, position);
}

//
// SourceVariable::create_constant
//
SourceVariable::Pointer SourceVariable::create_constant
(std::string const &nameSource, VariableType *type,
 std::string const &nameObject, SourcePosition const &position)
{
   return new SourceVariable(nameSource, type, nameObject, position);
}

//
// SourceVariable::create_literal
//
SourceVariable::Pointer SourceVariable::create_literal
(VariableType *type, ObjectExpression *expr, SourcePosition const &position)
{
   return new SourceVariable("", type, expr, position);
}

//
// SourceVariable::create_literal
//
SourceVariable::Pointer SourceVariable::create_literal
(VariableType *type, std::string const &nameObject,
 SourcePosition const &position)
{
   return new SourceVariable("", type, nameObject, position);
}

//
// SourceVariable::create_variable
//
SourceVariable::Pointer SourceVariable::create_variable
(std::string const &nameSource, VariableType *type,
 std::string const &nameObject, StorageClass sc, SourcePosition const &position)
{
   return new SourceVariable(nameSource, type, nameObject, sc, position);
}

//
// SourceVariable::get_StorageClass
//
SourceVariable::StorageClass SourceVariable::get_StorageClass
(std::string const &data, SourcePosition const &pos)
{
   if (data == "auto")
      return SC_AUTO;

   if (data == "register")
      return SC_REGISTER;

   if (data == "static")
      return SC_STATIC;

   if (data == "__autoreg")
      return get_sc_autoreg();

   if (data == "__staticreg")
      return get_sc_staticreg();

   if (data == "__mapregister")
      return SC_REGISTER_MAP;

   if (data == "__worldregister")
      return SC_REGISTER_WORLD;

   if (data == "__globalregister")
      return SC_REGISTER_GLOBAL;

   if (data == "__maparray")
      return SC_REGISTERARRAY_MAP;

   if (data == "__worldarray")
      return SC_REGISTERARRAY_WORLD;

   if (data == "__globalarray")
      return SC_REGISTERARRAY_GLOBAL;

   ERROR_P("invalid storage-class: %s", data.c_str());
}

//
// SourceVariable::get_sc_autoreg
//
SourceVariable::StorageClass SourceVariable::get_sc_autoreg()
{
   if (target_type == TARGET_Hexen || target_type == TARGET_ZDoom)
      return SC_REGISTER;

   return SC_AUTO;
}

//
// SourceVariable::get_sc_staticreg
//
SourceVariable::StorageClass SourceVariable::get_sc_staticreg()
{
   if (target_type == TARGET_Hexen || target_type == TARGET_ZDoom)
      return SC_REGISTER_MAP;

   return SC_STATIC;
}

//
// SourceVariable::getClass
//
SourceVariable::StorageClass SourceVariable::getClass() const
{
   return sc;
}

//
// SourceVariable::getData
//
VariableData::Pointer SourceVariable::getData() const
{
   ObjectExpression::Pointer address;
   bigsint                   size = type->getSize(position);

   if (expr)
      address = expr;
   else
      address = ObjectExpression::create_value_symbol(nameObject, position);

   switch (sc)
   {
   case SC_AUTO:
      return VariableData::create_auto(size, address);

   case SC_CONSTANT:
      return VariableData::create_literal(size, address);

   case SC_REGISTER:
      return VariableData::create_register
             (size, VariableData::SR_LOCAL, address);

   case SC_REGISTER_GLOBAL:
      return VariableData::create_register
             (size, VariableData::SR_GLOBAL, address);

   case SC_REGISTER_MAP:
      return VariableData::create_register
             (size, VariableData::SR_MAP, address);

   case SC_REGISTER_WORLD:
      return VariableData::create_register
             (size, VariableData::SR_WORLD, address);

   case SC_REGISTERARRAY_GLOBAL:
      return VariableData::create_registerarray
             (size, VariableData::SRA_GLOBAL, address, NULL);

   case SC_REGISTERARRAY_MAP:
      return VariableData::create_registerarray
             (size, VariableData::SRA_MAP, address, NULL);

   case SC_REGISTERARRAY_WORLD:
      return VariableData::create_registerarray
             (size, VariableData::SRA_WORLD, address, NULL);

   case SC_STATIC:
      return VariableData::create_static(size, address);
   }

   ERROR(position, "invalid SC");
}

//
// SourceVariable::getNameObject
//
std::string const & SourceVariable::getNameObject() const
{
   return nameObject;
}

//
// SourceVariable::getNameSource
//
std::string const & SourceVariable::getNameSource() const
{
   return nameSource;
}

//
// SourceVariable::getType
//
VariableType::Reference SourceVariable::getType() const
{
   return type;
}

// EOF

