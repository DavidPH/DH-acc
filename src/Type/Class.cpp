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
// Structure and union type handling.
//
//-----------------------------------------------------------------------------

#include "Class.hpp"

#include "MemFunc.hpp"

#include "ost_type.hpp"
#include "SourceException.hpp"


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// Type_Class::BaseClass::BaseClass
//
Type_Class::BaseClass::BaseClass(Clas const *base_, Access cont_, bool virt_) :
   base{base_}, cont{cont_}, virt{virt_}
{
}

//
// Type_Class::DataMember::DataMember
//
Type_Class::DataMember::DataMember(bigsint offs_, Type const *type_, Keyword name_,
   Access cont_, bool muta_) : offs{offs_}, type{type_}, name{name_}, cont{cont_},
   muta{muta_}
{
}

//
// Type_Class::FuncMember::FuncMember
//
Type_Class::FuncMember::FuncMember(bigsint offs_, MFun const *type_, Keyword name_,
   Access cont_, bool pure_) : offs{offs_}, type{type_}, name{name_}, cont{cont_},
   pure{pure_}
{
}

//
// Type_Class::ClassData::ClassData
//
Type_Class::ClassData::ClassData(ContextKey name_, bool structure_) :
   sizeBytes{0}, sizePtr{0}, sizeWords{0}, mfnType{nullptr}, ptmType{nullptr},
   name{name_}, complete{false}, structure{structure_}
{
}

//
// Type_Class::Type_Class
//
Type_Class::Type_Class(ContextKey name, bool structure) : data{new ClassData(name, structure)}
{
}

//
// Type_Class::~Type_Class
//
Type_Class::~Type_Class()
{
   if(!isQual()) delete data;
}

//
// Type_Class::addBaseClass
//
void Type_Class::addBaseClass(Clas const *base, Access cont, bool virt)
{
   if(data->complete) Error_Np("internal error: complete");

   data->baseClass.emplace_back(base, cont, virt);

   data->sizeBytes += base->getSizeBytes();
}

//
// Type_Class::addComplete
//
void Type_Class::addComplete()
{
   if(data->complete) Error_Np("internal error: complete");

   if(!data->sizeBytes) data->sizeBytes = 1;

   if(Target == TARGET_MageCraft)
   {
      data->sizePtr = data->sizeWords = (data->sizeBytes + 3) / 4;
      data->sizeBytes = data->sizeWords * 4;
   }
   else
   {
      data->sizePtr = data->sizeWords = data->sizeBytes;
   }

   data->complete = true;
}

//
// Type_Class::addDataMember
//
void Type_Class::addDataMember(bigsint offs, Type const *type, Keyword name,
   Access cont, bool muta)
{
   if(data->complete) Error_Np("internal error: complete");

   data->dataMember.emplace_back(offs, type, name, cont, muta);

   if(type->isComplete())
   {
      bigsint size = offs + type->getSizeBytes();

      if(data->sizeBytes < size)
         data->sizeBytes = size;
   }
}

//
// Type_Class::addFuncMember
//
void Type_Class::addFuncMember(bigsint offs, MFun const *type, Keyword name,
   Access cont, bool pure)
{
   if(data->complete) Error_Np("internal error: complete");

   data->funcMember.emplace_back(offs, type, name, cont, pure);
}

//
// Type_Class::addNameTypedef
//
void Type_Class::addNameTypedef(ContextKey name)
{
   if(!data->nameTypedef) data->nameTypedef = name;
}

//
// Type_Class::getAlignment
//
bigsint Type_Class::getAlignment() const
{
   if(Target == TARGET_MageCraft) return 4;

   return 1;
}

//
// Type_Class::getBaseClassBegin
//
auto Type_Class::getBaseClassBegin() const -> BaseCP
{
   if(data->baseClass.empty())
      return nullptr;
   else
      return &*data->baseClass.begin();
}

//
// Type_Class::getBaseClassCount
//
bigsint Type_Class::getBaseClassCount() const
{
   return data->baseClass.size();
}

//
// Type_Class::getBaseClassEnd
//
auto Type_Class::getBaseClassEnd() const -> BaseCP
{
   if(data->baseClass.empty())
      return nullptr;
   else
      return &*data->baseClass.end();
}

//
// Type_Class::getDataMember
//
auto Type_Class::getDataMember(Keyword name) const -> DMemCR
{
   for(auto const &mem : data->dataMember)
      if(mem.name == name) return mem;

   Error_Np("internal error: not data-member");
}

//
// Type_Class::getFuncMember
//
auto Type_Class::getFuncMember(Keyword name) const -> FMemCR
{
   for(auto const &mem : data->funcMember)
      if(mem.name == name) return mem;

   Error_Np("internal error: not func-member");
}

//
// Type_Class::getNameMangleBase
//
void Type_Class::getNameMangleBase(std::ostream &out, NameMangleStyle mangle) const
{
   out << (data->structure ? 'S' : 'U');
   getNameMangleName(out, mangle);
}

//
// Type_Class::getNameMangleName
//
void Type_Class::getNameMangleName(std::ostream &out, NameMangleStyle mangle) const
{
   if(!data->name.getKeyword() && data->nameTypedef)
      GetNameMangle(data->nameTypedef, out, mangle);
   else
      GetNameMangle(data->name, out, mangle);
}

//
// Type_Class::getSizeBytes
//
bigsint Type_Class::getSizeBytes() const
{
   if(!data->complete) Error_Np("internal error: incomplete");

   return data->sizeBytes;
}

//
// Type_Class::getSizePtr
//
bigsint Type_Class::getSizePtr() const
{
   if(!data->complete) Error_Np("internal error: incomplete");

   return data->sizePtr;
}

//
// Type_Class::getSizeWords
//
bigsint Type_Class::getSizeWords() const
{
   if(!data->complete) Error_Np("internal error: incomplete");

   return data->sizeWords;
}

//
// Type_Class::isDataMember
//
bool Type_Class::isDataMember(Keyword name) const
{
   for(auto const &mem : data->dataMember)
      if(mem.name == name) return true;

   return false;
}

//
// Type_Class::isFuncMember
//
int Type_Class::isFuncMember(Keyword name) const
{
   int n = 0;

   for(auto const &mem : data->funcMember)
      if(mem.name == name) ++n;

   return n;
}

// EOF

