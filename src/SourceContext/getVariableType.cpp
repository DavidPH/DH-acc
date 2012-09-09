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
// SourceContext::addVariableType_struct
//
void SourceContext::addVariableType_struct(std::string const &name)
{
   if(name.empty()) return;

   for(size_t i = 0; i < structNames.size(); ++i)
      if(structNames[i] == name) return;

   VariableType::Reference type = VariableType::get_bt_struct(name);

   structNames.push_back(name);
   structTypes.push_back(type);
}

//
// SourceContext::addVariableType_union
//
void SourceContext::addVariableType_union(std::string const &name)
{
   if(name.empty()) return;

   for(size_t i = 0; i < unionNames.size(); ++i)
      if(unionNames[i] == name) return;

   VariableType::Reference type = VariableType::get_bt_union(name);

   unionNames.push_back(name);
   unionTypes.push_back(type);
}

//
// SourceContext::getVariableType
//
VariableType::Reference SourceContext::getVariableType
(std::string const &name, SourcePosition const &pos)
{
   VariableType::Pointer type = getVariableTypeNull(name);

   if (!type)
      Error_NP("no such type: %s", name.c_str());

   return static_cast<VariableType::Reference>(type);
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
(std::string const &name, bool block, SourcePosition const &pos)
{
   VariableType::Pointer type = getVariableType_enum(name);

   if (type)
   {
      if (block)
	 {
         if (type->getComplete())
            Error_NP("enum redefined: %s", name.c_str());
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
      enumTypes.push_back(static_cast<VariableType::Reference>(type));
   }

   return static_cast<VariableType::Reference>(type);
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
      structTypes.push_back(static_cast<VariableType::Reference>(type));
   }

   return static_cast<VariableType::Reference>(type);
}

//
// SourceContext::getVariableType_struct
//
VariableType::Reference SourceContext::getVariableType_struct(
   std::string const &name, VariableType::VecStr const &names,
   VariableType::Vector const &types, SourcePosition const &pos)
{
   VariableType::Pointer type;

   if(!name.empty()) for(size_t i = 0; i < structNames.size(); ++i)
      if(structNames[i] == name) {type = structTypes[i]; break;}

   if(!type)
   {
      type = VariableType::get_bt_struct(name.empty() ? makeLabel() : name);

      structNames.push_back(name);
      structTypes.push_back(static_cast<VariableType::Reference>(type));
   }

   if(type->getComplete())
      Error_NP("struct %s redefined", name.c_str());

   type->makeComplete(names, types);

   return static_cast<VariableType::Reference>(type);
}

//
// SourceContext::getVariableType_typedef
//
VariableType::Reference SourceContext::getVariableType_typedef(
   std::string const &name, SourcePosition const &pos)
{
   for(size_t i = 0; i < typedefNames.size(); ++i)
      if(typedefNames[i] == name) return typedefTypes[i];

   if(parent) return parent->getVariableType_typedef(name, pos);

   Error_NP("expected typedef-name");
}

//
// SourceContext::getVariableType_typedef
//
VariableType::Reference SourceContext::getVariableType_typedef
(std::string const &name, VariableType *type, SourcePosition const &pos)
{
   for (size_t i = 0; i < typedefNames.size(); ++i)
      if (typedefNames[i] == name)
         Error_NP("typedef redefined: %s", name.c_str());

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
      unionTypes.push_back(static_cast<VariableType::Reference>(type));
   }

   return static_cast<VariableType::Reference>(type);
}

//
// SourceContext::getVariableType_union
//
VariableType::Reference SourceContext::getVariableType_union
(std::string const &name, VariableType::VecStr const &names,
 VariableType::Vector const &types, SourcePosition const &pos)
{
   VariableType::Pointer type;

   if(!name.empty()) for(size_t i = 0; i < unionNames.size(); ++i)
      if(unionNames[i] == name) {type = unionTypes[i]; break;}

   if(!type)
   {
      type = VariableType::get_bt_union(name.empty() ? makeLabel() : name);

      unionNames.push_back(name);
      unionTypes.push_back(static_cast<VariableType::Reference>(type));
   }

   if(type->getComplete())
      Error_NP("union %s redefined", name.c_str());

   type->makeComplete(names, types);

   return static_cast<VariableType::Reference>(type);
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

//
// SourceContext::isVariableType_typedef
//
bool SourceContext::isVariableType_typedef(std::string const &name) const
{
   for(size_t i = 0; i < typedefNames.size(); ++i)
      if(typedefNames[i] == name) return true;

   if(parent) return parent->isVariableType_typedef(name);

   return false;
}

// EOF

