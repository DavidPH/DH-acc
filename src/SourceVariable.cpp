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
#include "SourceException.hpp"
#include "SourceTokenC.hpp"
#include "VariableData.hpp"
#include "VariableType.hpp"



//
// SourceVariable::SourceVariable
//
SourceVariable::
SourceVariable(SourceVariable const &var)
               : Super(var), position(var.position), nameObject(var.nameObject),
                 nameSource(var.nameSource), expr(var.expr), type(var.type),
                 sc(var.sc)
{
}

//
// SourceVariable::SourceVariable
//
SourceVariable::
SourceVariable(std::string const &_nameSource, VariableType const *_type,
               ObjectExpression *_expr, SourcePosition const &_position)
               : position(_position), nameSource(_nameSource), expr(_expr),
                 type(_type), sc(SC_CONSTANT)
{
}

//
// SourceVariable::SourceVariable
//
SourceVariable::
SourceVariable(std::string const &_nameSource, VariableType const *_type,
               std::string const &_nameObject, SourcePosition const &_position)
               : position(_position), nameObject(_nameObject),
                 nameSource(_nameSource), type(_type), sc(SC_CONSTANT)
{
}

//
// ourceVariable::SourceVariable
//
SourceVariable::
SourceVariable(std::string const &_nameSource, VariableType const *_type,
               std::string const &_nameObject, StorageClass _sc,
               SourcePosition const &_position)
               : position(_position), nameObject(_nameObject),
                 nameSource(_nameSource), type(_type), sc(_sc)
{
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
SourceVariable::Pointer SourceVariable::
create_constant(std::string const &nameSource, VariableType const *type,
                ObjectExpression *expr, SourcePosition const &position)
{
   return new SourceVariable(nameSource, type, expr, position);
}

//
// SourceVariable::create_constant
//
SourceVariable::Pointer SourceVariable::
create_constant(std::string const &nameSource, VariableType const *type,
                std::string const &nameObject, SourcePosition const &position)
{
   return new SourceVariable(nameSource, type, nameObject, position);
}

//
// SourceVariable::create_literal
//
SourceVariable::Pointer SourceVariable::
create_literal(VariableType const *type, ObjectExpression *expr,
               SourcePosition const &position)
{
   return new SourceVariable("", type, expr, position);
}

//
// SourceVariable::create_literal
//
SourceVariable::Pointer SourceVariable::
create_literal(VariableType const *type, std::string const &nameObject,
               SourcePosition const &position)
{
   return new SourceVariable("", type, nameObject, position);
}

//
// SourceVariable::create_variable
//
SourceVariable::Pointer SourceVariable::
create_variable(std::string const &nameSource, VariableType const *type,
                std::string const &nameObject, StorageClass sc,
                SourcePosition const &position)
{
   return new SourceVariable(nameSource, type, nameObject, sc, position);
}

//
// SourceVariable::get_StorageClass
//
SourceVariable::StorageClass SourceVariable::
get_StorageClass(SourceTokenC const &token)
{
   if (token.getData() == "auto")
      return SC_AUTO;

   if (token.getData() == "register")
      return SC_REGISTER;

   if (token.getData() == "register_global")
      return SC_REGISTER_GLOBAL;

   if (token.getData() == "register_map")
      return SC_REGISTER_MAP;

   if (token.getData() == "register_world")
      return SC_REGISTER_WORLD;

   if (token.getData() == "registerarray_global")
      return SC_REGISTERARRAY_GLOBAL;

   if (token.getData() == "registerarray_map")
      return SC_REGISTERARRAY_MAP;

   if (token.getData() == "registerarray_world")
      return SC_REGISTERARRAY_WORLD;

   if (token.getData() == "static")
      return SC_STATIC;

   throw SourceException("invalid storage-class '" + token.getData() + "'", token.getPosition(), "SourceVariable");
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
   bigsint                   size = type->size(position);

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

   throw SourceException("invalid SC", position, "SourceVariable::getData");
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
VariableType const * SourceVariable::getType() const
{
   return type;
}

// EOF

