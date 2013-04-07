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

#include "SourceException.hpp"


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// Type_Enumerated::EnumData::EnumData
//
Type_Enumerated::EnumData::EnumData(ContextKey name_, bool cxx_, bool scoped_) :
   name{name_}, valueMax{0}, valueMin{0}, baseType{Void}, complete{false},
   cxx{cxx_}, scoped{cxx_ && scoped_}, unscoped{cxx_ && !scoped_}
{
}

//
// Type_Enumerated::Type_Enumerated
//
Type_Enumerated::Type_Enumerated(ContextKey name, bool cxx, bool scoped) :
   data{new EnumData(name, cxx, scoped)}
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
// Type_Enumerated::getSizeBits
//
bigsint Type_Enumerated::getSizeBits() const
{
   if(!data->complete) Error_Np("internal error: incomplete");

   return data->baseType->getSizeBits();
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

// EOF

