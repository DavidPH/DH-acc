//-----------------------------------------------------------------------------
//
// Copyright(C) 2013 David Hill
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
// Enumerated type handling.
//
//-----------------------------------------------------------------------------

#include "Enum.hpp"

#include "ObjectArchive.hpp"
#include "SourceException.hpp"


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

std::set<Type_Enumerated::EnumCR> Type_Enumerated::LoadSet;

std::set<Type_Enumerated::EnumData *> Type_Enumerated::EnumData::EnumDataSet;


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// Type_Enumerated::EnumData::EnumData
//
Type_Enumerated::EnumData::EnumData(Enum *type_, ContextKey name_, bool cxx_,
   bool scoped_) : name{name_}, valueMax{0}, valueMin{0}, baseType{Void}, type{type_},
   complete{false}, cxx{cxx_}, scoped{cxx_ && scoped_}, unscoped{cxx_ && !scoped_}
{
   EnumDataSet.insert(this);
}

//
// Type_Enumerated::EnumData::~EnumData
//
Type_Enumerated::EnumData::~EnumData()
{
   EnumDataSet.erase(this);
}

//
// Type_Enumerated::Type_Enumerated
//
Type_Enumerated::Type_Enumerated(ContextKey name, bool cxx, bool scoped) :
   data{new EnumData(this, name, cxx, scoped)}
{
}

//
// Type_Enumerated::~Type_Enumerated
//
Type_Enumerated::~Type_Enumerated()
{
   if(!isQual()) delete data;
}

//
// Type_Enumerated::addComplete
//
void Type_Enumerated::addComplete(Type const *baseType)
{
   if(data->complete) Error_Np("internal error: complete");

   data->baseType = static_cast<TypeCR>(baseType);
   data->complete = true;
}

//
// Type_Enumerated::addValue
//
void Type_Enumerated::addValue(Keyword name, bigsint value)
{
   if(data->complete) Error_Np("internal error: complete");

   if(data->values.count(name)) Error_Np("internal error: duplicate value");

   // Set min/max values.
   if(data->values.empty())
      data->valueMax = data->valueMin = value;
   else
   {
      if(data->valueMax < value) data->valueMax = value;
      if(data->valueMin > value) data->valueMin = value;
   }

   data->values.emplace(name, value);
}

//
// Type_Enumerated::getAlignment
//
bigsint Type_Enumerated::getAlignment() const
{
   if(!data->complete) Error_Np("internal error: incomplete");

   return data->baseType->getAlignment();
}

//
// Type_Enumerated::getBase
//
auto Type_Enumerated::getBase() const -> TypeCR
{
   if(!data->complete) Error_Np("internal error: incomplete");

   return data->baseType;
}

//
// Type_Enumerated::getNameMangleBase
//
void Type_Enumerated::getNameMangleBase(std::ostream &out, NameMangleStyle mangle) const
{
   out << 'E'; getNameMangleName(out, mangle);
}

//
// Type_Enumerated::getNameMangleName
//
void Type_Enumerated::getNameMangleName(std::ostream &out, NameMangleStyle mangle) const
{
   GetNameMangle(data->name, out, mangle);
}

//
// Type_Enumerated::getSizeBitsI
//
bigsint Type_Enumerated::getSizeBitsI() const
{
   if(!data->complete) Error_Np("internal error: incomplete");

   return data->baseType->getSizeBitsI();
}

//
// Type_Enumerated::getSizeBytes
//
bigsint Type_Enumerated::getSizeBytes() const
{
   if(!data->complete) Error_Np("internal error: incomplete");

   return data->baseType->getSizeBytes();
}

//
// Type_Enumerated::getSizePtr
//
bigsint Type_Enumerated::getSizePtr() const
{
   if(!data->complete) Error_Np("internal error: incomplete");

   return data->baseType->getSizePtr();
}

//
// Type_Enumerated::getSizeWords
//
bigsint Type_Enumerated::getSizeWords() const
{
   if(!data->complete) Error_Np("internal error: incomplete");

   return data->baseType->getSizeWords();
}

//
// Type_Enumerated::getValue
//
bigsint Type_Enumerated::getValue(Keyword name) const
{
   auto value = data->values.find(name);

   if(value == data->values.end()) Error_Np("internal error: not value");

   return value->second;
}

//
// Type_Enumerated::isSigned
//
bool Type_Enumerated::isSigned() const
{
   if(!data->complete) Error_Np("internal error: incomplete");

   return data->baseType->isSigned();
}

//
// Type_Enumerated::isValue
//
bool Type_Enumerated::isValue(Keyword name) const
{
   return data->values.count(name);
}

//
// Type_Enumerated::saveObject
//
ObjectSave &Type_Enumerated::saveObject(ObjectSave &save) const
{
   return Super::saveObject(save << KWRD_enum << data->name);
}

//
// Type_Enumerated::CreateC
//
auto Type_Enumerated::CreateC(ContextKey name) -> Reference
{
   return static_cast<Reference>(new Type_Enumerated(name, false, false));
}

//
// Type_Enumerated::CreateCXX
//
auto Type_Enumerated::CreateCXX(ContextKey name, bool scoped) -> Reference
{
   return static_cast<Reference>(new Type_Enumerated(name, false, scoped));
}

//
// Type_Enumerated::Find
//
auto Type_Enumerated::Find(ContextKey name) -> Pointer
{
   for(auto const &data : EnumData::EnumDataSet)
      if(data->name == name) return data->type;

   return nullptr;
}

//
// Type_Enumerated::Get
//
auto Type_Enumerated::Get(ContextKey name) -> EnumCR
{
   if(auto type = Find(name))
      return static_cast<EnumCR>(type);

   Error_p("internal error");
}

//
// Type_Enumerated::Load
//
ObjectLoad &Type_Enumerated::Load(ObjectLoad &load)
{
   std::set<EnumData *>::size_type size;

   load >> size;

   while(size--)
   {
      std::unordered_map<Keyword, bigsint> values;
      bigsint valueMax, valueMin;
      TypeCR baseType = Void;
      ContextKey name;
      bool complete, cxx, scoped;

      load >> name >> cxx >> scoped
         >> values >> valueMax >> valueMin >> baseType >> complete;

      auto type = Find(name);

      if(type)
      {
         if(!complete) continue;

         if(type->data->complete)
         {
            if(type->data->values != values || type->data->valueMax != valueMax ||
               type->data->valueMin != valueMin || type->data->baseType != baseType)
            {
               Error_p("conflicting enum definitions");
            }

            continue;
         }
      }
      else
      {
         type = cxx ? CreateCXX(name, scoped) : CreateC(name);

         LoadSet.insert(static_cast<EnumCR>(type));
      }

      type->data->values   = values;
      type->data->valueMax = valueMax;
      type->data->valueMin = valueMin;
      type->data->baseType = baseType;
      type->data->complete = complete;
   }

   return load;
}

//
// Type_Enumerated::LoadFinish
//
void Type_Enumerated::LoadFinish()
{
   LoadSet.clear();
}

//
// Type_Enumerated::Save
//
ObjectSave &Type_Enumerated::Save(ObjectSave &save)
{
   save << EnumData::EnumDataSet.size();

   for(auto const &data : EnumData::EnumDataSet)
      save << *data;

   return save;
}

//
// operator ObjectSave << Type_Enumerated::EnumData
//
ObjectSave &operator << (ObjectSave &save, Type_Enumerated::EnumData const &data)
{
   return save << data.name << data.cxx << data.scoped
      << data.values << data.valueMax << data.valueMin << data.baseType << data.complete;
}

// EOF

