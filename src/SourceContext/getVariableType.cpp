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
// Source-level context handling of variable types.
//
//-----------------------------------------------------------------------------

#include "../SourceContext.hpp"

#include "../SourceException.hpp"
#include "../SourceTokenC.hpp"
#include "../VariableType.hpp"



//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceContext::getVariableType
//
VariableType::Reference SourceContext::
getVariableType(std::string const &name, SourcePosition const &position)
{
   VariableType::Pointer type = getVariableTypeNull(name);

   if (!type)
      throw SourceException("no such type '" + name + "'", position, __func__);

   return type.ref();
}

//
// SourceContext::getVariableType_enum
//
VariableType::Pointer SourceContext::
getVariableType_enum(std::string const &name)
{
   if (name.empty()) return NULL;

   for (size_t i = 0; i < enumNames.size(); ++i)
      if (enumNames[i] == name)
         return enumTypes[i];

   if (parent) return parent->getVariableType_enum(name);

   return NULL;
}

//
// SourceContext::getVariableType_enum
//
VariableType::Reference SourceContext::getVariableType_enum
(std::string const &name, bool block, SourcePosition const &position)
{
   VariableType::Pointer type = getVariableType_enum(name);

   if (type)
   {
      if (block)
	 {
         if (type->getComplete())
            throw SourceException("enum redefined", position, __func__);
         else
            type->makeComplete();
      }
   }
   else
   {
      type = VariableType::get_bt_enum(name.empty() ? makeLabel() : name);

      if (block)
         type->makeComplete();

      enumNames.push_back(name);
      enumTypes.push_back(type.ref());
   }

   return type.ref();
}

//
// SourceContext::getVariableType_struct
//
VariableType::Pointer SourceContext::getVariableType_struct
(std::string const &name)
{
   if (name.empty()) return NULL;

   for (size_t i = 0; i < structNames.size(); ++i)
      if (structNames[i] == name)
         return structTypes[i];

   if (parent) return parent->getVariableType_struct(name);

   return NULL;
}

//
// SourceContext::getVariableType_struct
//
VariableType::Reference SourceContext::getVariableType_struct
(std::string const &name, SourcePosition const &)
{
   VariableType::Pointer type = getVariableType_struct(name);

   if (!type)
   {
      type = VariableType::get_bt_struct(name.empty() ? makeLabel() : name);

      structNames.push_back(name);
      structTypes.push_back(type.ref());
   }

   return type.ref();
}

//
// SourceContext::getVariableType_struct
//
VariableType::Reference SourceContext::getVariableType_struct
(std::string const &name, VariableType::VecStr const &names,
 VariableType::Vector const &types, SourcePosition const &position)
{
   VariableType::Reference type = getVariableType_struct(name, position);

   if (type->getComplete())
      throw SourceException("struct redefined", position, __func__);

   type->makeComplete(names, types);

   return type;
}

//
// SourceContext::getVariableType_typedef
//
VariableType::Reference SourceContext::getVariableType_typedef
(std::string const &name, VariableType *type, SourcePosition const &position)
{
   for (size_t i = 0; i < typedefNames.size(); ++i)
      if (typedefNames[i] == name)
         throw SourceException("typedef redefined", position, __func__);

   typedefNames.push_back(name);
   typedefTypes.push_back(static_cast<VariableType::Reference>(type));

   return static_cast<VariableType::Reference>(type);
}

//
// SourceContext::getVariableType_union
//
VariableType::Pointer SourceContext::getVariableType_union
(std::string const &name)
{
   if (name.empty()) return NULL;

   for (size_t i = 0; i < unionNames.size(); ++i)
      if (unionNames[i] == name)
         return unionTypes[i];

   if (parent) return parent->getVariableType_union(name);

   return NULL;
}

//
// SourceContext::getVariableType_union
//
VariableType::Reference SourceContext::getVariableType_union
(std::string const &name, SourcePosition const &)
{
   VariableType::Pointer type = getVariableType_union(name);

   if (!type)
   {
      type = VariableType::get_bt_union(name.empty() ? makeLabel() : name);

      unionNames.push_back(name);
      unionTypes.push_back(type.ref());
   }

   return type.ref();
}

//
// SourceContext::getVariableType_union
//
VariableType::Reference SourceContext::getVariableType_union
(std::string const &name, VariableType::VecStr const &names,
 VariableType::Vector const &types, SourcePosition const &position)
{
   VariableType::Reference type = getVariableType_union(name, position);

   if (type->getComplete())
      throw SourceException("union redefined", position, __func__);

   type->makeComplete(names, types);

   return type;
}

//
// SourceContext::getVariableTypeNull
//
VariableType::Pointer SourceContext::getVariableTypeNull
(std::string const &name)
{
   size_t i;

   if (name.empty()) return NULL;

   for (i = 0; i < typedefNames.size(); ++i)
      if (typedefNames[i] == name)
         return typedefTypes[i];

   for (i = 0; i < enumNames.size(); ++i)
      if (enumNames[i] == name)
         return enumTypes[i];

   for (i = 0; i < structNames.size(); ++i)
      if (structNames[i] == name)
         return structTypes[i];

   for (i = 0; i < unionNames.size(); ++i)
      if (unionNames[i] == name)
         return unionTypes[i];

   if (parent) return parent->getVariableTypeNull(name);

   return NULL;
}

// EOF

